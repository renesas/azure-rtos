
#include "r_smc_entry.h"

#include <platform.h>

#include <Config_SCI5.h>
#include <r_cmt_rx_if.h>

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
    
    /* Setup SCI5 for printf output. */
    R_Config_SCI5_Start();
    
    /* Create periodic timer for the system tick. */
    R_CMT_CreatePeriodic(100u, timer_callback, &chan);
}

