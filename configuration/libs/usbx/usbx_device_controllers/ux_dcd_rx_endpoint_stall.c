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

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_dcd_rx.h"
#include "ux_device_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_dcd_rx_endpoint_stall                           PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will stall a physical endpoint.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_rx                              Pointer to device controller    */
/*    endpoint                              Pointer to endpoint container */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_dcd_rx_register_read            Read register                   */ 
/*    _ux_dcd_rx_register_write           Write register                  */ 
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
UINT  _ux_dcd_rx_endpoint_stall(UX_DCD_RX *dcd_rx, UX_SLAVE_ENDPOINT *endpoint)
{

UX_DCD_RX_ED      *ed;


    /* Get the physical endpoint address in the endpoint container.  */
    ed =  (UX_DCD_RX_ED *) endpoint -> ux_slave_endpoint_ed;

    /* Force STALL on this endpoint.  */
    switch (ed -> ux_dcd_rx_ed_index)
    {
    
        case 0 :
            
            /* Set STALL in DCPCTR register.  */
            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_DCPCTR_PIDSTALL);
            break;        
        
        default : 
            
            /* Set STALL in PIPExCTR register.  */
            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index - 1) * 2), UX_RX_DCD_DCPCTR_PIDSTALL);
            break;        
        
    }

    /* Set the endpoint to stall.  */
    ed -> ux_dcd_rx_ed_status |=  UX_DCD_RX_ED_STATUS_STALLED;

    /* This function never fails.  */
    return(UX_SUCCESS);         
}

