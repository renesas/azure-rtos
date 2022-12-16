---------------
1. Usage Notes
---------------
1.1. For more information about how to use this sample project, 
please refer to section 2.2 of r01an6455ej0101-rx-azure-rtos.pdf

1.2. To keep 8 bytes size for double type
 "-dbl_size=8" compiler build option is set as default for CC-RX project
 Size of type 'double' is set to "64 bits" as default for IAR project
 "-m64bit-doubles" build option is set as default for GCC project

1.3. Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behaviour by yourself.
For example, if you create project with C++ option and CC-RX compiler, you will need to add abort() function manually.


------------------------
2. Caution / Known Issue
------------------------
For RX130 Target Board, RSK RX140, and RX72N Envision Kit

2.1. If you are using e2 studio 2022-04 or earlier version with GCC RX compiler,
please take note on the following issue.

The "_end" section in src/linker_script.ld should be at the end.
However, the section order in linker_script.ld is changed when first build and after code generated.
You can simply copy src/linker_script_sample.ld to current src/linker_script.ld, and build project again.
Or you can open linker_script.ld and move the below section to the end, and build project again
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

2.2. If you are using RX72N Envision Kit, the device on RX72N Envision Kit is R5F572NN (Flash memory 4MB).
However, when creating new project with old board version (v1.11 and older),
the project is created with R5F572ND device (Flash memory 2MB).
You can go to [Board] tab of Smart Configurator editor to check the correctness of the device.
In case the device is R5F572ND, at Board tab, you can click [...] button behind the Board combo-box
to quickly go to Change Device dialog and change target device to R5F572NN.

This issue is fixed from board version v1.12.