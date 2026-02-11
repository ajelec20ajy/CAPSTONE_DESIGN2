################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/TraceRecorder/streamports/RingBuffer/trcStreamPort.c 

OBJS += \
./Core/Src/TraceRecorder/streamports/RingBuffer/trcStreamPort.o 

C_DEPS += \
./Core/Src/TraceRecorder/streamports/RingBuffer/trcStreamPort.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/TraceRecorder/streamports/RingBuffer/%.o Core/Src/TraceRecorder/streamports/RingBuffer/%.su Core/Src/TraceRecorder/streamports/RingBuffer/%.cyclo: ../Core/Src/TraceRecorder/streamports/RingBuffer/%.c Core/Src/TraceRecorder/streamports/RingBuffer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303xE -c -I../Core/Inc -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Inc/Myheader" -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/streamports/RingBuffer/config" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/streamports/RingBuffer/include" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/config" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/include" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-TraceRecorder-2f-streamports-2f-RingBuffer

clean-Core-2f-Src-2f-TraceRecorder-2f-streamports-2f-RingBuffer:
	-$(RM) ./Core/Src/TraceRecorder/streamports/RingBuffer/trcStreamPort.cyclo ./Core/Src/TraceRecorder/streamports/RingBuffer/trcStreamPort.d ./Core/Src/TraceRecorder/streamports/RingBuffer/trcStreamPort.o ./Core/Src/TraceRecorder/streamports/RingBuffer/trcStreamPort.su

.PHONY: clean-Core-2f-Src-2f-TraceRecorder-2f-streamports-2f-RingBuffer

