------------------------
1. Caution / Known Issue
------------------------
The device on RX72N Envision Kit is R5F572NN (Flash memory 4MB).
However, when creating new project with old board version (v1.11 and older),
the project is created with R5F572ND device (Flash memory 2MB).
You can go to [Board] tab of Smart Configurator editor to check the correctness of the device.
In case the device is R5F572ND, at Board tab, you can click [...] button behind the Board combo-box
to quickly go to Change Device dialog and change target device to R5F572NN.

This issue is fixed from board version v1.12.

---------------
2. Usage Notes
---------------
For more information about how to use this sample project, 
please refer to section 2.7 of r01an6455ej0100-rx-azure-rtos.pdf

Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behaviour by yourself.
For example, if you create project with C++ option and CCRX compiler, you will need to add abort() function manually.
