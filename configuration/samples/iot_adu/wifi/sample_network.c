#include "nx_api.h"
#include "demo_printf.h"
#include "nx_driver_rx_wifi.h"

wifi_ip_configuration_t ip_cfg = {0};

/* Define the Wi-Fi network parameters. Only WPA2 security is supported.  */
extern uint8_t df_wifi_ssid;
extern uint8_t df_wifi_password;

UINT sample_network_configure(NX_IP *ip_ptr, ULONG *dns_server_address)
{
UINT    status;
UCHAR *wifi_ssid = (UCHAR *)&df_wifi_ssid;
UCHAR *wifi_password = (UCHAR *)&df_wifi_password;

    LOG_TERMINAL("Initializing Wi-Fi\r\n");

    status = R_WIFI_SX_ULPGN_Open();

    /* Check for wi-fi module open error. */
    if (status)
    {
        LOG_TERMINAL("Error connecting to Wi-Fi network.\r\n");
        return (status);
    }

    status = R_WIFI_SX_ULPGN_Connect(wifi_ssid, wifi_password, WIFI_SECURITY_WPA2, 1, &ip_cfg);

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
    return 0;
}
