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
please refer to section 2.7 of r01an6455ej0100-rx-azure-rtos.pdf


1.3. If you are using e2 studio 2022-04/Smart Configurator 2.13.0 or earlier, 
please do the following change on pin setting for SCI0 and SCI2.
In Smart Configurator editor (<projectname>.scfg)
- go to Components tab
- select r_sci_rx on the left panel
- on the right panel, 
  scroll down to SCI0 resource:
    check "SCI0"
    check "RXD0/SMISO0/SSCL0 Pin"
    check "TXD0/SMOSI0/SSDA0 Pin"
  scroll down to SCI2 resource:
    check "SCI2"
    check "RXD2/SMISO2/SSCL2 Pin"
    check "TXD2/SMOSI2/SSDA2 Pin"
    check "CTS2#/RTS2#/SS2# Pin"
And [Generate Code]