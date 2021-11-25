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
/*    _ux_dcd_rx_fifo_read                                PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function reads a buffer from FIFO C D0 or D1                   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                           Pointer to rx controller           */ 
/*    ed                                 Register to the ed               */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                                              */ 
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
/*                                            Used UX_ things instead of  */
/*                                            TX_ things directly,        */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _ux_dcd_rx_fifo_read(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed)
{

ULONG           fifo_access_status;
ULONG           max_packet_size;
ULONG           payload_length;
UINT            status;
UCHAR           *payload_buffer;
VOID            *fifo_addr;
ULONG           fifo_ctrl;
UX_INTERRUPT_SAVE_AREA


    /* Critical section, even ISR cannot be disrupted.  */
    UX_DISABLE
    
    /* We need to select the FIFO registers.  */
    switch (ed -> ux_dcd_rx_ed_fifo_index)
    {

        case  UX_RX_DCD_FIFO_C      :

            /* Set fifo_sel and fifo_addr fields to FIFO_C */
            fifo_addr =  (VOID *)(dcd_rx->ux_dcd_rx_base + UX_RX_DCD_CFIFO);
            fifo_ctrl =  UX_RX_DCD_CFIFOCTR;
            break;

        case  UX_RX_DCD_FIFO_D0     :
        
            /* Set fifo_sel and fifo_addr fields to FIFO_D0 */
            fifo_addr =  (VOID *)(dcd_rx->ux_dcd_rx_base + UX_RX_DCD_D0FIFO);
            fifo_ctrl =  UX_RX_DCD_D0FIFOCTR;
            break; 

        case  UX_RX_DCD_FIFO_D1     :

            /* Set fifo_sel and fifo_addr fields to FIFO_D1 */
            fifo_addr =  (VOID *)(dcd_rx->ux_dcd_rx_base + UX_RX_DCD_D1FIFO);
            fifo_ctrl =  UX_RX_DCD_D1FIFOCTR;
            break;
    }

    
    /* Get the Fifo access status for the endpoint.  */
    fifo_access_status =  _ux_dcd_rx_fifo_port_change(dcd_rx, ed, 0);
    
    /* Check Status.  */
    if (fifo_access_status == UX_ERROR)
    {
            
        /* Free the INTs now.  */
        UX_RESTORE

        /* We have an error. Abort.  */
        return(UX_ERROR);
    }

    /* Get the max packet size for this endpoint.  */
    max_packet_size = ed -> ux_dcd_rx_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
    
    /* Isolate the payload length.  */
    payload_length = fifo_access_status & UX_RX_DCD_FIFOCTR_DTLN;
    
    /* Save the payload length in the ED. This is needed to check for end of transfer.  */
    ed -> ux_dcd_rx_ed_actual_length =  payload_length;

    /* Set NAK.  */
    _ux_dcd_rx_endpoint_nak_set(dcd_rx, ed);

    /* Check for overwrite.  */
    if (ed -> ux_dcd_rx_ed_payload_length < payload_length)
    {

        /* Set Status to read overflow.  */
        status = UX_RX_DCD_FIFO_READ_OVER;
        
        /* Set the payload length to the size wanted by the caller.  */
        payload_length = ed -> ux_dcd_rx_ed_payload_length;

    }

    else
    {

        /* Check for short packet.  */
        if (payload_length == 0 || payload_length % max_packet_size != 0)
        
            /* We have a short packet.  */
            status = UX_RX_DCD_FIFO_READ_SHORT;

        else
        
            /* Continue reading.  */
            status = UX_RX_DCD_FIFO_READING;
        
    }

    /* Check for 0 length packet.  */
    if (payload_length == 0)

        /* Set the BCLR flag.  */
        _ux_dcd_rx_register_set(dcd_rx, fifo_ctrl, UX_RX_DCD_FIFOCTR_BCLR);
        
    else
    {

        /* Get the payload buffer address.  */
        payload_buffer =  ed -> ux_dcd_rx_ed_payload_address;
        
        /* Write 16 bits values.  */
        while (payload_length != 0)                  
        {
            
            /* Read the 8 bit value.  */
            *payload_buffer =  *((UCHAR *) fifo_addr);
    
            /* Update the payload buffer address.  */
            payload_buffer += sizeof(UCHAR);
        
            /* And the remaining length.  */
            payload_length -= sizeof(UCHAR);
            
        }
    }
            /* Free the INTs now.  */
    UX_RESTORE

    /* Return status about buffer transfer.  */
    return(status);
}

