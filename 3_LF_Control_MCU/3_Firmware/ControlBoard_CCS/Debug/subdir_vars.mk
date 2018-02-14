################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../2808_RAM_lnk.cmd \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_headers/cmd/DSP280x_Headers_nonBIOS.cmd 

ASM_SRCS += \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_CodeStartBranch.asm \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_usDelay.asm 

C_SRCS += \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_Adc.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_CpuTimers.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_DefaultIsr.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_EPwm.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_headers/source/DSP280x_GlobalVariableDefs.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_MemCopy.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_PieCtrl.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_PieVect.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_Sci.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_Spi.c \
C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_SysCtrl.c \
../adc.c \
../main.c \
../pwm.c \
../spi.c 

OBJS += \
./DSP280x_Adc.obj \
./DSP280x_CodeStartBranch.obj \
./DSP280x_CpuTimers.obj \
./DSP280x_DefaultIsr.obj \
./DSP280x_EPwm.obj \
./DSP280x_GlobalVariableDefs.obj \
./DSP280x_MemCopy.obj \
./DSP280x_PieCtrl.obj \
./DSP280x_PieVect.obj \
./DSP280x_Sci.obj \
./DSP280x_Spi.obj \
./DSP280x_SysCtrl.obj \
./DSP280x_usDelay.obj \
./adc.obj \
./main.obj \
./pwm.obj \
./spi.obj 

ASM_DEPS += \
./DSP280x_CodeStartBranch.pp \
./DSP280x_usDelay.pp 

C_DEPS += \
./DSP280x_Adc.pp \
./DSP280x_CpuTimers.pp \
./DSP280x_DefaultIsr.pp \
./DSP280x_EPwm.pp \
./DSP280x_GlobalVariableDefs.pp \
./DSP280x_MemCopy.pp \
./DSP280x_PieCtrl.pp \
./DSP280x_PieVect.pp \
./DSP280x_Sci.pp \
./DSP280x_Spi.pp \
./DSP280x_SysCtrl.pp \
./adc.pp \
./main.pp \
./pwm.pp \
./spi.pp 

C_DEPS__QUOTED += \
"DSP280x_Adc.pp" \
"DSP280x_CpuTimers.pp" \
"DSP280x_DefaultIsr.pp" \
"DSP280x_EPwm.pp" \
"DSP280x_GlobalVariableDefs.pp" \
"DSP280x_MemCopy.pp" \
"DSP280x_PieCtrl.pp" \
"DSP280x_PieVect.pp" \
"DSP280x_Sci.pp" \
"DSP280x_Spi.pp" \
"DSP280x_SysCtrl.pp" \
"adc.pp" \
"main.pp" \
"pwm.pp" \
"spi.pp" 

OBJS__QUOTED += \
"DSP280x_Adc.obj" \
"DSP280x_CodeStartBranch.obj" \
"DSP280x_CpuTimers.obj" \
"DSP280x_DefaultIsr.obj" \
"DSP280x_EPwm.obj" \
"DSP280x_GlobalVariableDefs.obj" \
"DSP280x_MemCopy.obj" \
"DSP280x_PieCtrl.obj" \
"DSP280x_PieVect.obj" \
"DSP280x_Sci.obj" \
"DSP280x_Spi.obj" \
"DSP280x_SysCtrl.obj" \
"DSP280x_usDelay.obj" \
"adc.obj" \
"main.obj" \
"pwm.obj" \
"spi.obj" 

ASM_DEPS__QUOTED += \
"DSP280x_CodeStartBranch.pp" \
"DSP280x_usDelay.pp" 

C_SRCS__QUOTED += \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_Adc.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_CpuTimers.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_DefaultIsr.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_EPwm.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_headers/source/DSP280x_GlobalVariableDefs.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_MemCopy.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_PieCtrl.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_PieVect.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_Sci.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_Spi.c" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_SysCtrl.c" \
"../adc.c" \
"../main.c" \
"../pwm.c" \
"../spi.c" 

ASM_SRCS__QUOTED += \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_CodeStartBranch.asm" \
"C:/Work/Pfcws64/RnD/ControlBoard_Project/TIDCS/DSP280x_common/source/DSP280x_usDelay.asm" 


