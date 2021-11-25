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
/*    _ux_dcd_rx_data_buffer_size                         PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function returns the size of the buffer data                   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                           Pointer to rx controller           */ 
/*    ed                                 Register to the ed               */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    buffer size                                                         */ 
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
ULONG  _ux_dcd_rx_data_buffer_size(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed)
{

ULONG       buffer_size;
ULONG       rx_register;

    /* First get the pipe umber. If this is the control endpoint, we
       treat it differently. */
    if (ed -> ux_dcd_rx_ed_index == 0)
    {

        /* Read the DCPCFG register.  */
        rx_register =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_DCPCFG);

        /* Check if we are in continuous mode.  */
        if (rx_register & UX_RX_DCD_DCPCFG_CNTMD)

            /* Set the buffer size to the maximum.  */
            buffer_size =  UX_RX_DCD_PIPE0_SIZE;

        else

            /* Use the endpoint max packet size.  */
            buffer_size = ed -> ux_dcd_rx_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

    }
    else
    {

        /* Select the pipe.  */
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_PIPESEL, UX_RX_DCD_PIPESEL_NO_PIPE); 
        _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_PIPESEL, ed -> ux_dcd_rx_ed_index); 

        /* Read the DCPCFG register.  */
        rx_register =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_DCPCFG);

        /* Use the endpoint max packet size.  */
        buffer_size = ed -> ux_dcd_rx_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
        
    }

    /* Return the buffer size.  */
    return(buffer_size);
}

