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
#include "ux_utility.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_dcd_rx_transfer_callback                        PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is invoked under ISR when an event happens on a       */
/*    specific endpoint.                                                  */
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
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_dcd_rx_address_set              Set address                     */ 
/*    _ux_dcd_rx_register_clear           Clear register                  */ 
/*    _ux_dcd_rx_register_read            Read register                   */ 
/*    _ux_dcd_rx_register_write           Write register                  */ 
/*    _ux_dcd_rx_register_set             Set register                    */ 
/*    _ux_device_stack_control_request_process Process control request    */ 
/*    _ux_utility_semaphore_get             Get semaphore                 */ 
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
UINT  _ux_dcd_rx_transfer_callback(UX_DCD_RX *dcd_rx, UX_SLAVE_TRANSFER *transfer_request, 
                                        ULONG interrupt_status, ULONG ctsq_mask)
{

UX_SLAVE_ENDPOINT       *endpoint;
UX_DCD_RX_ED            *ed;
UCHAR                   *data_pointer;
ULONG                   fifo_length;
UINT                    status;

    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint =  transfer_request -> ux_slave_transfer_request_endpoint;

    /* Get the rx endpoint.  */
    ed =  (UX_DCD_RX_ED *) endpoint -> ux_slave_endpoint_ed;

    /* Get the pointer to the data buffer of the transfer request.  */
    data_pointer =  transfer_request -> ux_slave_transfer_request_data_pointer;

    /* Check if we have a SETUP condition.  */
    if (interrupt_status ==  UX_RX_DCD_INTSTS0_CTRT)
    {

        /* Isolate the CTSQ field.  Check error. */
        switch (ctsq_mask)
        {

            case UX_RX_DCD_INTSTS0_CTSQ_CTSE :   
            
                /* There is an error in the state machine for SETUP transaction, revert to Idle and
                   wait for next SETUP.  */
                ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_IDLE;
    
                /* Done here.  */
                return(UX_ERROR);   
        
            case UX_RX_DCD_INTSTS0_CTSQ_SETUP :
                return(UX_SUCCESS);
                
            case UX_RX_DCD_INTSTS0_CTSQ_CRDS :
            case UX_RX_DCD_INTSTS0_CTSQ_CWDS :  
            case UX_RX_DCD_INTSTS0_CTSQ_CWNDSS :
                        
                /* We must have a valid SETUP packet.  This happens in 3 state machines : CRDS, CWDS, CWNDSS. */
                /* Clear VALID field.  */
                _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_INTSTS0, ~UX_RX_DCD_INTSTS0_VALID);
                
                /* We are in the SETUP phase.  Read the USBREQ, USBVAL, USBINDX and USBLENG registers to build the SETUP packet.  */
                /* Set the data pointer to the Transfer request setup area.  */
                data_pointer = transfer_request -> ux_slave_transfer_request_setup;
    
                /* Store the request field.  */            
                _ux_utility_short_put(data_pointer, (USHORT) _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_USBREQ));
                
                /* Store the Value field.  */            
                _ux_utility_short_put(data_pointer + UX_SETUP_VALUE, (USHORT) _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_USBVAL));
            
                /* Store the Index field.  */            
                _ux_utility_short_put(data_pointer + UX_SETUP_INDEX, (USHORT) _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_USBINDX));
            
                /* Store the Length field.  */            
                _ux_utility_short_put(data_pointer + UX_SETUP_LENGTH, (USHORT) _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_USBLENG));
    
                /* Clear the length of the data received.  */
                transfer_request -> ux_slave_transfer_request_actual_length =  0;
                
                /* Mark the phase as SETUP.  */
                transfer_request -> ux_slave_transfer_request_type =  UX_TRANSFER_PHASE_SETUP;
            
                /* Mark the transfer as successful.  */
                transfer_request -> ux_slave_transfer_request_completion_code =  UX_SUCCESS;
                
                /* Check if the transaction is IN.  */
                if (*transfer_request -> ux_slave_transfer_request_setup & UX_REQUEST_IN)
                
                    /* Call the Control Transfer dispatcher.  */
                    _ux_device_stack_control_request_process(transfer_request);
                
                else
                {
                    /* We are in a OUT transaction. Check if there is a data payload. If so, wait for the payload
                       to be delivered.  */
                    if (*(transfer_request -> ux_slave_transfer_request_setup + 6) == 0 &&
                        *(transfer_request -> ux_slave_transfer_request_setup + 7) == 0) 
                
                        /* Call the Control Transfer dispatcher.  */
                        _ux_device_stack_control_request_process(transfer_request);
                
                    else
                    {
                
                        /* The next transfer is Data coming from the host.  */
                        transfer_request -> ux_slave_transfer_request_phase = UX_TRANSFER_PHASE_DATA_IN;
            
                        /* Get the length we expect from the SETUP packet.  */
                        transfer_request -> ux_slave_transfer_request_requested_length = _ux_utility_short_get(transfer_request -> ux_slave_transfer_request_setup + 6);

                        /* Check if we have enough space for the request.  */
                        if (transfer_request -> ux_slave_transfer_request_requested_length > UX_SLAVE_REQUEST_CONTROL_MAX_LENGTH)
                        {

                            /* No space available, stall the endpoint.  */
                            _ux_dcd_rx_endpoint_stall(dcd_rx, endpoint);

                            /* Next phase is a SETUP.  */
                            ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_IDLE;

                            /* Done here.  */
                            return(UX_ERROR);
                        }
                        else
                        {

                            /* Reset what we have received so far.  */
                            transfer_request -> ux_slave_transfer_request_actual_length =  0;
                        
                            /* And reprogram the current buffer address to the beginning of the buffer.  */
                            transfer_request -> ux_slave_transfer_request_current_data_pointer =  transfer_request -> ux_slave_transfer_request_data_pointer;
                
                            /* Set PID to NAK for Control Endpoint.  */
                            _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_PIPECTR_PID_MASK);
    
                            /* Clear the BRDY and BEMP status for this pipe.  */
                            _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_BEMPSTS, (1ul << ed -> ux_dcd_rx_ed_index));
                            _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_BRDYSTS, (1ul << ed -> ux_dcd_rx_ed_index));
    
                            /* We have a request for a OUT transaction from the host.  */
                            /* Clear the FIFO buffer memory. */
                            _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_CFIFOCTR, UX_RX_DCD_FIFOCTR_BCLR);
                
                            /* Set the current endpoint fifo.  */
                            _ux_dcd_rx_current_endpoint_change(dcd_rx, ed, 0);
                    
                            /* Enable the BRDY interrupt.  */
                            _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_ENABLE);
                       
                            /* Set PID to BUF for Control Endpoint.  */
                            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_PIPECTR_PIDBUF);
    
                            /* Waiting for data on control endpoint.  */
                            ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_DATA_RX;
                        }
                
                    }                            
                }
                
                /* Check if the transaction is OUT and there is no data payload.  */
                if (((*transfer_request -> ux_slave_transfer_request_setup & UX_REQUEST_IN) == 0) && 
                        *(transfer_request -> ux_slave_transfer_request_setup + 6) == 0 &&
                        *(transfer_request -> ux_slave_transfer_request_setup + 7) == 0) 
                {
                
                    /* In the case of a SETUP followed by NO data payload, we let the controller reply to the next 
                       zero length IN packet. Reset the length to transfer. */
                    transfer_request -> ux_slave_transfer_request_in_transfer_length =  0;
                    transfer_request -> ux_slave_transfer_request_requested_length =    0;
                
                    /* Set the phase of the transfer to data OUT for status.  */
                    transfer_request -> ux_slave_transfer_request_phase =  UX_TRANSFER_PHASE_DATA_OUT;
            
                    /* Control endpoint is status.  */
                    ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_STATUS_TX;
    
                    /* Set PID to BUF and CCPL for Control Endpoint to indicate STATUS phase.  */
                    _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, (UX_RX_DCD_DCPCTR_PIDBUF | UX_RX_DCD_DCPCTR_CCPL));
                
                }            
               else
               {
                
                   /* Check if data transfer is OUT.  */
                   if ((*transfer_request -> ux_slave_transfer_request_setup & UX_REQUEST_IN) == 0)  
                       
                       /* Set the state to RX.  */
                       ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_DATA_RX;
        
                   else                    
    
                   /* Set the state to TX.  */
                   ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_DATA_TX;
               }
                            
                break;
        }
    }
    else
    {

        /* Check if we have a BRDY/BEMP interrupt.  */
        if (interrupt_status == UX_RX_DCD_INTSTS0_BRDY || interrupt_status == UX_RX_DCD_INTSTS0_BEMP)
        {
            
            

            /* Check Direction.  */
            if (transfer_request -> ux_slave_transfer_request_phase == UX_TRANSFER_PHASE_DATA_IN)
            {

                

                /* Get the size of the transfer, used for a IN transaction only.  */
                fifo_length =  transfer_request -> ux_slave_transfer_request_requested_length;

                /* Keep the FIFO length in the endpoint.  */
                ed -> ux_dcd_rx_ed_payload_length =  fifo_length;

                /* Check if the endpoint size is bigger that data requested. */
                if (fifo_length > endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize)

                    /* Adjust the transfer size.  */
                    ed -> ux_dcd_rx_ed_payload_length =  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

                /* Point the FIFO buffer to the current transfer request buffer address.  */
                ed -> ux_dcd_rx_ed_payload_address =  transfer_request -> ux_slave_transfer_request_current_data_pointer;
        
                /* Read the payload.  */
                status = _ux_dcd_rx_buffer_read(dcd_rx, ed);
                
                /* Check the error status.  */
                if (status != UX_SUCCESS)
                {
                 
                    /* Are we with using a non interrupt endpoint.  */
                    if (ed -> ux_dcd_rx_ed_index != 0)
                    {

                        /* The transfer is completed with error.  */
                        transfer_request -> ux_slave_transfer_request_status =  UX_TRANSFER_ERROR;

                        /* Non control endpoint operation, use semaphore.  */
                        _ux_utility_semaphore_put(&transfer_request -> ux_slave_transfer_request_semaphore);
                    }                            
                }
                else
                {                

                    /* Get the actual length received.  */
                    fifo_length = ed -> ux_dcd_rx_ed_actual_length;
                    
                    /* Adjust the data pointer.  */
                    transfer_request -> ux_slave_transfer_request_current_data_pointer += fifo_length;
                    
                    /* Adjust the transfer length remaining.  */
                    transfer_request -> ux_slave_transfer_request_in_transfer_length -= fifo_length;
    
                    /* Update the length of the data transmitted.  */
                    transfer_request -> ux_slave_transfer_request_actual_length += fifo_length;
    
                    /* Are we done with this transfer ? */
                    if ((transfer_request -> ux_slave_transfer_request_actual_length ==                         
                            transfer_request -> ux_slave_transfer_request_requested_length) || 
                            (fifo_length != endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize))
                    {
    
                        /* Set the completion code to no error.  */
                        transfer_request -> ux_slave_transfer_request_completion_code =  UX_SUCCESS;

                        /* Disable the Ready interrupt.  */
                        _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
    
                        /* Check what type of endpoint we are using, Control or non Control.  */
                        if (ed -> ux_dcd_rx_ed_index != 0)
                        {
    
                            /* The transfer is completed.  */
                            transfer_request -> ux_slave_transfer_request_status =  UX_TRANSFER_STATUS_COMPLETED;
    
                            /* Non control endpoint operation, use semaphore.  */
                            _ux_utility_semaphore_put(&transfer_request -> ux_slave_transfer_request_semaphore);
                        }                        
    
                        else 
                        {
                        
                            /* Check the phase for control endpoint.  */
                            if (ed -> ux_dcd_rx_ed_state ==  UX_DCD_RX_ED_STATE_DATA_RX)
                            {                
    
                                /* We are using a Control endpoint on a OUT transaction and there was a payload.  */
                                _ux_device_stack_control_request_process(transfer_request);
    
                                /* Set the state to STATUS phase TX.  */
                                ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_STATUS_TX;
    
                                /* We are using a Control endpoint, if there is a callback, invoke it. We are still under ISR.  */
                                if (transfer_request -> ux_slave_transfer_request_completion_function) 
                                    transfer_request -> ux_slave_transfer_request_completion_function (transfer_request) ;  
        
                                /* In the case of a SETUP with data transitioning to STATUS let the controller reply to the next 
                                   zero length IN packet. Reset the length to transfer. */
                                transfer_request -> ux_slave_transfer_request_in_transfer_length =  0;
                                transfer_request -> ux_slave_transfer_request_requested_length =    0;
                    
                                /* Set the phase of the transfer to data OUT for status.  */
                                transfer_request -> ux_slave_transfer_request_phase =  UX_TRANSFER_PHASE_DATA_OUT;
                    
                                /* Control endpoint is status.   */
                                ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_STATUS_TX;
    
                                /* Set PID to BUF and CCPL for Control Endpoint to indicate STATUS phase.  */
                                _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, (UX_RX_DCD_DCPCTR_PIDBUF | UX_RX_DCD_DCPCTR_CCPL));
                
                            }                    
                        }                           
                    }
                    else
                    {

                        /* Set PID to BUF. Isolate Control endpoint and others.  */
                        if (ed -> ux_dcd_rx_ed_index == 0)
    
                            /* Set PID to BUF for Control Endpoint.  */
                            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_PIPECTR_PIDBUF);

                        else
    
                            /* Set PID to BUF for non Control Endpoints.  Continue to receive.  */
                            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index -1) * 2), UX_RX_DCD_PIPECTR_PIDBUF);
                    }
                }
            }
            else
            {
                
                        
                /* Check if we have data to send.  */
                if (transfer_request -> ux_slave_transfer_request_in_transfer_length == 0)
                {
            
                    /* There is no data to send but we may need to send a Zero Length Packet.  */
                    if (transfer_request -> ux_slave_transfer_request_force_zlp ==  UX_TRUE)
                    {
                    

                        /* Reset the FIFO payload length.  */
                        ed -> ux_dcd_rx_ed_payload_length =  0;
            
                        /* Write the empty buffer to the pipe.  */
                        status =  _ux_dcd_rx_buffer_write(dcd_rx, ed); 

                        /* Check status.  */
                        if (status != UX_ERROR)         
                        {                           
                        
                            /* Set PID to BUF. Isolate Control endpoint and others.  */
                            if (ed -> ux_dcd_rx_ed_index == 0)
    
                                /* Set PID to BUF for Control Endpoint.  */
                                _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_PIPECTR_PIDBUF);

                            else
    
                                /* Set PID to BUF for non Control Endpoints.  */
                                _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index -1) * 2), UX_RX_DCD_PIPECTR_PIDBUF);
                        }

                        /* Reset the ZLP condition.  */
                        transfer_request -> ux_slave_transfer_request_force_zlp =  UX_FALSE;
                    }
                    else
                    {
                    
                        /* Set the completion code to no error.  */
                        transfer_request -> ux_slave_transfer_request_completion_code =  UX_SUCCESS;
                        
                        /* Disable the Buffer empty interrupt.  */
                        _ux_dcd_rx_buffer_empty_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);

                        /* Check what type of endpoint we are using, Control or non Control.  */
                        if (ed -> ux_dcd_rx_ed_index != 0)
                        {

                            /* The transfer is completed.  */
                            transfer_request -> ux_slave_transfer_request_status =  UX_TRANSFER_STATUS_COMPLETED;

                            /* Non control endpoint operation, use semaphore.  */
                            _ux_utility_semaphore_put(&transfer_request -> ux_slave_transfer_request_semaphore);
                            
                        }                            
                        else 
                        {

                            /* We get here when we may have a transition from DATA phase to STATUS phase on a control transfer.  */
                            if(ed -> ux_dcd_rx_ed_state ==  UX_DCD_RX_ED_STATE_DATA_TX)
                            {

                                /* We need to complete the Control transfer by a status phase.  */
                                transfer_request -> ux_slave_transfer_request_in_transfer_length =  0;
                                transfer_request -> ux_slave_transfer_request_requested_length =    0;
                
                                /* Set the phase of the transfer to data OUT for status.  */
                                transfer_request -> ux_slave_transfer_request_phase =  UX_TRANSFER_PHASE_DATA_OUT;
                
                                /* Control endpoint is status.   */
                                ed -> ux_dcd_rx_ed_state =  UX_DCD_RX_ED_STATE_STATUS_TX;

                                /* Set PID to BUF and CCPL for Control Endpoint to indicate STATUS phase.  */
                                _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, (UX_RX_DCD_DCPCTR_PIDBUF | UX_RX_DCD_DCPCTR_CCPL));
                                
                            }

                            /* We are using a Control endpoint, if there is a callback, invoke it. We are still under ISR.  */
                            if (transfer_request -> ux_slave_transfer_request_completion_function)  
                                transfer_request -> ux_slave_transfer_request_completion_function (transfer_request) ;  
                        }                           
                    }
                }
                else
                {
                            
                    /* Get the size of the transfer, used for a IN transaction only.  */
                    fifo_length =  transfer_request -> ux_slave_transfer_request_in_transfer_length;
    
                    /* Check if the endpoint size is bigger that data requested. */
                    if (fifo_length > endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize)
                    {            

                        /* Adjust the transfer size.  */
                        fifo_length =  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
                    }

                    /* Save the amount of bytes to transfer here.  */
                    ed -> ux_dcd_rx_ed_packet_length = fifo_length;

                    /* Point the FIFO buffer to the current transfer request buffer address.  */
                    ed -> ux_dcd_rx_ed_payload_address =  transfer_request -> ux_slave_transfer_request_current_data_pointer;

                    /* Write the buffer to the pipe.  */
                    status =  _ux_dcd_rx_buffer_write(dcd_rx, ed); 
                    
                   /* Keep the FIFO length in the endpoint.  */
                    ed -> ux_dcd_rx_ed_payload_length -=  fifo_length;
            
                    /* Adjust the data pointer.  */
                    transfer_request -> ux_slave_transfer_request_current_data_pointer += fifo_length;
                    
                    /* Adjust the transfer length remaining.  */
                    transfer_request -> ux_slave_transfer_request_in_transfer_length -= fifo_length;

                    /* Update the length of the data transmitted.  */
                    transfer_request -> ux_slave_transfer_request_actual_length += fifo_length;

                    /* Check status.  */
                    if (status != UX_ERROR)         
                    {                           
                        /* Set PID to BUF. Isolate Control endpoint and others.  */
                        if (ed -> ux_dcd_rx_ed_index == 0)
    
                            /* Set PID to BUF for Control Endpoint.  */
                            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_DCPCTR, UX_RX_DCD_PIPECTR_PIDBUF);

                        else
    
                            /* Set PID to BUF for non Control Endpoints.  */
                            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_PIPE1CTR + ((ed -> ux_dcd_rx_ed_index -1) * 2), UX_RX_DCD_PIPECTR_PIDBUF);
                    }
                }
            }
        }
    }

    /* We are done.  */
    return(UX_SUCCESS);
}

