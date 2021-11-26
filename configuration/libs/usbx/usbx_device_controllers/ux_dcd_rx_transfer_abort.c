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
/*    _ux_dcd_rx_transfer_abort                           PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will terminate a transfer.                            */
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
/*    _ux_dcd_rx_endpoint_nak_set         Set NAK status                  */ 
/*    _ux_dcd_rx_register_clear           Register clear                  */ 
/*    _ux_dcd_rx_current_endpoint_change  Change current endpoint         */ 
/*    _ux_dcd_rx_buffer_ready_interrupt   Buffer ready interrupt          */ 
/*    _ux_dcd_rx_buffer_empty_interrupt   Buffer empty interrupt          */ 
/*    _ux_dcd_rx_buffer_notready_interrupt Buffer not ready interrupt     */ 
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
/*                                            Used UX_ things instead of  */
/*                                            TX_ things directly,        */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _ux_dcd_rx_transfer_abort(UX_DCD_RX *dcd_rx, UX_SLAVE_TRANSFER *transfer_request)
{

UX_DCD_RX_ED            *ed;
UX_SLAVE_ENDPOINT       *endpoint;

UX_INTERRUPT_SAVE_AREA

    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint =  transfer_request -> ux_slave_transfer_request_endpoint;

    /* Keep the physical endpoint address in the endpoint container.  */
    ed =  (UX_DCD_RX_ED *) endpoint -> ux_slave_endpoint_ed;
    
    /* Entering critical area. Disable interrupts.  */
    UX_DISABLE
    
    /* Set PIPEx FIFO in in status. */
    _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_PIPESEL, ed -> ux_dcd_rx_ed_index);

    /* Check the type of endpoint.  */
    if (ed -> ux_dcd_rx_ed_index != 0)
    {

        /* Set NAK.  */
        _ux_dcd_rx_endpoint_nak_set(dcd_rx, ed);
        
        /* Clear the FIFO buffer memory. */
        _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_CFIFOCTR, UX_RX_DCD_FIFOCTR_BCLR);
        
        /* Set the current fifo port.  */
        _ux_dcd_rx_current_endpoint_change(dcd_rx, ed, 0);
            
        /* Clear the enabled interrupts.  */
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_BEMPSTS, (1ul << ed -> ux_dcd_rx_ed_index));
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_BRDYSTS, (1ul << ed -> ux_dcd_rx_ed_index));
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_NRDYSTS, (1ul << ed -> ux_dcd_rx_ed_index));

        /* Disable the Ready interrupt.  */
        _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
        
        /* Disable the Buffer empty interrupt.  */
        _ux_dcd_rx_buffer_empty_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
            
        /* Disable the Buffer Not Ready interrupt.  */
        _ux_dcd_rx_buffer_notready_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
        
        
    }

    /* Restore interrupts.  */
    UX_RESTORE

    /* This function never fails.  */
    return(UX_SUCCESS);         
}

