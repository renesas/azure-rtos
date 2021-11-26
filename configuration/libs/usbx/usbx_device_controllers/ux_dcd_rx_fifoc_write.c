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
/*    _ux_dcd_rx_fifod_write                              PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function writes a buffer to FIFOC                              */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                                Pointer to rx controller      */ 
/*    ed                                    Register to the ed            */ 
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
UINT  _ux_dcd_rx_fifoc_write(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed)
{

ULONG            fifo_access_status;
ULONG            max_packet_size;
ULONG            data_buffer_size;
ULONG            payload_length;
UINT             status;
UCHAR            *payload_buffer;
ULONG            rx_register;
UCHAR            *cfifo_ptr;
UX_INTERRUPT_SAVE_AREA

    /* Critical section, even ISR cannot be disrupted.  */
    UX_DISABLE

    /* Check if we are using endpoint 0.  */
    if (ed -> ux_dcd_rx_ed_index == 0)
    
    
        /* Get the Fifo access status for endpoint 0.  */
        fifo_access_status =  _ux_dcd_rx_fifo_port_change(dcd_rx, ed, UX_RX_DCD_CFIFOSEL_ISEL);

    else
    
        /* Get the Fifo access status for non endpoint 0.  */
        fifo_access_status =  _ux_dcd_rx_fifo_port_change(dcd_rx, ed, 0);
    
    /* Check Status.  */
    if (fifo_access_status == UX_ERROR)
    {        

        /* Free the INTs now.  */
        UX_RESTORE

        /* We have an error. Abort.  */
        return(UX_ERROR);
    }

    /* Get the data buffer size.  */
    data_buffer_size = _ux_dcd_rx_data_buffer_size(dcd_rx, ed);

    /* Get the max packet size for this endpoint.  */
    max_packet_size = ed -> ux_dcd_rx_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
    
    /* Check if this transfer takes more than one packet.  */
    if (ed -> ux_dcd_rx_ed_payload_length <= max_packet_size) 
    {

        /* Set the payload to the TD payload length.  */
        payload_length =  ed -> ux_dcd_rx_ed_payload_length;

        /* Set Status to write ends.  */
        status = UX_RX_DCD_FIFO_WRITE_END;
        
        /* Check for 0 packet.  */
        if ((payload_length == 0) || (payload_length % max_packet_size ) != 0)

            /* Set Status to write short.  */
            status = UX_RX_DCD_FIFO_WRITE_SHORT;
    }

    else
    {
        
        /* We are doing a multi write.  */
        status = UX_RX_DCD_FIFO_WRITING;
                
        /* Payload length is the buffer size.  */
        payload_length = data_buffer_size;
        
    }      
    
    /* Get the payload buffer address.  */
    payload_buffer =  ed -> ux_dcd_rx_ed_payload_address;
    
    /* Memorize packet length.  */
    ed -> ux_dcd_rx_ed_packet_length = payload_length;
    
    cfifo_ptr = (UCHAR *)(dcd_rx -> ux_dcd_rx_base + UX_RX_DCD_CFIFO);
#ifdef UX_RX71M
    if ((UX_RX_USB_BASE == UX_RX_USBA_BASE) || (dcd_rx -> ux_dcd_rx_base == UX_RX_USBA_BASE))
    {
        cfifo_ptr += 3;
    }
#endif
    /* Write 8 bits values.  */
    while (payload_length != 0)                  
    {
        
        /* Write the 8 bit value.  */
        *cfifo_ptr =  *payload_buffer;

        /* Update the payload buffer address.  */
        payload_buffer += sizeof(UCHAR);
    
        /* And the remaining length.  */
        payload_length -= sizeof(UCHAR);
        
    }
    
    /* Check status. If we have a short packet, we need to set 
       the BVAL flag  in the Fifo controller.  */
    if (status != UX_RX_DCD_FIFO_WRITING)
    {

        /* Read the current FIFO control register.  */
        rx_register = _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_CFIFOCTR);

        /* Check if the BVAL bit is already set.  */
        if ((rx_register & UX_RX_DCD_FIFOCTR_BVAL) == 0)

            /* No so set it.  This will enable the Buffer Memory Valid flag.  
               This bit is set to 1 when data has been written to the FIFO and
               this is a short packet or zero packet or a full packet but not the
               end of the transmission.  */
            _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_CFIFOCTR, UX_RX_DCD_FIFOCTR_BVAL);
    }  

    /* Free the INTs now.  */
    UX_RESTORE    

    /* Return status about buffer transfer.  */
    return(status);
}

