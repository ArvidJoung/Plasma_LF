################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/App/ti/ccsv5/tools/compiler/c2000_6.1.3

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"C:/Work/Pfcws64/RnD/LF_Power_Project/LF_Power/F2801_RAM.cmd" \
"./PwmCtrl.obj" \
"./PwmAdc.obj" \
"./LF_Power.obj" \
"./Example_Flash280x_CsmKeys.obj" \
"./Example_Flash280x_API.obj" \
"./DSP280x_usDelay.obj" \
"./DSP280x_SysCtrl.obj" \
"./DSP280x_Sci.obj" \
"./DSP280x_PieVect.obj" \
"./DSP280x_PieCtrl.obj" \
"./DSP280x_MemCopy.obj" \
"./DSP280x_GlobalVariableDefs.obj" \
"./DSP280x_DefaultIsr.obj" \
"./DSP280x_CpuTimers.obj" \
"./DSP280x_CodeStartBranch.obj" \
"./DSP280x_Adc.obj" \
-l"C:\Work\Pfcws64\RnD\LF_Power_Project\Flash2808_API_V302\lib\Flash2808_API_V302.lib" \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_headers/cmd/DSP280x_Headers_nonBIOS.cmd" \
"C:/App/ti/ccsv5/tools/compiler/c2000_6.1.3/lib/rts2800_ml.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: C:\Work\Pfcws64\RnD\LF_Power_Project\LF_Power/Debug/LF_Power.out

# Tool invocations
C:\Work\Pfcws64\RnD\LF_Power_Project\LF_Power/Debug/LF_Power.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C2000 Linker'
	"C:/App/ti/ccsv5/tools/compiler/c2000_6.1.3/bin/cl2000" -v28 -ml --float_support=softlib -g --define="_DEBUG" --define="LARGE_MODEL" --diag_warning=225 --display_error_number --obj_directory="C:/Work/Pfcws64/RnD/LF_Power_Project/LF_Power/LF_Power/Debug" -z -m"C:/Work/Pfcws64/RnD/LF_Power_Project/LF_Power/Debug/LF_Power.map" --stack_size=0x3FE --warn_sections -i"C:/App/ti/ccsv5/tools/compiler/c2000_6.1.3/lib" -i"C:/Work/Pfcws64/RnD/LF_Power_Project/Flash2808_API_V302/lib" -i"C:/App/ti/ccsv5/tools/compiler/c2000_6.1.3/include" -i"C:/Work/Pfcws64/RnD/LF_Power_Project/LF_Power/LF_Power" -i"C:/Work/Pfcws64/RnD/LF_Power_Project/LF_Power" --reread_libs --rom_model -o "C:\Work\Pfcws64\RnD\LF_Power_Project\LF_Power/Debug/LF_Power.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C2000_EXECUTABLE_OUTPUTS__QUOTED) "C:\Work\Pfcws64\RnD\LF_Power_Project\LF_Power\Debug\LF_Power.out"
	-$(RM) "DSP280x_Adc.pp" "DSP280x_CpuTimers.pp" "DSP280x_DefaultIsr.pp" "DSP280x_GlobalVariableDefs.pp" "DSP280x_MemCopy.pp" "DSP280x_PieCtrl.pp" "DSP280x_PieVect.pp" "DSP280x_Sci.pp" "DSP280x_SysCtrl.pp" "Example_Flash280x_API.pp" "LF_Power.pp" "PwmAdc.pp" "PwmCtrl.pp" 
	-$(RM) "DSP280x_Adc.obj" "DSP280x_CodeStartBranch.obj" "DSP280x_CpuTimers.obj" "DSP280x_DefaultIsr.obj" "DSP280x_GlobalVariableDefs.obj" "DSP280x_MemCopy.obj" "DSP280x_PieCtrl.obj" "DSP280x_PieVect.obj" "DSP280x_Sci.obj" "DSP280x_SysCtrl.obj" "DSP280x_usDelay.obj" "Example_Flash280x_API.obj" "Example_Flash280x_CsmKeys.obj" "LF_Power.obj" "PwmAdc.obj" "PwmCtrl.obj" 
	-$(RM) "DSP280x_CodeStartBranch.pp" "DSP280x_usDelay.pp" "Example_Flash280x_CsmKeys.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
