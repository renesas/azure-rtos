
#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H

/* Wi-Fi is used to connect Azure cloud.*/
/*
#define ENABLE_WIFI
*/

/* Ether is used to connect Azure cloud.*/
#define ENABLE_ETHER

#if (BSP_CFG_RTOS_USED != 5)
#error "Error! Need to define Azure RTOS in BSP configuration"
#endif

#ifndef MAC_ADDRESS
#define MAC_ADDRESS  {0x0,0x04,0x00,0x00,0x00,0x00}
#endif

void platform_setup(void);

#endif // HARDWARE_SETUP_H
