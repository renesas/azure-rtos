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

static UINT wifi_connect();
VOID sample_network_configure(NX_IP *ip_ptr, ULONG *dns_server_address)
{
ULONG   ip_address = 0;
ULONG   network_mask = 0;
ULONG   gateway_address = 0;
UINT    status;
ULONG   dns_address_second;


/* Initialize the demo printf implementation. */
//demo_printf_init();


/* Connect to Wifi.  */
if (wifi_connect())
{
	return;
}

#if 0
    WIFI_GetIP_Address((UCHAR *)&ip_address);
    WIFI_GetIP_Mask((UCHAR *)&network_mask);
    WIFI_GetGateway_Address((UCHAR *)&gateway_address);

    NX_CHANGE_ULONG_ENDIAN(ip_address);
    NX_CHANGE_ULONG_ENDIAN(network_mask);
    NX_CHANGE_ULONG_ENDIAN(gateway_address);
#endif
    if (dns_server_address)
    {
    	//*dns_server_address = 0x08080808;
    	//*dns_server_address = 0xc0A8017F;
    	*dns_server_address = ip_cfg.gateway;
    //    WIFI_GetDNS_Address((UCHAR *)dns_address, (UCHAR *)&dns_address_second);
    //   NX_CHANGE_ULONG_ENDIAN(*dns_address);
    }

    ip_address = ip_cfg.ipaddress;
    network_mask = ip_cfg.subnetmask;
    gateway_address = ip_cfg.gateway;
    status = nx_ip_address_set(ip_ptr, ip_address, network_mask);

    /* Check for IP address set errors.  */
    if (status)
    {
        printf("IP ADDRESS SET FAIL.\r\n");
        return;
    }

    status = nx_ip_gateway_address_set(ip_ptr, gateway_address);

    /* Check for gateway address set errors.  */
    if (status)
    {
        printf("IP GATEWAY ADDRESS SET FAIL.\r\n");
        return;
    }
}



static UINT wifi_connect()
{
UINT    status;


	printf("Initializing Wi-Fi\r\n");

	status = R_WIFI_SX_ULPGN_Open();

	/* Check for wi-fi module open error. */
	if (status)
	{
		printf("Error connecting to Wi-Fi network.\r\n");
		return(status);
	}

	status = R_WIFI_SX_ULPGN_Connect(WIFI_SSID, WIFI_PASSWORD, WIFI_SECURITY_WPA2, 1, &ip_cfg);

	if(status)
	{
		printf("Error connecting to Wi-Fi network.\r\n");
		return(status);
	}
	else
	{
		printf("Wi-Fi connected.\r\n");
	}

	status = R_WIFI_SX_ULPGN_GetIpAddress(&ip_cfg);

	/* Check for API error. */
	if (status)
	{
		printf("No IP Address.\r\n");
		return(status);
	}

	/* Output IP address and gateway address. */
	printf("IP address: %lu.%lu.%lu.%lu\r\n",
		   (ip_cfg.ipaddress >> 24),
		   (ip_cfg.ipaddress >> 16 & 0xFF),
		   (ip_cfg.ipaddress >> 8 & 0xFF),
		   (ip_cfg.ipaddress & 0xFF));
	printf("Mask: %lu.%lu.%lu.%lu\r\n",
		   (ip_cfg.subnetmask >> 24),
		   (ip_cfg.subnetmask >> 16 & 0xFF),
		   (ip_cfg.subnetmask >> 8 & 0xFF),
		   (ip_cfg.subnetmask & 0xFF));
	printf("Gateway: %lu.%lu.%lu.%lu\r\n",
		   (ip_cfg.gateway >> 24),
		   (ip_cfg.gateway >> 16 & 0xFF),
		   (ip_cfg.gateway >> 8 & 0xFF),
		   (ip_cfg.gateway & 0xFF));
	return(status);
}
