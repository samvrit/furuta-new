################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.cla $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-c2000_20.2.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --include_path="C:/Users/sasrinivas/Desktop/furuta_mbed/motor_controller_ti/cla_ex1_asin" --include_path="C:/Users/sasrinivas/Desktop/furuta_mbed/motor_controller_ti/cla_ex1_asin/device" --include_path="C:/ti/c2000/C2000Ware_3_01_00_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-c2000_20.2.0.LTS/include" --define=_LAUNCHXL_F28379D --define=DEBUG --define=_FLASH --define=CPU1 --diag_suppress=10063 --diag_suppress=10068 --diag_suppress=770 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-c2000_20.2.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --include_path="C:/Users/sasrinivas/Desktop/furuta_mbed/motor_controller_ti/cla_ex1_asin" --include_path="C:/Users/sasrinivas/Desktop/furuta_mbed/motor_controller_ti/cla_ex1_asin/device" --include_path="C:/ti/c2000/C2000Ware_3_01_00_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-c2000_20.2.0.LTS/include" --define=_LAUNCHXL_F28379D --define=DEBUG --define=_FLASH --define=CPU1 --diag_suppress=10063 --diag_suppress=10068 --diag_suppress=770 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


