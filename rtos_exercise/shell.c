
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rtos_exercise.h"
#include "shell.h"

/* FreeRTOS objects */
#define MSGQUEUE_OBJECTS 16
#define CMD_BUF_SIZE     64

typedef struct {
    uint8_t Idx;
    char SubCmd[CMD_BUF_SIZE]; // Store subcommand or arguments as a string
} MSGQUEUE_OBJ_t;

static osMessageQueueId_t mid_MsgQueue;
static osThreadId_t tid_UserAction;
static osThreadId_t tid_processReq;

const osThreadAttr_t ua_attributes = {
  .name = "User Action",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t pr_attributes = {
  .name = "process Req",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

static void Thread_UserAction(void *argument);
static void Thread_processReq(void *argument);
static int print_help(void *arg);
static int clearScreen(void *arg);

extern UART_HandleTypeDef huart2;

#define HISTORY_SIZE 10

#define RX_BUF_SIZE 128

static uint8_t rxBuf[RX_BUF_SIZE];
static volatile int rxHead = 0;
static volatile int rxTail = 0;

static char history[HISTORY_SIZE][CMD_BUF_SIZE];
static int historyCount = 0;
static int historyIndex = -1; // -1 means no history currently selected

static struct cmd_t *shell_cmds;
static uint32_t num_cmds = 0;
static bool shell_initialized = false;

TaskStatus_t statusArray[16];

int printTaskStatusDetailed(void* arg) {
    UBaseType_t taskCount;
    uint32_t totalRunTime;

    taskCount = uxTaskGetSystemState(statusArray, 16, &totalRunTime);

    for (UBaseType_t i = 0; i < taskCount; i++) {
        printf("%-15s: State=%03d\t Prio=%03lu\t Stack=%03u\t RunTime=%03lu\t ",
               statusArray[i].pcTaskName,
               statusArray[i].eCurrentState,
               statusArray[i].uxCurrentPriority,
               statusArray[i].usStackHighWaterMark,
               statusArray[i].ulRunTimeCounter);
    }
}

static int print_help(void *arg) {
    printf("command (%d) help:\n", num_cmds);
    for (int i = 0; i < num_cmds; i++) {
        printf("  %s: %s\n", shell_cmds[i].name, shell_cmds[i].help);
    }
    return 0;
}

static int clearScreen(void *arg) {
    const char *clr = "\033[2J\033[H"; 
    // \033[2J clears the screen
    // \033[H moves cursor to home (row 0, col 0)
    HAL_UART_Transmit(&huart2, (uint8_t*)clr, strlen(clr), HAL_MAX_DELAY);
    return 0;
}

/* Interrupt callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        int next = (rxHead + 1) % RX_BUF_SIZE;
        if (next != rxTail) { // avoid overflow
            rxBuf[rxHead] = huart->Instance->DR; // received byte
            rxHead = next;
        }
        // restart interrupt reception
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&rxBuf[rxHead], 1);
    }
}

/* Get next char from buffer (blocking until available) */
static uint8_t uartGetChar(void) {
    while (rxHead == rxTail) {
        osDelay(1); // yield until data arrives
    }
    uint8_t ch = rxBuf[rxTail];
    rxTail = (rxTail + 1) % RX_BUF_SIZE;
    return ch;
}

static void getUserInput(char *buf, int maxLen) {
    int idx = 0;

    const char *prompt = "$ ";
    HAL_UART_Transmit(&huart2, (uint8_t*)prompt, strlen(prompt), HAL_MAX_DELAY);

    while (1) {
        uint8_t ch = uartGetChar();

        if (ch == '\r' || ch == '\n') {
            HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
            buf[idx] = '\0';

            if (strlen(buf) > 0) {
                strncpy(history[historyCount % HISTORY_SIZE], buf, CMD_BUF_SIZE);
                historyCount++;
                historyIndex = -1;
            }
            return;
        }
        else if (ch == 0x7F || ch == '\b') {
            if (idx > 0) {
                idx--;
                HAL_UART_Transmit(&huart2, (uint8_t*)"\b \b", 3, HAL_MAX_DELAY);
            }
        }
        else if (ch == '\t') {
            buf[idx] = '\0';
            for (int i = 0; i < num_cmds; i++) {
                if (strncmp(buf, shell_cmds[i].name, idx) == 0) {
                    const char *cmd = shell_cmds[i].name;
                    int remaining = strlen(cmd) - idx;
                    strncpy(&buf[idx], &cmd[idx], remaining);
                    idx += remaining;
                    HAL_UART_Transmit(&huart2, (uint8_t*)&cmd[idx - remaining], remaining, HAL_MAX_DELAY);
                    break;
                }
            }
        }
        else if (ch == '\033') {
            uint8_t seq1 = uartGetChar();
            uint8_t seq2 = uartGetChar();

            if (seq1 == '[' && seq2 == 'A') {
                // Up Arrow
                if (historyCount > 0) {
                    if (historyIndex < historyCount - 1) historyIndex++;
                    strcpy(buf, history[(historyCount - 1 - historyIndex) % HISTORY_SIZE]);
                    idx = strlen(buf);

                    const char *clrLine = "\r\033[K$ ";
                    HAL_UART_Transmit(&huart2, (uint8_t*)clrLine, strlen(clrLine), HAL_MAX_DELAY);
                    HAL_UART_Transmit(&huart2, (uint8_t*)buf, idx, HAL_MAX_DELAY);
                }
            } else if (seq1 == '[' && seq2 == 'B') {
                // Down Arrow
                if (historyIndex > 0) {
                    historyIndex--;
                    strcpy(buf, history[(historyCount - 1 - historyIndex) % HISTORY_SIZE]);
                    idx = strlen(buf);

                    const char *clrLine = "\r\033[K$ ";
                    HAL_UART_Transmit(&huart2, (uint8_t*)clrLine, strlen(clrLine), HAL_MAX_DELAY);
                    HAL_UART_Transmit(&huart2, (uint8_t*)buf, idx, HAL_MAX_DELAY);
                } else {
                    historyIndex = -1;
                    idx = 0;
                    buf[0] = '\0';
                    const char *clrLine = "\r\033[K$ ";
                    HAL_UART_Transmit(&huart2, (uint8_t*)clrLine, strlen(clrLine), HAL_MAX_DELAY);
                }
            }
        }
        else {
            if (idx < maxLen - 1) {
                buf[idx++] = ch;
                HAL_UART_Transmit(&huart2, &ch, 1, HAL_MAX_DELAY);
            }
        }
    }
}

static uint8_t getCommandId(const char *buf) {
    for (int i = 0; i < num_cmds; i++) {
        if (strncmp(buf, shell_cmds[i].name, strlen(shell_cmds[i].name)) == 0) {
            return shell_cmds[i].id;
        }
    }
    return 0xFF; // Invalid command
}

int parseSubCommand(char *buf, char **argv, int maxArgs) {
    int argc = 0;
    char *token = strtok(buf, " ");
    while (token != NULL && argc < maxArgs) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    return argc;
}

/* User input thread */
static void Thread_UserAction(void *argument) {
    MSGQUEUE_OBJ_t msg;
    char Buf[CMD_BUF_SIZE];

    while (1) {      
        getUserInput(Buf, CMD_BUF_SIZE);
        // Tokenize into arguments
        char *argv[4];
        int argc = parseSubCommand(Buf, argv, 4);
        if (argc > 0) {
            msg.Idx = getCommandId(argv[0]);  // main command
            // Store subcommand string in Buf for later
            if(msg.Idx == 0xFF) {
                printf("Unknown command: %s\n", argv[0]);
                continue;
            }
            strncpy(msg.SubCmd, (argc > 1) ? argv[1] : "", CMD_BUF_SIZE);
            osMessageQueuePut(mid_MsgQueue, &msg, 0U, 0U);
            osThreadYield();
        }
    }
}

/* Request processor thread */
static void Thread_processReq(void *argument) {
    MSGQUEUE_OBJ_t msg;
    osStatus_t status;
    while (1) {
        status = osMessageQueueGet(mid_MsgQueue, &msg, NULL, osWaitForever);
        if (status == osOK) {
            if (shell_cmds[msg.Idx].handler != NULL && msg.Idx < num_cmds) {
                shell_cmds[msg.Idx].handler(msg.SubCmd);
            }
        }
    }
}

int user_shell_init(void) {
    int ret = 0;
    mid_MsgQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_OBJ_t), NULL);
    tid_UserAction = osThreadNew(Thread_UserAction, NULL, &ua_attributes);
    tid_processReq = osThreadNew(Thread_processReq, NULL, &pr_attributes);

    /* register basic commands */
    num_cmds = 0;
    shell_cmds = malloc(sizeof(struct cmd_t) * NUM_COMMANDS);
    if(shell_cmds == NULL) {
        printf("Failed to allocate memory for shell commands\n");
        return -1; // Allocation failed
    }

    printf("Shell App.\n - 'help' for available commands.\n");
    shell_initialized = true;
    
    ret = register_command(CMD_HELP, "help", print_help, "Display available commands");
    ret |= register_command(CMD_CLEAN, "clear", clearScreen, "Clear the screen");
    ret |= register_command(CMD_PS, "ps", printTaskStatusDetailed, "Show task status");
    return ret;
}

int register_command(enum cmd_id id, const char *name, 
                    cmd_handler_t handler, 
                    const char *help)
{
    if(!shell_initialized) {
        printf("Shell not initialized. Call user_shell_init() first.\n");
        return -1;
    }

    if(num_cmds >= NUM_COMMANDS) {
        printf("Command limit reached. Cannot register more commands.\n");
        return -1; // No space for more commands
    }

    shell_cmds[id].id = id;
    shell_cmds[id].name = strdup(name); // Duplicate string to avoid issues
    shell_cmds[id].handler = handler;
    shell_cmds[id].help = strdup(help);
    num_cmds++;
    return 0;
}

