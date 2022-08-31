---------------
1. Usage Notes
---------------
1.1. This demonstration requires the network connection.
Before running the demo, please check to make sure the connections on board are set up correctly.
It is recommended to connect Wi-Fi Pmod Expansion to PMOD2.
To use PMOD2, following modifications are required
- P13: J18, J17 jumper header pin mounting (J18 Pin2-3, J17 Pin1-2 short), R114 removal
- P12: J21 jumper header pin mounting (J21 Pin2-3 short), R152 removal
- P33: J22 jumper header pin mounting (J22 Pin2-3 short), R189 removal
- P32: J23 jumper header pin mounting (J23 Pin2-3 short), R208 removal
- P52: J19 jumper header pin mounting (J19 Pin1-2 short), R225 removal

1.2. For more information about how to use this sample project, 
please refer to section 2.3 of r01an6455ej0100-rx-azure-rtos.pdf

1.3. From software package v6.1.12_rel-rx.1.0.0, "-dbl_size=8" compiler build option is set as default for CC-RX project 
to keep 8 bytes size for double type.

1.4. Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behaviour by yourself.
For example, if you create project with C++ option and CCRX compiler, you will need to add abort() function manually.
