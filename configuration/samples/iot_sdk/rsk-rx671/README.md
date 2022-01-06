---------------
1. Usage Notes
---------------
1.1. This demonstration requires the network connection.
Before running the demo, 
please check the following material inside the project to make sure all the connections on board are set up correctly

r12tu0157ej0100-rx671-rsk-preparation.pdf


1.2. This demonstration requires some information for the connection to IoTHub
Please define HOST_NAME, DEVICE_ID, DEVICE_SYMMETRIC_KEY in src\sample_config.h
And define WIFI_SSDI and WIFI_PASSWORD in src\main.c


1.3. Please do the following change on pin setting for SCI6
In Smart Configurator editor (<projectname>.scfg)
- go to Components tab
- select r_sci_rx on the left panel
- on the right panel, scroll down to SCI6 resource, uncheck "SCK6 pin" and check "CTS6#/RTS6#/SS6# pin"

1.4. Check clock setting
In Smart Configurator editor (<projectname>.scfg)
- go to Clocks tab
- make sure "HOCO clock" is selected and related clock setting are set correctly

1.5. Check debug setting
In Smart Configurator editor (<projectname>.scfg)
- go to System tab
- make sure "FINE" is selected