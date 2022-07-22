------------------------
1. Caution / Known Issue
------------------------
If you are using e2 studio 2022-04 or earlier version with GCC RX compiler,
please take note on the following issue.

The "_end" section in src/linker_script.ld should be at the end.
However, the section order in linker_script.ld is changed when first build and after code generated.
If you are using RSK RX140, you can simply copy src/linker_script_sample.ld to current src/linker_script.ld, and build project again.
If you are using other RX140 devices, please open linker_script.ld and move the below section to the end, and build project again
.bss :
{
	_bss = .;
	*(.bss)
	*(.bss.**)
	*(COMMON)
	*(B)
	*(B_1)
	*(B_2)
	_ebss = .;
	_end = .;
} > RAM

---------------
2. Usage Notes
---------------
For more information about how to use this sample project, 
please refer to section 2.2 of r01an6455ej0100-rx-azure-rtos.pdf