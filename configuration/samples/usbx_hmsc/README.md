---------------
1. Usage Notes
---------------
1.1. For more information about how to use this sample project, 
please refer to section 2.10 of r01an6455ej0102-rx-azure-rtos.pdf

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
If you are using RSK board (RSK RX65N, RSK RX671), please do the pin assignment check for USB0_VBUSEN.
In Smart Configurator editor (<projectname>.scfg)
- go to Pins tab
- at Pin Function, select USB0 on the left panel
- on the right panel, check and change USB0_VBUSEN pin number if it is incorrect (there is a warning icon)

This issue will be fixed at that next update of RSK board description file.
