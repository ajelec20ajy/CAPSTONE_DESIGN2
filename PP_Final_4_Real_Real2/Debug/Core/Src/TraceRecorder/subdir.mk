################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/TraceRecorder/trcAssert.c \
../Core/Src/TraceRecorder/trcCounter.c \
../Core/Src/TraceRecorder/trcDependency.c \
../Core/Src/TraceRecorder/trcDiagnostics.c \
../Core/Src/TraceRecorder/trcEntryTable.c \
../Core/Src/TraceRecorder/trcError.c \
../Core/Src/TraceRecorder/trcEvent.c \
../Core/Src/TraceRecorder/trcEventBuffer.c \
../Core/Src/TraceRecorder/trcExtension.c \
../Core/Src/TraceRecorder/trcHardwarePort.c \
../Core/Src/TraceRecorder/trcHeap.c \
../Core/Src/TraceRecorder/trcISR.c \
../Core/Src/TraceRecorder/trcInternalEventBuffer.c \
../Core/Src/TraceRecorder/trcInterval.c \
../Core/Src/TraceRecorder/trcKernelPort.c \
../Core/Src/TraceRecorder/trcMultiCoreEventBuffer.c \
../Core/Src/TraceRecorder/trcObject.c \
../Core/Src/TraceRecorder/trcPrint.c \
../Core/Src/TraceRecorder/trcRunnable.c \
../Core/Src/TraceRecorder/trcStackMonitor.c \
../Core/Src/TraceRecorder/trcStateMachine.c \
../Core/Src/TraceRecorder/trcStaticBuffer.c \
../Core/Src/TraceRecorder/trcStreamingRecorder.c \
../Core/Src/TraceRecorder/trcString.c \
../Core/Src/TraceRecorder/trcTask.c \
../Core/Src/TraceRecorder/trcTaskMonitor.c \
../Core/Src/TraceRecorder/trcTimestamp.c 

OBJS += \
./Core/Src/TraceRecorder/trcAssert.o \
./Core/Src/TraceRecorder/trcCounter.o \
./Core/Src/TraceRecorder/trcDependency.o \
./Core/Src/TraceRecorder/trcDiagnostics.o \
./Core/Src/TraceRecorder/trcEntryTable.o \
./Core/Src/TraceRecorder/trcError.o \
./Core/Src/TraceRecorder/trcEvent.o \
./Core/Src/TraceRecorder/trcEventBuffer.o \
./Core/Src/TraceRecorder/trcExtension.o \
./Core/Src/TraceRecorder/trcHardwarePort.o \
./Core/Src/TraceRecorder/trcHeap.o \
./Core/Src/TraceRecorder/trcISR.o \
./Core/Src/TraceRecorder/trcInternalEventBuffer.o \
./Core/Src/TraceRecorder/trcInterval.o \
./Core/Src/TraceRecorder/trcKernelPort.o \
./Core/Src/TraceRecorder/trcMultiCoreEventBuffer.o \
./Core/Src/TraceRecorder/trcObject.o \
./Core/Src/TraceRecorder/trcPrint.o \
./Core/Src/TraceRecorder/trcRunnable.o \
./Core/Src/TraceRecorder/trcStackMonitor.o \
./Core/Src/TraceRecorder/trcStateMachine.o \
./Core/Src/TraceRecorder/trcStaticBuffer.o \
./Core/Src/TraceRecorder/trcStreamingRecorder.o \
./Core/Src/TraceRecorder/trcString.o \
./Core/Src/TraceRecorder/trcTask.o \
./Core/Src/TraceRecorder/trcTaskMonitor.o \
./Core/Src/TraceRecorder/trcTimestamp.o 

C_DEPS += \
./Core/Src/TraceRecorder/trcAssert.d \
./Core/Src/TraceRecorder/trcCounter.d \
./Core/Src/TraceRecorder/trcDependency.d \
./Core/Src/TraceRecorder/trcDiagnostics.d \
./Core/Src/TraceRecorder/trcEntryTable.d \
./Core/Src/TraceRecorder/trcError.d \
./Core/Src/TraceRecorder/trcEvent.d \
./Core/Src/TraceRecorder/trcEventBuffer.d \
./Core/Src/TraceRecorder/trcExtension.d \
./Core/Src/TraceRecorder/trcHardwarePort.d \
./Core/Src/TraceRecorder/trcHeap.d \
./Core/Src/TraceRecorder/trcISR.d \
./Core/Src/TraceRecorder/trcInternalEventBuffer.d \
./Core/Src/TraceRecorder/trcInterval.d \
./Core/Src/TraceRecorder/trcKernelPort.d \
./Core/Src/TraceRecorder/trcMultiCoreEventBuffer.d \
./Core/Src/TraceRecorder/trcObject.d \
./Core/Src/TraceRecorder/trcPrint.d \
./Core/Src/TraceRecorder/trcRunnable.d \
./Core/Src/TraceRecorder/trcStackMonitor.d \
./Core/Src/TraceRecorder/trcStateMachine.d \
./Core/Src/TraceRecorder/trcStaticBuffer.d \
./Core/Src/TraceRecorder/trcStreamingRecorder.d \
./Core/Src/TraceRecorder/trcString.d \
./Core/Src/TraceRecorder/trcTask.d \
./Core/Src/TraceRecorder/trcTaskMonitor.d \
./Core/Src/TraceRecorder/trcTimestamp.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/TraceRecorder/%.o Core/Src/TraceRecorder/%.su Core/Src/TraceRecorder/%.cyclo: ../Core/Src/TraceRecorder/%.c Core/Src/TraceRecorder/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303xE -c -I../Core/Inc -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Inc/Myheader" -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/streamports/RingBuffer/config" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/streamports/RingBuffer/include" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/config" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/include" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-TraceRecorder

clean-Core-2f-Src-2f-TraceRecorder:
	-$(RM) ./Core/Src/TraceRecorder/trcAssert.cyclo ./Core/Src/TraceRecorder/trcAssert.d ./Core/Src/TraceRecorder/trcAssert.o ./Core/Src/TraceRecorder/trcAssert.su ./Core/Src/TraceRecorder/trcCounter.cyclo ./Core/Src/TraceRecorder/trcCounter.d ./Core/Src/TraceRecorder/trcCounter.o ./Core/Src/TraceRecorder/trcCounter.su ./Core/Src/TraceRecorder/trcDependency.cyclo ./Core/Src/TraceRecorder/trcDependency.d ./Core/Src/TraceRecorder/trcDependency.o ./Core/Src/TraceRecorder/trcDependency.su ./Core/Src/TraceRecorder/trcDiagnostics.cyclo ./Core/Src/TraceRecorder/trcDiagnostics.d ./Core/Src/TraceRecorder/trcDiagnostics.o ./Core/Src/TraceRecorder/trcDiagnostics.su ./Core/Src/TraceRecorder/trcEntryTable.cyclo ./Core/Src/TraceRecorder/trcEntryTable.d ./Core/Src/TraceRecorder/trcEntryTable.o ./Core/Src/TraceRecorder/trcEntryTable.su ./Core/Src/TraceRecorder/trcError.cyclo ./Core/Src/TraceRecorder/trcError.d ./Core/Src/TraceRecorder/trcError.o ./Core/Src/TraceRecorder/trcError.su ./Core/Src/TraceRecorder/trcEvent.cyclo ./Core/Src/TraceRecorder/trcEvent.d ./Core/Src/TraceRecorder/trcEvent.o ./Core/Src/TraceRecorder/trcEvent.su ./Core/Src/TraceRecorder/trcEventBuffer.cyclo ./Core/Src/TraceRecorder/trcEventBuffer.d ./Core/Src/TraceRecorder/trcEventBuffer.o ./Core/Src/TraceRecorder/trcEventBuffer.su ./Core/Src/TraceRecorder/trcExtension.cyclo ./Core/Src/TraceRecorder/trcExtension.d ./Core/Src/TraceRecorder/trcExtension.o ./Core/Src/TraceRecorder/trcExtension.su ./Core/Src/TraceRecorder/trcHardwarePort.cyclo ./Core/Src/TraceRecorder/trcHardwarePort.d ./Core/Src/TraceRecorder/trcHardwarePort.o ./Core/Src/TraceRecorder/trcHardwarePort.su ./Core/Src/TraceRecorder/trcHeap.cyclo ./Core/Src/TraceRecorder/trcHeap.d ./Core/Src/TraceRecorder/trcHeap.o ./Core/Src/TraceRecorder/trcHeap.su ./Core/Src/TraceRecorder/trcISR.cyclo ./Core/Src/TraceRecorder/trcISR.d ./Core/Src/TraceRecorder/trcISR.o ./Core/Src/TraceRecorder/trcISR.su ./Core/Src/TraceRecorder/trcInternalEventBuffer.cyclo ./Core/Src/TraceRecorder/trcInternalEventBuffer.d ./Core/Src/TraceRecorder/trcInternalEventBuffer.o ./Core/Src/TraceRecorder/trcInternalEventBuffer.su ./Core/Src/TraceRecorder/trcInterval.cyclo ./Core/Src/TraceRecorder/trcInterval.d ./Core/Src/TraceRecorder/trcInterval.o ./Core/Src/TraceRecorder/trcInterval.su ./Core/Src/TraceRecorder/trcKernelPort.cyclo ./Core/Src/TraceRecorder/trcKernelPort.d ./Core/Src/TraceRecorder/trcKernelPort.o ./Core/Src/TraceRecorder/trcKernelPort.su ./Core/Src/TraceRecorder/trcMultiCoreEventBuffer.cyclo ./Core/Src/TraceRecorder/trcMultiCoreEventBuffer.d ./Core/Src/TraceRecorder/trcMultiCoreEventBuffer.o ./Core/Src/TraceRecorder/trcMultiCoreEventBuffer.su ./Core/Src/TraceRecorder/trcObject.cyclo ./Core/Src/TraceRecorder/trcObject.d ./Core/Src/TraceRecorder/trcObject.o ./Core/Src/TraceRecorder/trcObject.su ./Core/Src/TraceRecorder/trcPrint.cyclo ./Core/Src/TraceRecorder/trcPrint.d ./Core/Src/TraceRecorder/trcPrint.o ./Core/Src/TraceRecorder/trcPrint.su ./Core/Src/TraceRecorder/trcRunnable.cyclo ./Core/Src/TraceRecorder/trcRunnable.d ./Core/Src/TraceRecorder/trcRunnable.o ./Core/Src/TraceRecorder/trcRunnable.su ./Core/Src/TraceRecorder/trcStackMonitor.cyclo ./Core/Src/TraceRecorder/trcStackMonitor.d ./Core/Src/TraceRecorder/trcStackMonitor.o ./Core/Src/TraceRecorder/trcStackMonitor.su ./Core/Src/TraceRecorder/trcStateMachine.cyclo ./Core/Src/TraceRecorder/trcStateMachine.d ./Core/Src/TraceRecorder/trcStateMachine.o ./Core/Src/TraceRecorder/trcStateMachine.su ./Core/Src/TraceRecorder/trcStaticBuffer.cyclo ./Core/Src/TraceRecorder/trcStaticBuffer.d ./Core/Src/TraceRecorder/trcStaticBuffer.o ./Core/Src/TraceRecorder/trcStaticBuffer.su ./Core/Src/TraceRecorder/trcStreamingRecorder.cyclo ./Core/Src/TraceRecorder/trcStreamingRecorder.d ./Core/Src/TraceRecorder/trcStreamingRecorder.o ./Core/Src/TraceRecorder/trcStreamingRecorder.su ./Core/Src/TraceRecorder/trcString.cyclo ./Core/Src/TraceRecorder/trcString.d ./Core/Src/TraceRecorder/trcString.o ./Core/Src/TraceRecorder/trcString.su ./Core/Src/TraceRecorder/trcTask.cyclo ./Core/Src/TraceRecorder/trcTask.d ./Core/Src/TraceRecorder/trcTask.o ./Core/Src/TraceRecorder/trcTask.su ./Core/Src/TraceRecorder/trcTaskMonitor.cyclo ./Core/Src/TraceRecorder/trcTaskMonitor.d ./Core/Src/TraceRecorder/trcTaskMonitor.o ./Core/Src/TraceRecorder/trcTaskMonitor.su ./Core/Src/TraceRecorder/trcTimestamp.cyclo ./Core/Src/TraceRecorder/trcTimestamp.d ./Core/Src/TraceRecorder/trcTimestamp.o ./Core/Src/TraceRecorder/trcTimestamp.su

.PHONY: clean-Core-2f-Src-2f-TraceRecorder

