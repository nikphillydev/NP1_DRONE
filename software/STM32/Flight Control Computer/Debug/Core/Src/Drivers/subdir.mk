################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Drivers/BMI088.cpp \
../Core/Src/Drivers/BMP388.cpp \
../Core/Src/Drivers/LIS3MDL.cpp \
../Core/Src/Drivers/PMW3901.cpp \
../Core/Src/Drivers/US100_Ultrasonic.cpp \
../Core/Src/Drivers/usb.cpp 

C_SRCS += \
../Core/Src/Drivers/bmi088.c \
../Core/Src/Drivers/bmp388.c \
../Core/Src/Drivers/lis3mdl.c 

C_DEPS += \
./Core/Src/Drivers/bmi088.d \
./Core/Src/Drivers/bmp388.d \
./Core/Src/Drivers/lis3mdl.d 

OBJS += \
./Core/Src/Drivers/BMI088.o \
./Core/Src/Drivers/BMP388.o \
./Core/Src/Drivers/LIS3MDL.o \
./Core/Src/Drivers/PMW3901.o \
./Core/Src/Drivers/US100_Ultrasonic.o \
./Core/Src/Drivers/bmi088.o \
./Core/Src/Drivers/bmp388.o \
./Core/Src/Drivers/lis3mdl.o \
./Core/Src/Drivers/usb.o 

CPP_DEPS += \
./Core/Src/Drivers/BMI088.d \
./Core/Src/Drivers/BMP388.d \
./Core/Src/Drivers/LIS3MDL.d \
./Core/Src/Drivers/PMW3901.d \
./Core/Src/Drivers/US100_Ultrasonic.d \
./Core/Src/Drivers/usb.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Drivers/%.o Core/Src/Drivers/%.su Core/Src/Drivers/%.cyclo: ../Core/Src/Drivers/%.cpp Core/Src/Drivers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/Inc/Threads -I../Core/Inc/Utility -I../Core/Inc/Drivers -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_MotionFX_Library/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/Drivers/%.o Core/Src/Drivers/%.su Core/Src/Drivers/%.cyclo: ../Core/Src/Drivers/%.c Core/Src/Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu18 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/Inc/Threads -I../Core/Inc/Utility -I../Core/Inc/Drivers -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_MotionFX_Library/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Drivers

clean-Core-2f-Src-2f-Drivers:
	-$(RM) ./Core/Src/Drivers/BMI088.cyclo ./Core/Src/Drivers/BMI088.d ./Core/Src/Drivers/BMI088.o ./Core/Src/Drivers/BMI088.su ./Core/Src/Drivers/BMP388.cyclo ./Core/Src/Drivers/BMP388.d ./Core/Src/Drivers/BMP388.o ./Core/Src/Drivers/BMP388.su ./Core/Src/Drivers/LIS3MDL.cyclo ./Core/Src/Drivers/LIS3MDL.d ./Core/Src/Drivers/LIS3MDL.o ./Core/Src/Drivers/LIS3MDL.su ./Core/Src/Drivers/PMW3901.cyclo ./Core/Src/Drivers/PMW3901.d ./Core/Src/Drivers/PMW3901.o ./Core/Src/Drivers/PMW3901.su ./Core/Src/Drivers/US100_Ultrasonic.cyclo ./Core/Src/Drivers/US100_Ultrasonic.d ./Core/Src/Drivers/US100_Ultrasonic.o ./Core/Src/Drivers/US100_Ultrasonic.su ./Core/Src/Drivers/bmi088.cyclo ./Core/Src/Drivers/bmi088.d ./Core/Src/Drivers/bmi088.o ./Core/Src/Drivers/bmi088.su ./Core/Src/Drivers/bmp388.cyclo ./Core/Src/Drivers/bmp388.d ./Core/Src/Drivers/bmp388.o ./Core/Src/Drivers/bmp388.su ./Core/Src/Drivers/lis3mdl.cyclo ./Core/Src/Drivers/lis3mdl.d ./Core/Src/Drivers/lis3mdl.o ./Core/Src/Drivers/lis3mdl.su ./Core/Src/Drivers/usb.cyclo ./Core/Src/Drivers/usb.d ./Core/Src/Drivers/usb.o ./Core/Src/Drivers/usb.su

.PHONY: clean-Core-2f-Src-2f-Drivers

