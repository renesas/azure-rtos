---------------
1. Usage Notes
---------------
1.1. For more information about how to use this sample project, 
please refer to section 2.1 of r01an6455ej0210-rx-azure-rtos.pdf

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

2.3. When using RX microcontrollers with DPFPU
Target projects (RX72N, RX671):
- Projects with the double precision floating point instruction generation option (CC-RX: -dpfpu, GCC: --mdfpu=64, IAR: --fpu=64) enabled on RXv3 core devices
Workaround:
- Call tx_thread_fpu_enable() at the beginning of every task.

2.4. Critical Vulnerability in Microsoft Azure RTOS ThreadX
Azure RTOS ThreadX allows arbitrary read and write access and potential privilege escalation due to a vulnerability in its parameter checking mechanism, affecting versions 6.2.1 and below, with no known workarounds.
Please refer to the following two sources for details on vulnerabilities.
  * [Azure RTOS ThreadX Remote Code Execution Vulnerability](https://github.com/eclipse-threadx/threadx/security/advisories/GHSA-p7w6-62rq-vrf9)
  * [Renesas PSIRT](https://www.renesas.com/us/en/support/renesas-psirt)
