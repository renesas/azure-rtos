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
please refer to section 2.8 of r01an6455ej0100-rx-azure-rtos.pdf


------------------------
2. Caution / Known Issue
------------------------
If you are using e2 studio 2022-04 or earlier verion with GCC RX compiler, please take note on following issue.

For this GUIX 16bpp sample project, RAM2 should be used for sections in src/linker_script.ld
However, the section setting is changed to RAM in linker_script.ld when first build and after code generated.
You can simply copy src/linker_script_sample.ld to src/linker_script.ld, and build project again
