################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/STM32F3xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/TraceRecorder/streamports/RingBuffer/subdir.mk
-include Core/Src/TraceRecorder/subdir.mk
-include Core/Src/BSP/Servo(MG996R)/subdir.mk
-include Core/Src/BSP/Motor/TB6612FNG/subdir.mk
-include Core/Src/BSP/Motor/Encoder/subdir.mk
-include Core/Src/BSP/GY_85/subdir.mk
-include Core/Src/BSP/ETC/subdir.mk
-include Core/Src/App/Estimation/IMU/subdir.mk
-include Core/Src/App/Estimation/Encoder/subdir.mk
-include Core/Src/App/ETC/subdir.mk
-include Core/Src/App/Control/Pure_Pursuit/subdir.mk
-include Core/Src/App/Control/PID/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := PP_Final_4_Real_Real2
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
PP_Final_4_Real_Real2.elf \

MAP_FILES += \
PP_Final_4_Real_Real2.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
PP_Final_4_Real_Real2.list \


# All Target
all: main-build

# Main-build Target
main-build: PP_Final_4_Real_Real2.elf secondary-outputs

# Tool invocations
PP_Final_4_Real_Real2.elf PP_Final_4_Real_Real2.map: $(OBJS) $(USER_OBJS) C:\Users\ajy97\Desktop\Capstone\PP_Final_4_Real_Real2\STM32F303RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "PP_Final_4_Real_Real2.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\ajy97\Desktop\Capstone\PP_Final_4_Real_Real2\STM32F303RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="PP_Final_4_Real_Real2.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

PP_Final_4_Real_Real2.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "PP_Final_4_Real_Real2.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) PP_Final_4_Real_Real2.elf PP_Final_4_Real_Real2.list PP_Final_4_Real_Real2.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
