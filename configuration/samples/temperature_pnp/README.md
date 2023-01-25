---------------
1. Usage Notes
---------------
1.1. For more information about how to use this sample project, 
please refer to section 2.7 of r01an6455ej0102-rx-azure-rtos.pdf

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
After the project generation is completed, please do the pin assignment check for SCI.
In Smart Configurator editor (<projectname>.scfg)
- go to Pins tab
- at Pin Function, select SCI channel being used on the left panel (channel with opened blue box)
- on the right panel, check and change the pin function if it is incorrect: 
  RXDx and TXDx should be used but the default setting could be SMISOx and SMOSIx

This issue will be fixed at that next update of e2 studio.
