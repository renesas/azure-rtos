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


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   RX Controller Driver                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_dcd_rx.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_dcd_rx_buffer_empty_interrupt                   PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function enable or disable the BEMP interrupt for the pipe     */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                           Pointer to rx controller           */ 
/*    ed                                 Endpoint                         */ 
/*    flag                               Enable or Disable                */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    none                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    RX Controller Driver                                                */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
VOID  _ux_dcd_rx_buffer_empty_interrupt(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed, ULONG flag)
{

ULONG       rx_register;
ULONG       rx_bemp_register;

    /* Read the Interrupt Enable register 0.  */
    rx_register = _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_INTENB0);

    /* Reset the BEMPE, NRDYE, BRDYE bits.  */
    _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_INTENB0, UX_RX_DCD_INTENB0_BEMPE | UX_RX_DCD_INTENB0_NRDYE | UX_RX_DCD_INTENB0_BRDYE);

    /* Read the BEMP register.  */
    rx_bemp_register = _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_BEMPENB);

    /* Enable or Disable ?  */
    if (flag == UX_DCD_RX_ENABLE)

        /* Set the appropriate endpoint number.  */
        rx_bemp_register |= (1ul << ed -> ux_dcd_rx_ed_index);

    else
    
        /* Reset the appropriate endpoint number.  */
        rx_bemp_register &= ~(1ul << ed -> ux_dcd_rx_ed_index);

    /* Write the BEMP register.  */
    _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_BEMPENB, rx_bemp_register);

    /* Restore the INTENB0 register.  */
    _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_INTENB0, rx_register);
    
    return;
}

