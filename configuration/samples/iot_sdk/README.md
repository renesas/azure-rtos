---------------
1. Usage Notes
---------------
1.1. For more information about how to use this sample project, 
please refer to section 2.6 of r01an6455ej0200-rx-azure-rtos.pdf

1.2. To keep 8 bytes size for double type
 "-dbl_size=8" compiler build option is set as default for CC-RX project
 Size of type 'double' is set to "64 bits" as default for IAR project
 "-m64bit-doubles" build option is set as default for GCC project

1.3. Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behavior by yourself.
For example, if you create project with C++ option and CC-RX compiler, you will need to add abort() function manually.

------------------------
2. Caution / Known Issue
------------------------
2.1. When using GCC compiler, in case you set Optimization level as Optimize size(-Os), please set the linker option not to remove unused sections as following
- in Project Explorer view, right-click on the project and select Properties
- on Properties dialog select C/C++ Build -> Settings -> Tool Settings tab -> Linker -> Other
- add "-Wl,--no-gc-sections" on User defined options
- click Apply and Close" button

2.2. When using GCC compiler, the "_end" section in src/linker_script.ld should be at the end. However, the default linker script may not meet this order, so please check linker_script.ld and move the section below to the end if needed, and build project again
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

2.3. After the project generation is completed, please do the pin assignment check for SCI.
In Smart Configurator editor (<projectname>.scfg)
- go to Pins tab
- at Pin Function, select SCI channel being used on the left panel (channel with opened blue box)
- on the right panel, check and change the pin function if it is incorrect: 
  RXDx and TXDx should be used but the default setting could be SMISOx and SMOSIx

This issue is fixed from e2 studio 2023-04 and Smart Configurator for RX 2.17.

2.4. When using RX microcontrollers with DPFPU
Target projects (RX72N, RX671):
- Projects with the double precision floating point instruction generation option (CC-RX: -dpfpu, GCC: --mdfpu=64, IAR: --fpu=64) enabled on RXv3 core devices
Workaround:
- Call tx_thread_fpu_enable() at the beginning of every task.

2.5. Note : Renesas Discontinues Sequans-Sourced LTE Modules RYZ014A (CAT-M1)
Renesas announces to discontinue the existing Sequans-sourced LTE module known as the part number RYZ014A and will no longer be shipping this product. 
With the discontinuation of RYZ014A, the CK-RX65N v1 board will also be discontinued. If you are using RYZ014A in a current design or production, the Sequans part numbers, GM01Q is a pin and functionally compatible replacement for RYZ014A.
Below Cellular driver of RX family works the below alternate product combination.
- RYZ014A Cellular Module Control Module ： Sequans GM01Q is the compatible module.

Regarding EOL notice of the RYZ014A, please see :
[The link] https://www.renesas.com/us/en/document/eln/plc-240004-end-life-eol-process-select-part-numbers?r=1503996
[The product page] https://www.renesas.com/us/en/products/wireless-connectivity/cellular-iot-modules/ryz014a-lte-cat-m1-cellular-iot-module