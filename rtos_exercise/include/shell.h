#ifndef __SHELL_H__
#define __SHELL_H__

typedef int (*cmd_handler_t)(void *arg);

enum cmd_id {
    CMD_HELP,
    CMD_CLEAN,
    CMD_PS, 
    CMD_LED,
    CMD_TASK,
    NUM_COMMANDS = 16
};

struct cmd_t {
    enum cmd_id id;
    char *name;
    cmd_handler_t handler;
    char *help;
};

int user_shell_init(void);
int register_command(enum cmd_id id, const char *name, cmd_handler_t handler, const char *help);
int parseSubCommand(char *buf, char **argv, int maxArgs);

#endif // __SHELL_H__
