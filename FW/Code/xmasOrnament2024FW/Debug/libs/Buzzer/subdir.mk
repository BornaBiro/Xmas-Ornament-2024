################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/Buzzer/Buzzer.c 

OBJS += \
./libs/Buzzer/Buzzer.o 

C_DEPS += \
./libs/Buzzer/Buzzer.d 


# Each subdirectory must supply rules for building sources it contributes
libs/Buzzer/%.o libs/Buzzer/%.su libs/Buzzer/%.cyclo: ../libs/Buzzer/%.c libs/Buzzer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C011xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I"/Users/borna/STM32CubeIDE/workspace_1.17.0/xmasOrnament2024FW/libs/WS2812" -I"/Users/borna/STM32CubeIDE/workspace_1.17.0/xmasOrnament2024FW/libs/Buzzer" -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-libs-2f-Buzzer

clean-libs-2f-Buzzer:
	-$(RM) ./libs/Buzzer/Buzzer.cyclo ./libs/Buzzer/Buzzer.d ./libs/Buzzer/Buzzer.o ./libs/Buzzer/Buzzer.su

.PHONY: clean-libs-2f-Buzzer

