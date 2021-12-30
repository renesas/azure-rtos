/* This is a small ping demo of the high-performance NetX Duo TCP/IP stack.  */

#include "tx_api.h"
#include "nx_api.h"
#include "nx_driver_rx671_rsk.h"

#include <demo_printf.h>

#include <r_wifi_sx_ulpgn_if.h>

/* Define the helper thread for running Azure SDK on ThreadX (THREADX IoT Platform).  */
#ifndef WIFI_SSID
#define WIFI_SSID                     ""
#endif /* WIFI_SSID  */

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD                 ""
#endif /* WIFI_SSID  */

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

/* Define the ThreadX and NetX object control blocks...  */

TX_THREAD  sample_helper_thread;
NX_PACKET_POOL    pool_0;
NX_IP             ip_0;  


/* Define the IP thread's stack area.  */

ULONG             ip_thread_stack[2 * 1024 / sizeof(ULONG)];


/* Define packet pool for the demonstration.  */
static ULONG sample_pool[SAMPLE_POOL_SIZE / sizeof(ULONG)];
static ULONG sample_pool_size = sizeof(sample_pool);
static ULONG sample_ip_stack[SAMPLE_IP_STACK_SIZE / sizeof(ULONG)];

/* Define the prototypes for sample thread.  */
static void sample_helper_thread_entry(ULONG parameter);

/* Define the stack for ThreadX.  */
static ULONG sample_helper_thread_stack[SAMPLE_HELPER_STACK_SIZE / sizeof(ULONG)];

/* Define the ARP cache area.  */

ULONG             arp_space_area[512 / sizeof(ULONG)];

                                                           
/* Define an error counter.  */

ULONG             error_counter;

VOID platform_setup(void);

int main(int argc, char ** argv)
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
     
    /* Initialize the NetX system.  */
    nx_system_initialize();
    
    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", SAMPLE_PACKET_SIZE,
                                   (UCHAR *)sample_pool , sample_pool_size);

    /* Check for pool creation error.  */
    if (status)
        error_counter++;


    /* Create sample helper thread. */
    status = tx_thread_create(&sample_helper_thread, "Demo Thread",
                              sample_helper_thread_entry, 0,
                              sample_helper_thread_stack, SAMPLE_HELPER_STACK_SIZE,
                              SAMPLE_HELPER_THREAD_PRIORITY, SAMPLE_HELPER_THREAD_PRIORITY,
                              TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check for helper thread creation error.  */
    if (status)
        error_counter++;

}

void sample_helper_thread_entry(ULONG parameter)
{
	UINT    status;
	wifi_ip_configuration_t ip_cfg =  {0};

	printf("Initializing Wi-Fi\r\n");

	status = R_WIFI_SX_ULPGN_Open();

	/* Check for wi-fi module open error. */
    if (status)
        error_counter++;

    status = R_WIFI_SX_ULPGN_Connect (WIFI_SSID, WIFI_PASSWORD, WIFI_SECURITY_WPA2, 1, &ip_cfg);

    /* Check for wi-fi connect error. */
    if (status)
        error_counter++;

    if(status) {
    	printf("Error connecting to Wi-Fi network.\r\n");
    } else {
    	printf("Wi-Fi connected.\r\n");
    }
    status = R_WIFI_SX_ULPGN_GetIpAddress(&ip_cfg);

    /* Check for API error. */
    if (status)
        error_counter++;

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

}
