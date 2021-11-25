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
#include "ux_utility.h"
#include "ux_device_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_dcd_rx_transfer_request                         PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will initiate a transfer to a specific endpoint.      */
/*    If the endpoint is IN, the endpoint register will be set to accept  */
/*    the request.                                                        */
/*                                                                        */
/*    If the endpoint is IN, the endpoint FIFO will be filled with the    */
/*    buffer and the endpoint register set.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_rx                              Pointer to device controller    */
/*    transfer_request                      Pointer to transfer request   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_dcd_rx_register_read            Read register                   */ 
/*    _ux_dcd_rx_register_set             Set register                    */ 
/*    _ux_dcd_rx_register_write           Write register                  */ 
/*    _ux_utility_semaphore_get           Get semaphore                   */ 
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
UINT  _ux_dcd_rx_transfer_request(UX_DCD_RX *dcd_rx, UX_SLAVE_TRANSFER *transfer_request)
{

UX_DCD_RX_ED            *ed;
UCHAR                   *fifo_buffer;
ULONG                   fifo_length;
UX_SLAVE_ENDPOINT       *endpoint;
UINT                    status; 
ULONG                   pipectr_reg;

UX_INTERRUPT_SAVE_AREA

    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint =  transfer_request -> ux_slave_transfer_request_endpoint;

    /* Get the physical endpoint from the logical endpoint.  */
    ed =  (UX_DCD_RX_ED *) endpoint -> ux_slave_endpoint_ed;
    
    /* Check the endpoint status, if it is free, we have an illegal endpoint.  */
    if ((ed -> ux_dcd_rx_ed_status & UX_DCD_RX_ED_STATUS_USED) == 0)
        return(UX_ERROR);

    /* Entering critical area. Disable interrupts.  */
    UX_DISABLE
    
    /* Set PID to NAK. Isolate Control endpoint and others.  */
    if (ed -> ux_dcd_rx_ed_index == 0)
    
        /* Set PID to NAK for Control Endpoint.  */
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_PIPECTR_PID_MASK);

    else
    {
        /* We should not have to do that, but just in case, wait for data to be out of the FIFO first.  */
        pipectr_reg = _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index -1) * 2));
        while(pipectr_reg & UX_RX_DCD_DCPCTR_INBUFM )
                pipectr_reg = _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index -1) * 2));

        /* Set PID to NAK for non Control Endpoints.  */
        _ux_dcd_rx_endpoint_nak_set(dcd_rx, ed);
        
    }
    
    /* Clear the BRDY and BEMP status for this pipe.  */
    _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_BEMPSTS, (1ul << ed -> ux_dcd_rx_ed_index));
    _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_BRDYSTS, (1ul << ed -> ux_dcd_rx_ed_index));

    /* Check for transfer direction.  */
    if (transfer_request -> ux_slave_transfer_request_phase == UX_TRANSFER_PHASE_DATA_OUT)
    {

        /* Data Out. So we expect a IN transaction from the host.  */
        /* Get the size of the transfer, used for a IN transaction only.  */
        fifo_length =  transfer_request -> ux_slave_transfer_request_requested_length;

        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_rx_ed_payload_length =  fifo_length;

        /* Check if the endpoint size is bigger that data requested. */
        if(fifo_length > endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize)

            /* Adjust the transfer size.  */
            fifo_length =  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

        /* Point the FIFO buffer to the current transfer request buffer address.  */
        fifo_buffer =  transfer_request -> ux_slave_transfer_request_data_pointer;
        
        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_rx_ed_payload_address =  fifo_buffer;
        
        /* Is it control endpoint ?.  */
        if (ed -> ux_dcd_rx_ed_index == 0)

            /* Set the current fifo port.  */
            _ux_dcd_rx_current_endpoint_change(dcd_rx, ed, UX_RX_DCD_CFIFOSEL_ISEL);
        
        else
        
            /* Set the current fifo port.  */
            _ux_dcd_rx_current_endpoint_change(dcd_rx, ed, 0);
        
        
        /* Clear the FIFO buffer memory. */
        _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_CFIFOCTR, UX_RX_DCD_FIFOCTR_BCLR);

        /* Check if the endpoint size is bigger that data requested. */
        if(fifo_length > endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize)
    
            /* Adjust the transfer size.  */
            fifo_length =  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
    
        /* Point the FIFO buffer to the current transfer request buffer address.  */
        fifo_buffer =  transfer_request -> ux_slave_transfer_request_data_pointer;
        
        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_rx_ed_payload_address =  fifo_buffer;
        
        /* Write the buffer to the pipe.  */
        status =  _ux_dcd_rx_buffer_write(dcd_rx, ed); 
        
        /* Check status.  */
        if (status != UX_ERROR) 
        {        

            /* Adjust the FIFO length in the endpoint.  */
            ed -> ux_dcd_rx_ed_payload_length -=  fifo_length;

            /* Adjust the data pointer.  */
            transfer_request -> ux_slave_transfer_request_current_data_pointer += fifo_length;
            
            /* Update the length of the data transmitted.  */
            transfer_request -> ux_slave_transfer_request_actual_length += fifo_length;
              
            /* Adjust the transfer length remaining.  */
            transfer_request -> ux_slave_transfer_request_in_transfer_length -= fifo_length;
        
            /* Set PID to BUF. Isolate Control endpoint and others.  */
            if (ed -> ux_dcd_rx_ed_index == 0)
        
                /* Set PID to BUF for Control Endpoint.  */
                _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_PIPECTR_PIDBUF);
    
            else
        
                /* Set PID to BUF for non Control Endpoints.  */
                _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index -1) * 2), UX_RX_DCD_PIPECTR_PIDBUF);
        }
        else
        {        
         
            /* Restore interrupts.  */
            UX_RESTORE
         
            /* We have an error on the write to the FIFO.  */
            return(UX_ERROR);
        }        
                
        /* Restore interrupts.  */
        UX_RESTORE
        
        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if (ed -> ux_dcd_rx_ed_index != 0)
        {

            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_slave_transfer_request_semaphore, UX_WAIT_FOREVER);
    
            /* Check the completion code. */
            if (status != UX_SUCCESS)
                return(status);
            
            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
                
                /* Return completion code.  */
                return(transfer_request -> ux_slave_transfer_request_completion_code);
            
            /* Return to caller with success.  */
            return(UX_SUCCESS);

        }
    }
    else
    {

        /* We have a request for a OUT transaction from the host.  */
        /* Clear the FIFO buffer memory. */
        _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_CFIFOCTR, UX_RX_DCD_FIFOCTR_BCLR);

        /* Set the current endpoint fifo.  */
        _ux_dcd_rx_current_endpoint_change(dcd_rx, ed, 0);
    
        /* Enable the BRDY interrupt.  */
        _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_ENABLE);
           
        /* Set PID to BUF. Isolate Control endpoint and others.  */
        if (ed -> ux_dcd_rx_ed_index == 0)
    
            /* Set PID to BUF for Control Endpoint.  */
            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_PIPECTR_PIDBUF);

        else
    
            /* Set PID to BUF for non Control Endpoints.  */
            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index -1) * 2), UX_RX_DCD_PIPECTR_PIDBUF);
    
        /* Restore interrupts.  */
        UX_RESTORE

        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if (ed -> ux_dcd_rx_ed_index != 0)
        {

            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_slave_transfer_request_semaphore, UX_WAIT_FOREVER);
                
            /* Check the completion code. */
            if (status != UX_SUCCESS)
                return(status);
            
            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
                return(transfer_request -> ux_slave_transfer_request_completion_code);
        
            /* Return to caller with success.  */
            return(UX_SUCCESS);
        }
    }

    /* Return to caller with success.  */
    return(UX_SUCCESS);
}

