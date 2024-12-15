################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Drivers/bmi088.c \
../Core/Src/Drivers/bmp388.c \
../Core/Src/Drivers/lis3mdl.c \
../Core/Src/Drivers/usb.c 

OBJS += \
./Core/Src/Drivers/bmi088.o \
./Core/Src/Drivers/bmp388.o \
./Core/Src/Drivers/lis3mdl.o \
./Core/Src/Drivers/usb.o 

C_DEPS += \
./Core/Src/Drivers/bmi088.d \
./Core/Src/Drivers/bmp388.d \
./Core/Src/Drivers/lis3mdl.d \
./Core/Src/Drivers/usb.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Drivers/%.o Core/Src/Drivers/%.su Core/Src/Drivers/%.cyclo: ../Core/Src/Drivers/%.c Core/Src/Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Drivers

clean-Core-2f-Src-2f-Drivers:
	-$(RM) ./Core/Src/Drivers/bmi088.cyclo ./Core/Src/Drivers/bmi088.d ./Core/Src/Drivers/bmi088.o ./Core/Src/Drivers/bmi088.su ./Core/Src/Drivers/bmp388.cyclo ./Core/Src/Drivers/bmp388.d ./Core/Src/Drivers/bmp388.o ./Core/Src/Drivers/bmp388.su ./Core/Src/Drivers/lis3mdl.cyclo ./Core/Src/Drivers/lis3mdl.d ./Core/Src/Drivers/lis3mdl.o ./Core/Src/Drivers/lis3mdl.su ./Core/Src/Drivers/usb.cyclo ./Core/Src/Drivers/usb.d ./Core/Src/Drivers/usb.o ./Core/Src/Drivers/usb.su

.PHONY: clean-Core-2f-Src-2f-Drivers

