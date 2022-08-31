---------------
1. Usage Notes
---------------
1.1. For more information about how to use this sample project, 
please refer to section 2.5 of r01an6455ej0100-rx-azure-rtos.pdf

1.2. From software package v6.1.12_rel-rx.1.0.0, "-dbl_size=8" compiler build option is set as default for CC-RX project 
to keep 8 bytes size for double type.

1.3. Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behaviour by yourself.
For example, if you create project with C++ option and CCRX compiler, you will need to add abort() function manually.
