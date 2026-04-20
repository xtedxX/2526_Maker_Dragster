################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/bldc_esc.c \
../Core/Src/dma.c \
../Core/Src/gpio.c \
../Core/Src/hc05_bt.c \
../Core/Src/i2c.c \
../Core/Src/main.c \
../Core/Src/mma8452q.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g4xx.c \
../Core/Src/tc1602a_lcd.c \
../Core/Src/tim.c \
../Core/Src/usart.c \
../Core/Src/ws2812.c 

OBJS += \
./Core/Src/bldc_esc.o \
./Core/Src/dma.o \
./Core/Src/gpio.o \
./Core/Src/hc05_bt.o \
./Core/Src/i2c.o \
./Core/Src/main.o \
./Core/Src/mma8452q.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g4xx.o \
./Core/Src/tc1602a_lcd.o \
./Core/Src/tim.o \
./Core/Src/usart.o \
./Core/Src/ws2812.o 

C_DEPS += \
./Core/Src/bldc_esc.d \
./Core/Src/dma.d \
./Core/Src/gpio.d \
./Core/Src/hc05_bt.d \
./Core/Src/i2c.d \
./Core/Src/main.d \
./Core/Src/mma8452q.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g4xx.d \
./Core/Src/tc1602a_lcd.d \
./Core/Src/tim.d \
./Core/Src/usart.d \
./Core/Src/ws2812.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/bldc_esc.cyclo ./Core/Src/bldc_esc.d ./Core/Src/bldc_esc.o ./Core/Src/bldc_esc.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/hc05_bt.cyclo ./Core/Src/hc05_bt.d ./Core/Src/hc05_bt.o ./Core/Src/hc05_bt.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mma8452q.cyclo ./Core/Src/mma8452q.d ./Core/Src/mma8452q.o ./Core/Src/mma8452q.su ./Core/Src/stm32g4xx_hal_msp.cyclo ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_hal_msp.su ./Core/Src/stm32g4xx_it.cyclo ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/stm32g4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g4xx.cyclo ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/system_stm32g4xx.su ./Core/Src/tc1602a_lcd.cyclo ./Core/Src/tc1602a_lcd.d ./Core/Src/tc1602a_lcd.o ./Core/Src/tc1602a_lcd.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su ./Core/Src/ws2812.cyclo ./Core/Src/ws2812.d ./Core/Src/ws2812.o ./Core/Src/ws2812.su

.PHONY: clean-Core-2f-Src

