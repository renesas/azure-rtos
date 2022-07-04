------------------------
1. Caution / Known Issue
------------------------
The "_end" section in src/linker_script.ld should be at the end.

However, the section order in linker_script.ld is changed when first build and after code generated.

If you are using RSK RX140, you can simply copy src/linker_script_sample.ld to current src/linker_script.ld, and build project again

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

Note: this issue is fixed from e2 studio 2022-07.