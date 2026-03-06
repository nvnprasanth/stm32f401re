################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rtos_exercise/task_demo.c 

OBJS += \
./rtos_exercise/task_demo.o 

C_DEPS += \
./rtos_exercise/task_demo.d 


# Each subdirectory must supply rules for building sources it contributes
rtos_exercise/%.o rtos_exercise/%.su rtos_exercise/%.cyclo: ../rtos_exercise/%.c rtos_exercise/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/nvnpr/Documents/FreeRTOS/stm32f4_freertos/rtos_exercise/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-rtos_exercise

clean-rtos_exercise:
	-$(RM) ./rtos_exercise/task_demo.cyclo ./rtos_exercise/task_demo.d ./rtos_exercise/task_demo.o ./rtos_exercise/task_demo.su

.PHONY: clean-rtos_exercise

