################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/BSP/Motor/TB6612FNG/bsp_motor_driver.c 

OBJS += \
./Core/Src/BSP/Motor/TB6612FNG/bsp_motor_driver.o 

C_DEPS += \
./Core/Src/BSP/Motor/TB6612FNG/bsp_motor_driver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/BSP/Motor/TB6612FNG/%.o Core/Src/BSP/Motor/TB6612FNG/%.su Core/Src/BSP/Motor/TB6612FNG/%.cyclo: ../Core/Src/BSP/Motor/TB6612FNG/%.c Core/Src/BSP/Motor/TB6612FNG/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303xE -c -I../Core/Inc -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Inc/Myheader" -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/streamports/RingBuffer/config" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/streamports/RingBuffer/include" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/config" -I"C:/Users/ajy97/Desktop/Capstone/PP_Final_4_Real_Real2/Core/Src/TraceRecorder/include" -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-BSP-2f-Motor-2f-TB6612FNG

clean-Core-2f-Src-2f-BSP-2f-Motor-2f-TB6612FNG:
	-$(RM) ./Core/Src/BSP/Motor/TB6612FNG/bsp_motor_driver.cyclo ./Core/Src/BSP/Motor/TB6612FNG/bsp_motor_driver.d ./Core/Src/BSP/Motor/TB6612FNG/bsp_motor_driver.o ./Core/Src/BSP/Motor/TB6612FNG/bsp_motor_driver.su

.PHONY: clean-Core-2f-Src-2f-BSP-2f-Motor-2f-TB6612FNG

