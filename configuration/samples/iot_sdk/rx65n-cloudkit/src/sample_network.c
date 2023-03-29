#include "nx_api.h"
#include "demo_printf.h"
#include "nx_driver_rx_wifi.h"

wifi_ip_configuration_t ip_cfg = {0};

/* Define the Wi-Fi network parameters. Only WPA2 security is supported.  */
#ifndef WIFI_SSID
#define WIFI_SSID                     ""
#endif /* WIFI_SSID  */

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD                 ""
#endif /* WIFI_PASSWORD  */

UINT sample_network_configure(NX_IP *ip_ptr, ULONG *dns_server_address)
{
UINT    status;

    LOG_TERMINAL("Initializing Wi-Fi\r\n");

    status = R_WIFI_SX_ULPGN_Open();

    /* Check for wi-fi module open error. */
    if (status)
    {
        LOG_TERMINAL("Error connecting to Wi-Fi network.\r\n");
        return (status);
    }

    status = R_WIFI_SX_ULPGN_Connect(WIFI_SSID, WIFI_PASSWORD, WIFI_SECURITY_WPA2, 1, &ip_cfg);

    if (status)
    {
        LOG_TERMINAL("Error connecting to Wi-Fi network.\r\n");
        return (status);
    }
    else
    {
        LOG_TERMINAL("Wi-Fi connected.\r\n");
    }

    status = R_WIFI_SX_ULPGN_GetIpAddress( &ip_cfg);

    /* Check for API error. */
    if (status)
    {
        LOG_TERMINAL("No IP Address.\r\n");
        return (status);
    }

    /* Output IP address and gateway address. */
    LOG_TERMINAL("IP address: %lu.%lu.%lu.%lu\r\n",
            (ip_cfg.ipaddress >> 24),
            (ip_cfg.ipaddress >> 16 & 0xFF),
            (ip_cfg.ipaddress >> 8 & 0xFF),
            (ip_cfg.ipaddress & 0xFF));
    LOG_TERMINAL("Mask: %lu.%lu.%lu.%lu\r\n",
            (ip_cfg.subnetmask >> 24),
            (ip_cfg.subnetmask >> 16 & 0xFF),
            (ip_cfg.subnetmask >> 8 & 0xFF),
            (ip_cfg.subnetmask & 0xFF));
    LOG_TERMINAL("Gateway: %lu.%lu.%lu.%lu\r\n",
            (ip_cfg.gateway >> 24),
            (ip_cfg.gateway >> 16 & 0xFF),
            (ip_cfg.gateway >> 8 & 0xFF),
            (ip_cfg.gateway & 0xFF));

    if (dns_server_address)
    {
        *dns_server_address = ip_cfg.gateway;
    }

    status = nx_ip_address_set(ip_ptr, ip_cfg.ipaddress, ip_cfg.subnetmask);

    /* Check for IP address set errors.  */
    if (status)
    {
        LOG_TERMINAL("IP ADDRESS SET FAIL.\r\n");
        return 0;
    }

    status = nx_ip_gateway_address_set(ip_ptr, ip_cfg.gateway);

    /* Check for gateway address set errors.  */
    if (status)
    {
        LOG_TERMINAL("IP GATEWAY ADDRESS SET FAIL.\r\n");
        return 0;
    }
}
