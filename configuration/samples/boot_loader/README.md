---------------
1. Usage Notes
---------------
1.1. This sample project requires a "DUAL" device setting.
After project generation, please change to "DUAl" device following steps:
 [Project] > [Change Device]
 At "Target Device" text box, append "_DUAL" (e.g. R5F565NEHxFB_DUAL)
 Click Next, Next and Finish
After device is changed, compiler setting is reset, please
 [Project] > [C/C++ Project Settings]
 For CC-RX, 
  go to Linker > Section, click Import... and select "linker_section_sample.esi" under src folder of current project
  go to Linker > Section > Symbol file, add PFRAM2=RPFRAM2
 For GCC RX,
  Copy and replace conent of linker_script_sample.ld for current linker_script.ld under src folder of current project

1.2. For more information about how to use this sample project, 
please refer to section 2.x of r01an6455ej010x-rx-azure-rtos.pdf

1.3. To keep 8 bytes size for double type
 "-dbl_size=8" compiler build option is set as default for CC-RX project
 Size of type 'double' is set to "64 bits" as default for IAR project
 "-m64bit-doubles" build option is set as default for GCC project

1.4. Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behaviour by yourself.
For example, if you create project with C++ option and CC-RX compiler, you will need to add abort() function manually.
