
#include "r_smc_entry.h"

#include <platform.h>

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
 */
void usb0_setup(void)
{
    /* Enable writing to registers related to operating modes, LPC, CGC and software reset */
    SYSTEM.PRCR.WORD = 0xA50BU;

    SYSTEM.VOLSR.BIT.USBVON = 1;
    /* enable USB0 module */
    MSTP(USB0) = 0;

    /* Clear any previously pending interrupts */
    IR(USB0, USBR0)  = 0;

    /* Set USBR0 priority level */
    IPR(USB0, USBR0) = ICU_USB0_USBR0_PRIORITY;

    /* Enable USBR0 interrupt */
    R_BSP_InterruptRequestEnable(VECT(USB0, USBR0));

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

    /* Setup SCIx for printf output. */
    //R_Config_SCIx_Start();
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set USB0_VBUS pin */
    MPC.PD2PFS.BYTE = 0x11U;
    PORTD.PMR.BIT.B2 = 1U;

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);

    /* Create periodic timer for the system tick. */
    R_CMT_CreatePeriodic(100u, timer_callback, &chan);

    usb0_setup();
}
