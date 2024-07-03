---------------
1. Usage Notes
---------------
1.1. This demonstration requires the network connection.
Before running the demo, please check to make sure the connections on board are set up correctly.
It is recommended to connect Wi-Fi Pmod Expansion to PMOD2.
To use PMOD2, following modifications are required
- P13: J18, J17 jumper header pin mounting (J18 Pin2-3, J17 Pin1-2 short), R114 removal
- P12: J21 jumper header pin mounting (J21 Pin2-3 short), R152 removal
- P33: J22 jumper header pin mounting (J22 Pin2-3 short), R189 removal
- P32: J23 jumper header pin mounting (J23 Pin2-3 short), R208 removal
- P52: J19 jumper header pin mounting (J19 Pin1-2 short), R225 removal

1.2. For more information about how to use this sample project, 
please refer to section 2.6 of r01an6455ej0210-rx-azure-rtos.pdf

1.3. To keep 8 bytes size for double type
 "-dbl_size=8" compiler build option is set as default for CC-RX project
 Size of type 'double' is set to "64 bits" as default for IAR project
 "-m64bit-doubles" build option is set as default for GCC project

1.4. Please also take note that the sample project is verified with C project.
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

2.3. If you are using e2 studio 2022-04/Smart Configurator 2.13.0 or earlier, 
please do the following change on pin setting for SCI0 and SCI2.
In Smart Configurator editor (<projectname>.scfg)
- go to Components tab
- select r_sci_rx on the left panel
- on the right panel, 
  scroll down to SCI0 resource:
    check "SCI0"
    check "RXD0/SMISO0/SSCL0 Pin"
    check "TXD0/SMOSI0/SSDA0 Pin"
  scroll down to SCI2 resource:
    check "SCI2"
    check "RXD2/SMISO2/SSCL2 Pin"
    check "TXD2/SMOSI2/SSDA2 Pin"
    check "CTS2#/RTS2#/SS2# Pin"
And [Generate Code]

2.4. When using RX microcontrollers with DPFPU
Target projects (RX72N, RX671):
- Projects with the double precision floating point instruction generation option (CC-RX: -dpfpu, GCC: --mdfpu=64, IAR: --fpu=64) enabled on RXv3 core devices
Workaround:
- Call tx_thread_fpu_enable() at the beginning of every task.