---------------
1. Usage Notes
---------------
1.1. For more information about how to use this sample project, 
please refer to section 2.7 of r01an6455ej0100-rx-azure-rtos.pdf

1.2. From software package v6.1.12_rel-rx.1.0.0, to keep 8 bytes size for double type
 "-dbl_size=8" compiler build option is set as default for CC-RX project
 Size of type 'double' is set to "64 bits" as default for IAR project
 Please set "-m64bit-doubles" manually for GCC project

1.3. Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behaviour by yourself.
For example, if you create project with C++ option and CC-RX compiler, you will need to add abort() function manually.
