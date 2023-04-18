
#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H


/* Ether is used to connect Azure cloud.*/
#define ENABLE_ETHER

#ifndef MAC_ADDRESS
#define MAC_ADDRESS  {0x0,0x04,0x00,0x00,0x00,0x00}
#endif

void platform_setup(void);

#endif // HARDWARE_SETUP_H
