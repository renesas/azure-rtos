
#include "r_smc_entry.h"

#include <platform.h>

#include <Config_SCI8.h>
#include <r_cmt_rx_if.h>
#include <r_ether_rx_if.h>
#include <r_ether_rx_pinset.h>

#include <tx_api.h>

#include "hardware_setup.h"
#include "demo_printf.h"

void _tx_timer_interrupt(void);

/* CMT Timer callback used as the system tick. */
void timer_callback(void * pdata)
{
    _tx_timer_interrupt();
}


void platform_setup(void)
{
    uint32_t chan;
    ether_param_t eth_param = {0};
    
    /* Setup SCI8 for printf output. */
    R_Config_SCI8_Start();
    
    /* Create periodic timer for the system tick. */
    R_CMT_CreatePeriodic(TX_TIMER_TICKS_PER_SECOND, timer_callback, &chan);
    
    /* Setup Ethernet hardware. */
    R_ETHER_Initial();

    R_ETHER_PinSet_ETHERC0_MII();

    eth_param.channel = 0u;
    R_ETHER_Control(CONTROL_POWER_ON, eth_param);
}

