################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Threads/sensor.cpp 

OBJS += \
./Core/Src/Threads/sensor.o 

CPP_DEPS += \
./Core/Src/Threads/sensor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Threads/%.o Core/Src/Threads/%.su Core/Src/Threads/%.cyclo: ../Core/Src/Threads/%.cpp Core/Src/Threads/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/Inc/Threads -I../Core/Inc/Utility -I../Core/Inc/Drivers -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/ST/STM32_MotionFX_Library/Inc -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Threads

clean-Core-2f-Src-2f-Threads:
	-$(RM) ./Core/Src/Threads/sensor.cyclo ./Core/Src/Threads/sensor.d ./Core/Src/Threads/sensor.o ./Core/Src/Threads/sensor.su

.PHONY: clean-Core-2f-Src-2f-Threads

