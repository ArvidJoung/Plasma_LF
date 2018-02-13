################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_headers/cmd/DSP280x_Headers_nonBIOS.cmd \
C:/Work/Pfcws64/RnD/LF_Power_Project/LF_Power/F2801_RAM.cmd 

LIB_SRCS += \
C:/App/ti/ccsv5/tools/compiler/c2000_6.1.3/lib/rts2800_ml.lib 

ASM_SRCS += \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_CodeStartBranch.asm \
../DSP280x_usDelay.asm \
../Example_Flash280x_CsmKeys.asm 

C_SRCS += \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_Adc.c \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_CpuTimers.c \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_DefaultIsr.c \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_headers/source/DSP280x_GlobalVariableDefs.c \
../DSP280x_MemCopy.c \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_PieCtrl.c \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_PieVect.c \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_Sci.c \
C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_SysCtrl.c \
../Example_Flash280x_API.c \
../LF_Power.c \
../PwmAdc.c \
../PwmCtrl.c 

OBJS += \
./DSP280x_Adc.obj \
./DSP280x_CodeStartBranch.obj \
./DSP280x_CpuTimers.obj \
./DSP280x_DefaultIsr.obj \
./DSP280x_GlobalVariableDefs.obj \
./DSP280x_MemCopy.obj \
./DSP280x_PieCtrl.obj \
./DSP280x_PieVect.obj \
./DSP280x_Sci.obj \
./DSP280x_SysCtrl.obj \
./DSP280x_usDelay.obj \
./Example_Flash280x_API.obj \
./Example_Flash280x_CsmKeys.obj \
./LF_Power.obj \
./PwmAdc.obj \
./PwmCtrl.obj 

ASM_DEPS += \
./DSP280x_CodeStartBranch.pp \
./DSP280x_usDelay.pp \
./Example_Flash280x_CsmKeys.pp 

C_DEPS += \
./DSP280x_Adc.pp \
./DSP280x_CpuTimers.pp \
./DSP280x_DefaultIsr.pp \
./DSP280x_GlobalVariableDefs.pp \
./DSP280x_MemCopy.pp \
./DSP280x_PieCtrl.pp \
./DSP280x_PieVect.pp \
./DSP280x_Sci.pp \
./DSP280x_SysCtrl.pp \
./Example_Flash280x_API.pp \
./LF_Power.pp \
./PwmAdc.pp \
./PwmCtrl.pp 

C_DEPS__QUOTED += \
"DSP280x_Adc.pp" \
"DSP280x_CpuTimers.pp" \
"DSP280x_DefaultIsr.pp" \
"DSP280x_GlobalVariableDefs.pp" \
"DSP280x_MemCopy.pp" \
"DSP280x_PieCtrl.pp" \
"DSP280x_PieVect.pp" \
"DSP280x_Sci.pp" \
"DSP280x_SysCtrl.pp" \
"Example_Flash280x_API.pp" \
"LF_Power.pp" \
"PwmAdc.pp" \
"PwmCtrl.pp" 

OBJS__QUOTED += \
"DSP280x_Adc.obj" \
"DSP280x_CodeStartBranch.obj" \
"DSP280x_CpuTimers.obj" \
"DSP280x_DefaultIsr.obj" \
"DSP280x_GlobalVariableDefs.obj" \
"DSP280x_MemCopy.obj" \
"DSP280x_PieCtrl.obj" \
"DSP280x_PieVect.obj" \
"DSP280x_Sci.obj" \
"DSP280x_SysCtrl.obj" \
"DSP280x_usDelay.obj" \
"Example_Flash280x_API.obj" \
"Example_Flash280x_CsmKeys.obj" \
"LF_Power.obj" \
"PwmAdc.obj" \
"PwmCtrl.obj" 

ASM_DEPS__QUOTED += \
"DSP280x_CodeStartBranch.pp" \
"DSP280x_usDelay.pp" \
"Example_Flash280x_CsmKeys.pp" 

C_SRCS__QUOTED += \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_Adc.c" \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_CpuTimers.c" \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_DefaultIsr.c" \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_headers/source/DSP280x_GlobalVariableDefs.c" \
"../DSP280x_MemCopy.c" \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_PieCtrl.c" \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_PieVect.c" \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_Sci.c" \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_SysCtrl.c" \
"../Example_Flash280x_API.c" \
"../LF_Power.c" \
"../PwmAdc.c" \
"../PwmCtrl.c" 

ASM_SRCS__QUOTED += \
"C:/Work/Pfcws64/RnD/LF_Power_Project/TIDCS/DSP280x_common/source/DSP280x_CodeStartBranch.asm" \
"../DSP280x_usDelay.asm" \
"../Example_Flash280x_CsmKeys.asm" 


