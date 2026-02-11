################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Inc/Myheader/bsp_nRF24.c 

OBJS += \
./Core/Inc/Myheader/bsp_nRF24.o 

C_DEPS += \
./Core/Inc/Myheader/bsp_nRF24.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/Myheader/%.o Core/Inc/Myheader/%.su Core/Inc/Myheader/%.cyclo: ../Core/Inc/Myheader/%.c Core/Inc/Myheader/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303xE -c -I../Core/Inc -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Inc/Myheader" -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/streamports/RingBuffer/config" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/streamports/RingBuffer/include" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/config" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/include" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-Myheader

clean-Core-2f-Inc-2f-Myheader:
	-$(RM) ./Core/Inc/Myheader/bsp_nRF24.cyclo ./Core/Inc/Myheader/bsp_nRF24.d ./Core/Inc/Myheader/bsp_nRF24.o ./Core/Inc/Myheader/bsp_nRF24.su

.PHONY: clean-Core-2f-Inc-2f-Myheader

