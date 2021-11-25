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
#include "ux_system.h"
#include "ux_utility.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_dcd_rx_endpoint_nak_set                         PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function sets a NAK to an endpoint                             */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                              Pointer to rx controller        */ 
/*    ed                                    Register to the ed            */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    fifo_access_status                                                  */ 
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
VOID  _ux_dcd_rx_endpoint_nak_set(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed)
{
    
ULONG   busy_pipe;
    
    /* Take a pessimist view ! */
    busy_pipe = UX_RX_DCD_DCPCTR_PBUSY;

    /* Select the Pipe index.  */
    switch (ed -> ux_dcd_rx_ed_index)
    {
    
        case 0 :
            
            /* Set NAK in DCPCTR register.  */
            _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_DCPCTR_PID_MASK);

            /* Read the busy flag. */
            while (busy_pipe)
                busy_pipe = _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_DCPCTR) & UX_RX_DCD_DCPCTR_PBUSY;

            break;        
        
        default : 
            
            /* Set NAK in PIPExCTR register.  */
            _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index - 1) * 2), UX_RX_DCD_DCPCTR_PID_MASK);

            /* Read the busy flag. */
            while (busy_pipe)
                busy_pipe = _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index - 1) * 2)) & UX_RX_DCD_DCPCTR_PBUSY;

            break;        
        
    }
    return;
}

