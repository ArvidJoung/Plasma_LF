################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
LF_Power_cl1377_수정.obj: ../Backup/LF_Power_cl1377_수정.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"C:/App/ti/ccsv5/tools/compiler/c2000/bin/cl2000" -v28 -ml --float_support=softlib -g --include_path="C:/App/ti/ccsv5/tools/compiler/c2000/include" --include_path="C:/App/ti/xdais_7_21_01_07/packages/ti/xdais" --include_path="C:/Work/Perforce64/RnD/LF_Power_Project/TIDCS/DSP280x_headers/include" --include_path="C:/Work/Perforce64/RnD/LF_Power_Project/TIDCS/DSP280x_common/include" --define="_DEBUG" --define="LARGE_MODEL" --diag_warning=225 --display_error_number --obj_directory="C:/Work/Perforce64/RnD/LF_Power_Project/LF_Power/LF_Power/Debug" --preproc_with_compile --preproc_dependency="Backup/LF_Power_cl1377_수정.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


