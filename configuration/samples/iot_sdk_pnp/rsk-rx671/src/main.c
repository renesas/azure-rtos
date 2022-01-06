/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

#include "nx_api.h"
#include "nxd_dns.h"
#include "nx_secure_tls_api.h"
#include "nx_driver_rx671_rsk.h"

#include <demo_printf.h>

/* Include the sample.  */
extern VOID sample_entry(NX_IP* ip_ptr, NX_PACKET_POOL* pool_ptr, NX_DNS* dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time));

/* Define the Wi-Fi network parameters. Only WPA2 security is supported.  */
#ifndef WIFI_SSID
#define WIFI_SSID                     ""
#endif /* WIFI_SSID  */

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD                 ""
#endif /* WIFI_PASSWORD  */

/* Define the helper thread for running Azure SDK on ThreadX (THREADX IoT Platform).  */
#ifndef SAMPLE_HELPER_STACK_SIZE
#define SAMPLE_HELPER_STACK_SIZE      (4096)
#endif /* SAMPLE_HELPER_STACK_SIZE  */

#ifndef SAMPLE_HELPER_THREAD_PRIORITY
#define SAMPLE_HELPER_THREAD_PRIORITY (4)
#endif /* SAMPLE_HELPER_THREAD_PRIORITY  */

/* Define user configurable symbols. */
#ifndef SAMPLE_IP_STACK_SIZE
#define SAMPLE_IP_STACK_SIZE          (2048)
#endif /* SAMPLE_IP_STACK_SIZE  */

#ifndef SAMPLE_PACKET_COUNT
#define SAMPLE_PACKET_COUNT           (60)
#endif /* SAMPLE_PACKET_COUNT  */

#ifndef SAMPLE_PACKET_SIZE
#define SAMPLE_PACKET_SIZE            (1536)
#endif /* SAMPLE_PACKET_SIZE  */

#define SAMPLE_POOL_SIZE              ((SAMPLE_PACKET_SIZE + sizeof(NX_PACKET)) * SAMPLE_PACKET_COUNT)

#ifndef SAMPLE_ARP_CACHE_SIZE
#define SAMPLE_ARP_CACHE_SIZE         (512)
#endif /* SAMPLE_ARP_CACHE_SIZE  */

#ifndef SAMPLE_IP_THREAD_PRIORITY
#define SAMPLE_IP_THREAD_PRIORITY     (1)
#endif /* SAMPLE_IP_THREAD_PRIORITY */


/* Using the TIME protocol to get UNIX time.  */
/* Note that the time server must support the time protocol on port 37. */
/*
*/

#ifndef SAMPLE_TIME_SERVER_NAME
//#define SAMPLE_TIME_SERVER_NAME     "utcnist.colorado.edu"    /* Time Server.  */
#define SAMPLE_TIME_SERVER_NAME       "time4.nrc.ca"
#endif /* SAMPLE_TIME_SERVER_NAME */

/* Default time. GMT: Saturday, January 1, 2022 12:00:00 AM. Epoch timestamp: 1640995200.  */
#ifndef SAMPLE_SYSTEM_TIME 
#define SAMPLE_SYSTEM_TIME            1640995200
#endif /* SAMPLE_SYSTEM_TIME  */

static TX_THREAD        sample_helper_thread;
static NX_PACKET_POOL   pool_0;
static NX_IP            ip_0;
static NX_DNS           dns_0;
static NX_TCP_SOCKET 	time_socket;
wifi_ip_configuration_t ip_cfg = {0};

/* System clock time for UTC.  */
static ULONG            unix_time_base;

/* Define the stack/cache for ThreadX.  */
static ULONG sample_pool[SAMPLE_POOL_SIZE / sizeof(ULONG)];
static ULONG sample_ip_stack[SAMPLE_IP_STACK_SIZE / sizeof(ULONG)];

static ULONG sample_helper_thread_stack[SAMPLE_HELPER_STACK_SIZE / sizeof(ULONG)];

/* Define the prototypes for sample thread.  */
static void sample_helper_thread_entry(ULONG parameter);

static UINT wifi_connect();
static UINT time_sync(void);
static UINT unix_time_get(ULONG *unix_time);

VOID platform_setup(void);

/* Define main entry point.  */
int main(void)
{   
    /* Setup the hardware. */
    platform_setup();
    
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */
void    tx_application_define(void *first_unused_memory)
{

UINT  status;

    /* Initialize the demo printf implementation. */
    demo_printf_init();

    /* Create sample helper thread. */
    status = tx_thread_create(&sample_helper_thread, "Demo Thread",
                              sample_helper_thread_entry, 0,
                              sample_helper_thread_stack, SAMPLE_HELPER_STACK_SIZE,
                              SAMPLE_HELPER_THREAD_PRIORITY, SAMPLE_HELPER_THREAD_PRIORITY,
                              TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check status.  */
    if (status)
    {
        return;
    }
}

/* Define sample helper thread entry.  */
void sample_helper_thread_entry(ULONG parameter)
{
UINT    status;

	/* Connect to Wifi.  */
	if (wifi_connect())
	{
		return;
	}

	/* Initialize the NetX system.  */
	nx_system_initialize();

	/* Create a packet pool.  */
	status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", SAMPLE_PACKET_SIZE,
								   (UCHAR *)sample_pool , sizeof(sample_pool));

	/* Check for pool creation error.  */
	if (status)
	{
		printf("PACKET POOL CREATE FAIL.\r\n");
		return;
	}

	/* Create an IP instance.  */
	status = nx_ip_create(&ip_0, "NetX IP Instance 0",
						  0, 0xFFFFFFFF,
						  &pool_0, nx_driver_rx671_rsk,
						  (UCHAR*)sample_ip_stack, sizeof(sample_ip_stack),
						  SAMPLE_IP_THREAD_PRIORITY);

	/* Check for IP create errors.  */
	if (status)
	{
		printf("IP CREATE FAIL.\r\n");
		return;
	}

	/* Enable TCP.  */
	status = nx_tcp_enable(&ip_0);

	/* Check for TCP enable errors.  */
	if (status)
	{
		printf("TCP ENABLE FAIL.\r\n");
		return;
	}

	/* Enable UDP.  */
	status = nx_udp_enable(&ip_0);

	/* Check for UDP enable errors.  */
	if (status)
	{
		printf("UDP ENABLE FAIL.\r\n");
		return;
	}

	/* Set IP address.  */
    status = nx_ip_address_set(&ip_0, ip_cfg.ipaddress, ip_cfg.subnetmask);

    /* Check for IP address set errors.  */
    if (status)
    {
	  	printf("IP ADDRESS SET FAIL.\r\n");
        return;
    }

	/* Set gateway address.  */
    status = nx_ip_gateway_address_set(&ip_0, ip_cfg.gateway);

    /* Check for gateway address set errors.  */
    if (status)
    {
	  	printf("IP GATEWAY ADDRESS SET FAIL.\r\n");
        return;
    }

	/* Initialize TLS.  */
	nx_secure_tls_initialize();

	/* Start to sync the local time.  */
	status = time_sync();

    /* Check status.  */
    if (status)
    {
        printf("Time Sync failed.\r\n");
        printf("Set Time to default value: SAMPLE_SYSTEM_TIME.\r\n");
        unix_time_base = SAMPLE_SYSTEM_TIME;
    }
    else
    {
        printf("Time Sync successful.\r\n");
    }

    /* Start sample.  */
    sample_entry(&ip_0, &pool_0, &dns_0, unix_time_get);
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

static UINT time_sync(void)
{
UINT status;
NXD_ADDRESS host_address;
NX_PACKET *packet;
ULONG time;
ULONG len;
ULONG system_time_in_second;

    printf("Time Sync...\r\n");

    status = nxd_dns_host_by_name_get(NULL, SAMPLE_TIME_SERVER_NAME, &host_address, 5000, 0);

    if(status != NX_SUCCESS)
    {
        return(NX_NOT_SUCCESSFUL);
    }

    demo_printf("Time server IP address: %lu.%lu.%lu.%lu\r\n",
			    (host_address.nxd_ip_address.v4 >> 24),
			    (host_address.nxd_ip_address.v4 >> 16 & 0xFF),
			    (host_address.nxd_ip_address.v4 >> 8 & 0xFF),
			    (host_address.nxd_ip_address.v4 & 0xFF));

    status = nx_tcp_socket_create(&ip_0, &time_socket, "socket", NX_IP_NORMAL, NX_DONT_FRAGMENT, 127u, 2048u, NULL, NULL);


    if(status != NX_SUCCESS)
    {
        return(status);
    }

    status = nx_tcp_client_socket_bind(&time_socket, NX_ANY_PORT, TX_WAIT_FOREVER);
    if(status != NX_SUCCESS)
    {
        nx_tcp_socket_delete(&time_socket);
        return(status);
    }

    status = nx_tcp_client_socket_connect(&time_socket, host_address.nxd_ip_address.v4, 37u, TX_WAIT_FOREVER);
    if(status != NX_SUCCESS)
    {
        nx_tcp_client_socket_unbind(&time_socket);
        nx_tcp_socket_delete(&time_socket);
        return(status);
    }

    /* Wait for response.  */
    status = nx_tcp_socket_receive(&time_socket, &packet, TX_WAIT_FOREVER);
    if(status != NX_SUCCESS)
    {
        nx_tcp_socket_disconnect(&time_socket, NX_NO_WAIT);
        nx_tcp_client_socket_unbind(&time_socket);
        nx_tcp_socket_delete(&time_socket);
        return(status);
    }

    if(packet -> nx_packet_length != 4)
    {
    	nx_packet_release(packet);
        nx_tcp_socket_disconnect(&time_socket, NX_NO_WAIT);
        nx_tcp_client_socket_unbind(&time_socket);
        nx_tcp_socket_delete(&time_socket);
        return (NX_NOT_SUCCESSFUL);
    }

    status = nx_packet_data_extract_offset(packet, 0u, &time, sizeof(time), &len);
    if ((status != NX_SUCCESS) || (len != 4))
    {
    	nx_packet_release(packet);
        nx_tcp_socket_disconnect(&time_socket, NX_NO_WAIT);
        nx_tcp_client_socket_unbind(&time_socket);
        nx_tcp_socket_delete(&time_socket);
        return (NX_NOT_SUCCESSFUL);
    }

    NX_CHANGE_ULONG_ENDIAN(time);

    demo_printf("Time: %u\r\n", time - 2208988800ul);

    system_time_in_second = tx_time_get() / TX_TIMER_TICKS_PER_SECOND;
    unix_time_base = (time - (system_time_in_second + 2208988800ul));

	nx_packet_release(packet);
    nx_tcp_socket_disconnect(&time_socket, NX_NO_WAIT);
    nx_tcp_client_socket_unbind(&time_socket);
    nx_tcp_socket_delete(&time_socket);

	return(NX_SUCCESS);
}

static UINT unix_time_get(ULONG *unix_time)
{

    /* Return number of seconds since Unix Epoch (1/1/1970 00:00:00).  */
    *unix_time =  unix_time_base + (tx_time_get() / TX_TIMER_TICKS_PER_SECOND);

    return(NX_SUCCESS);
}
