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
/*    _ux_dcd_rx_endpoint_status                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will retrieve the status of the endpoint.             */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_rx                              Pointer to device controller    */
/*    endpoint_index                        Endpoint index                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
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
UINT  _ux_dcd_rx_endpoint_status(UX_DCD_RX *dcd_rx, ULONG endpoint_index)
{

UX_DCD_RX_ED      *ed;


    /* Fetch the address of the physical endpoint.  */
    ed =  &dcd_rx -> ux_dcd_rx_ed[endpoint_index];

    /* Check the endpoint status, if it is free, we have a illegal endpoint.  */
    if ((ed -> ux_dcd_rx_ed_status & UX_DCD_RX_ED_STATUS_USED) == 0)
        return(UX_ERROR);
        
    /* Check if the endpoint is stalled.  */
    if ((ed -> ux_dcd_rx_ed_status & UX_DCD_RX_ED_STATUS_STALLED) == 0)
        return(UX_FALSE);
    else            
        return(UX_TRUE);
}

