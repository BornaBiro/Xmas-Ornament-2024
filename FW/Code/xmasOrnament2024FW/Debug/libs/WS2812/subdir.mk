################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/WS2812/WS2812.c 

OBJS += \
./libs/WS2812/WS2812.o 

C_DEPS += \
./libs/WS2812/WS2812.d 


# Each subdirectory must supply rules for building sources it contributes
libs/WS2812/%.o libs/WS2812/%.su libs/WS2812/%.cyclo: ../libs/WS2812/%.c libs/WS2812/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32C011xx -c -I../Core/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc -I../Drivers/STM32C0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32C0xx/Include -I../Drivers/CMSIS/Include -I"/Users/borna/STM32CubeIDE/workspace_1.17.0/xmasOrnament2024FW/libs/WS2812" -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-libs-2f-WS2812

clean-libs-2f-WS2812:
	-$(RM) ./libs/WS2812/WS2812.cyclo ./libs/WS2812/WS2812.d ./libs/WS2812/WS2812.o ./libs/WS2812/WS2812.su

.PHONY: clean-libs-2f-WS2812

