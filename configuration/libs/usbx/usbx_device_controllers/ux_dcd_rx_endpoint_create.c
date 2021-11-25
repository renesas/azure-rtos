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
/*    _ux_dcd_rx_endpoint_create                          PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will create a physical endpoint.                      */
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
/*    _ux_dcd_rx_endpoint_reset           Reset endpoint                  */ 
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
/*                                            Used UX_ things instead of  */
/*                                            TX_ things directly,        */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _ux_dcd_rx_endpoint_create(UX_DCD_RX *dcd_rx, UX_SLAVE_ENDPOINT *endpoint)
{

UX_DCD_RX_ED        *ed;
ULONG               rx_endpoint_index;
ULONG               rx_endpoint_mask;
ULONG               pipe_index;
ULONG               buffer_index;
ULONG               current_pipe_index;
ULONG               endpoint_direction;
ULONG               endpoint_type;
ULONG               pipe_type;
ULONG               pipe_selection_status;

UX_INTERRUPT_SAVE_AREA
                                                            
    /* The endpoint index in the array of the rx must match the endpoint number. 
       The rx has 10 endpoints maximum.  */
    rx_endpoint_index =  endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & ~(ULONG)UX_ENDPOINT_DIRECTION;
    
    /* Fetch the address of the physical endpoint.  */
    ed =  &dcd_rx -> ux_dcd_rx_ed[rx_endpoint_index];

    /* Check the endpoint status, if it is free, reserve it. If not reject this endpoint.  */
    if ((ed -> ux_dcd_rx_ed_status & UX_DCD_RX_ED_STATUS_USED) == 0)
    {
        
        /* We can use this endpoint.  */
        ed -> ux_dcd_rx_ed_status |=  UX_DCD_RX_ED_STATUS_USED;

        /* Keep the physical endpoint address in the endpoint container.  */
        endpoint -> ux_slave_endpoint_ed =  (VOID *) ed;

        /* Save the endpoint pointer.  */
        ed -> ux_dcd_rx_ed_endpoint =  endpoint;
        
        /* Build the endpoint mask from the endpoint descriptor.  */
        rx_endpoint_mask =  endpoint -> ux_slave_endpoint_descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE;
        
        /* Set the endpoint type and direction.  */
        switch (rx_endpoint_mask)
        {

        case UX_CONTROL_ENDPOINT:

            /* Control pipe index is always 0.  */
            pipe_index = 0;
            ed -> ux_dcd_rx_ed_buffer_number =  0;
            ed -> ux_dcd_rx_ed_buffer_size =  (( endpoint ->  ux_slave_endpoint_descriptor.wMaxPacketSize) / 64);
            break;                

        case UX_BULK_ENDPOINT:

            /* Obtain the first available pipe. */
            current_pipe_index = 1;
            pipe_index = 0;
            
            /* Take a pessimist view on endpoint selection.  */
            pipe_selection_status = UX_ERROR;
            
            while( ( current_pipe_index <= 5 ) && ( pipe_index ==  0 ) )
            {
            
                /* Select pipe 1 - 5.  */
                _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_PIPESEL, current_pipe_index);
               
                /* Check whether pipe is in use.  */
                pipe_type =  (_ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_PIPECFG) & UX_RX_DCD_PIPECFG_EPNUM_MASK);
            
                /* Not in use, choose this one.  */
                if (pipe_type == 0)
                {
                
                    /* This is the pipe index to use.  */
                    pipe_index = current_pipe_index;  
                    
                    /* Memorize the pipe associated with the endpoint.  */
                    dcd_rx -> ux_dcd_rx_pipe[current_pipe_index] = rx_endpoint_index;
                    
                    /* We are done here.  */
                    pipe_selection_status = UX_SUCCESS;
                    
                    /* We are done with pipe selection.  */
                    break;
                   
                }                    
                    
                /* Increment the current index.  */
                current_pipe_index++;
            }
            
            /* Endpoint selected ? */
            if (pipe_selection_status != UX_SUCCESS)
                
                /* Cannot proceed.  */
                return(UX_ERROR);

            /* Set the endpoint type to BULK.  */
            endpoint_type =  UX_RX_DCD_PIPECFG_TYPE_BULK;

            /* Configure the  endpoint  hardware, pick up buffer size & number. */
            ed -> ux_dcd_rx_ed_buffer_number =  dcd_rx -> ux_dcd_rx_next_available_bufnum;
            ed -> ux_dcd_rx_ed_buffer_size =  UX_DCD_RX_MAX_BULK_PAYLOAD / UX_DCD_RX_MAX_BUF_SIZE;
            
            /* Calculate the next available free buffer.  */
            dcd_rx -> ux_dcd_rx_next_available_bufnum =  (dcd_rx -> ux_dcd_rx_next_available_bufnum ) + ( ed -> ux_dcd_rx_ed_buffer_size);
            if (dcd_rx -> ux_dcd_rx_next_available_bufnum > UX_RX_DCD_MAX_BUF_NUM)
                 return (UX_ERROR); 
                 
            break;

        case UX_INTERRUPT_ENDPOINT:

            /* Set first buffer number. */
            buffer_index = 4;
            pipe_index = 0;
            
            /* Take a pessimist view on endpoint selection.  */
            pipe_selection_status = UX_ERROR;
            
            /* Browse Pipes from 6 to 9 which can be interrupt.  */
            for (current_pipe_index = 6; current_pipe_index < 10; current_pipe_index++)
            {
            
                /* Select pipe 6 - 9.  */
                _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_PIPESEL, current_pipe_index);
               
                        
                /* Check whether pipe is in use.  */
                pipe_type =  (_ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_PIPECFG) & UX_RX_DCD_PIPECFG_EPNUM_MASK);
            
                /* Not in use, choose this one.  */
                if (pipe_type == 0)
                {
        
                    /* Set the pipe index.  */
                    pipe_index = current_pipe_index;
        
                    /* Memorize the pipe associated with the endpoint.  */
                    dcd_rx -> ux_dcd_rx_pipe[current_pipe_index] = rx_endpoint_index;
                    
                    /* We are done here.  */
                    pipe_selection_status = UX_SUCCESS;
                   
                    /* We are done with pipe selection.  */
                    break;
                }            
        
                /* Next buffer.  */
                buffer_index++;
        
            }
            
            /* Endpoint selected ? */
            if (pipe_selection_status != UX_SUCCESS)
                
                /* Cannot proceed.  */
                return(UX_ERROR);

            /* Set the endpoint type to Interrupt.  */
            endpoint_type =  UX_RX_DCD_PIPECFG_TYPE_INTERRUPT;

            /* Configure the  endpoint  hardware, pick up buffer size. Number is fixed. */
            ed -> ux_dcd_rx_ed_buffer_number =  buffer_index;
            ed -> ux_dcd_rx_ed_buffer_size =  0;

            break;                

        default:

            return(UX_ERROR);
        }

        /* Memorize the endpoint index.  */
        ed -> ux_dcd_rx_ed_index =  pipe_index;

        /* For pipes, use FIFO_C.  */
        ed -> ux_dcd_rx_ed_fifo_index =  UX_RX_DCD_FIFO_C; 

        /* Check endpoint type.  */
        if (rx_endpoint_mask ==  UX_CONTROL_ENDPOINT)
        {

            /* Continue the configuration for control endpoint only.  */
            /* Initialize the DCPMAX register.  */
            _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_DCPMAXP,  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize);
            
        }
        else
        {
    
            /* Continue the configuration for Bulk and Interrupt only.  */
            /* Check if IN or OUT.  */
            if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) == UX_ENDPOINT_IN)
            
                /* Set the direction to OUT.  */
                endpoint_direction = UX_RX_DCD_PIPECFG_DIR;
             
            else
                    
                /* Reset the endpoint direction as bit 4 of PIPECFG is 0 for IN.  */
                endpoint_direction = 0;
                
            /* Set PID to NAK. */
            _ux_dcd_rx_endpoint_nak_set(dcd_rx, ed);
             
            /* Critical section, even ISR cannot be disrupted.  */
            UX_DISABLE
        
            /* Select the endpoint register to map all registers.  */
            _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_PIPESEL, ed -> ux_dcd_rx_ed_index);
        
            /* Set Pipe configuration registers.  */
            _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_PIPECFG, (endpoint_type | UX_RX_DCD_PIPECFG_SHTNAK | endpoint_direction | (endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & 0xF)));
        
            /* Initialize the PIPEMAX register.  */
            _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_PIPEMAXP,  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize);
        
            /* Free the INTs now.  */
            UX_RESTORE
        
        }
        
        /* Reset the endpoint.  */
        _ux_dcd_rx_endpoint_reset(dcd_rx, endpoint);
        
    
        /* Return successful completion.  */
        return(UX_SUCCESS);         
    }

    /* The endpoint is already used.  */
    return(UX_NO_ED_AVAILABLE);
    
}

