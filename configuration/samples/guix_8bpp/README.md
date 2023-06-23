---------------
1. Usage Notes
---------------
1.1. To configure/change the current graphical display on LCD, please use Azure RTOS GUIX Studio
(The Azure RTOS GUIX Studio installer is available here: https://aka.ms/azrtos-guix-installer)
- Open Azure RTOS GUIX Studio
- Select "Open Project"
- Select .gxp file that is available under "src" folder of this project

For more information about how to use Azure RTOS GUIX Studio, please check this page
https://docs.microsoft.com/azure/rtos/guix/about-guix-studio

For more information about GUIX in general, please refer to the following page
https://docs.microsoft.com/azure/rtos/guix/about-guix


1.2. For more information about how to use this sample project, 
please refer to section 2.8 of r01an6455ej0102-rx-azure-rtos.pdf

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
For GUIX 16bpp sample project only

2.1. If you are using e2 studio 2022-04 or earlier verion with GCC RX compiler, please take note on following issue.

For this GUIX 16bpp sample project, RAM2 should be used for sections in src/linker_script.ld
However, the section setting is changed to RAM in linker_script.ld when first build and after code generated.
You can simply copy src/linker_script_sample.ld to src/linker_script.ld, and build project again

2.2. If you are using RX72N Envision Kit, the device on RX72N Envision Kit is R5F572NN (Flash memory 4MB).
However, when creating new project with old board version (v1.11 and older),
the project is created with R5F572ND device (Flash memory 2MB).
You can go to [Board] tab of Smart Configurator editor to check the correctness of the device.
In case the device is R5F572ND, at Board tab, you can click [...] button behind the Board combo-box
to quickly go to Change Device dialog and change target device to R5F572NN.

This issue is fixed from board version v1.12.
