---------------
1. Usage Notes
---------------
1.1. For more information about how to use this sample project, 
please refer to section 2.5 of r01an6455ej0102-rx-azure-rtos.pdf

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
2.1. After the project generation is completed, please do the pin assignment check for SCI.
In Smart Configurator editor (<projectname>.scfg)
- go to Pins tab
- at Pin Function, select SCI channel being used on the left panel (channel with opened blue box)
- on the right panel, check and change the pin function if it is incorrect: 
  RXDx and TXDx should be used but the default setting could be SMISOx and SMOSIx

This issue is fixed from e2 studio 2023-04 and Smart Configurator for RX 2.17.

2.2. If you use sample project with EWF library (Preliminary version) and disable the debug log,
the sample program can not communicate with RYZ014A module.
For the details of issue and workaround, please refer to this link
https://github.com/Azure/embedded-wireless-framework/issues/16

The issue will be fixed at next update of software package.
