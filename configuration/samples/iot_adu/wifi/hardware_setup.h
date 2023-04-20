
#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H

/* Wi-Fi is used to connect Azure cloud.*/
#define ENABLE_WIFI

#ifndef WIFI_SSID
#define WIFI_SSID                     ""
#endif /* WIFI_SSID  */

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD                 ""
#endif /* WIFI_SSID  */

void platform_setup(void);

#endif // HARDWARE_SETUP_H
