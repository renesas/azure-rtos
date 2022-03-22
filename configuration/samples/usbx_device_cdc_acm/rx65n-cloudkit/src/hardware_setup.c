
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

/* USB0 setup
 *     The USB SCKCR2 is configured by Smart Configure's clock tab and initialized by r_bsp.
 *     The interrupt number for USB0 is pre-assinged as 185 in r_bsp_interrupt_config.h
 */
void usb0_setup(void)
{
    /* Enable writing to registers related to operating modes, LPC, CGC and software reset */
    SYSTEM.PRCR.WORD = 0xA50BU;

    /* enable USB0 module */
    MSTP(USB0) = 0;

    /* Clear any previously pending interrupts */
    IR(USB0, USBR0)  = 0;

    /* Set USBR0 priority level */
    IPR(USB0, USBR0) = ICU_USB0_USBR0_PRIORITY;

    /* Enable USBR0 interrupt */
    R_BSP_InterruptRequestEnable(VECT(USB0, USBR0));

    BSP_PRV_INT_B_SELECT_185 = BSP_PRV_INT_B_NUM_USB0_USBI0;

    /* Set USBI0 priority level */
    IPR(USB0, USBI0) = ICU_USB0_USBI0_PRIORITY;

    /* Enable USBI0 interrupt in ICU */
    R_BSP_InterruptRequestEnable(VECT(USB0, USBI0));

    /* Enable protection */
    SYSTEM.PRCR.WORD = 0xA500U;
}

void platform_setup(void)
{
    uint32_t chan;

    /* Setup SCI5 for printf output. */
    R_Config_SCI5_Start();

    /* Create periodic timer for the system tick. */
    R_CMT_CreatePeriodic(TX_TIMER_TICKS_PER_SECOND, timer_callback, &chan);

    usb0_setup();
}
