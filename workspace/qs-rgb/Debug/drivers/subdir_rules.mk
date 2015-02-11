################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
drivers/buttons.obj: C:/StellarisWare/boards/ek-lm4f120xl/drivers/buttons.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.10/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.10/include" --include_path="C:/StellarisWare/boards/ek-lm4f120xl" --include_path="C:/StellarisWare" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA2 --define=UART_BUFFERED --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/buttons.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

drivers/rgb.obj: C:/StellarisWare/boards/ek-lm4f120xl/drivers/rgb.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.10/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -O2 -g --include_path="c:/ti/ccsv6/tools/compiler/arm_5.1.10/include" --include_path="C:/StellarisWare/boards/ek-lm4f120xl" --include_path="C:/StellarisWare" --gcc --define=ccs="ccs" --define=PART_LM4F120H5QR --define=TARGET_IS_BLIZZARD_RA2 --define=UART_BUFFERED --diag_warning=225 --display_error_number --gen_func_subsections=on --ual --preproc_with_compile --preproc_dependency="drivers/rgb.pp" --obj_directory="drivers" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


