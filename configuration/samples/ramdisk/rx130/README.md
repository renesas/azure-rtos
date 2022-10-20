------------------------
1. Caution / Known Issue
------------------------
If you are using e2 studio 2022-04 or earlier version with GCC RX compiler,
please take note on the following issue.

The "_end" section in src/linker_script.ld should be at the end.
However, the section order in linker_script.ld is changed when first build and after code generated.
If you are using RX130 Target Board, you can simply copy src/linker_script_sample.ld to current src/linker_script.ld, and build project again.
If you are using other RX130 devices, please open linker_script.ld and move the below section to the end, and build project again
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
2.1. For more information about how to use this sample project, 
please refer to section 2.2 of r01an6455ej0101-rx-azure-rtos.pdf

2.2. From software package v6.1.12_rel-rx.1.0.0, to keep 8 bytes size for double type
 "-dbl_size=8" compiler build option is set as default for CC-RX project
 Size of type 'double' is set to "64 bits" as default for IAR project
 Please set "-m64bit-doubles" manually for GCC project

2.3. Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behaviour by yourself.
For example, if you create project with C++ option and CC-RX compiler, you will need to add abort() function manually.
