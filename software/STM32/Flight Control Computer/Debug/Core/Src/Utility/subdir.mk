################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/Utility/IIRFilter.cpp 

OBJS += \
./Core/Src/Utility/IIRFilter.o 

CPP_DEPS += \
./Core/Src/Utility/IIRFilter.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Utility/%.o Core/Src/Utility/%.su Core/Src/Utility/%.cyclo: ../Core/Src/Utility/%.cpp Core/Src/Utility/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Core/Inc/Threads -I../Core/Inc/Utility -I../Core/Inc/Drivers -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Utility

clean-Core-2f-Src-2f-Utility:
	-$(RM) ./Core/Src/Utility/IIRFilter.cyclo ./Core/Src/Utility/IIRFilter.d ./Core/Src/Utility/IIRFilter.o ./Core/Src/Utility/IIRFilter.su

.PHONY: clean-Core-2f-Src-2f-Utility

