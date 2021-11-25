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
/*    This function writes a buffer to FIFOD0 or FIFOD1                   */ 
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
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _ux_dcd_rx_fifod_write(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed)
{

ULONG            fifo_access_status;
ULONG            max_packet_size;
ULONG            data_buffer_size;
ULONG            payload_length;
UINT             status;
UCHAR            *payload_buffer;
VOID             *fifo_addr;
ULONG            fifo_ctrl;
ULONG            rx_register;

    /* Get the Fifo access status.  */
    fifo_access_status =  _ux_dcd_rx_fifo_port_change(dcd_rx, ed, 0);
    
    /* Check Status.  */
    if (fifo_access_status != UX_SUCCESS)
        
        /* We have an error. Abort.  */
        return(fifo_access_status);

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
    
    /* We need to select the FIFO registers.  */
    if (ed -> ux_dcd_rx_ed_fifo_index == UX_RX_DCD_FIFO_D0)
    {

        /* Set fifo_sel and fifo_addr fields to FIFO_D0 */
        fifo_addr =     (VOID *)(dcd_rx -> ux_dcd_rx_base + UX_RX_DCD_D0FIFO);
        fifo_ctrl =  UX_RX_DCD_D0FIFOCTR;
    }
    else
    {

        /* Set fifo_sel and fifo_addr fields to FIFO_D1 */
        fifo_addr =     (VOID *)(dcd_rx -> ux_dcd_rx_base + UX_RX_DCD_D1FIFO);
        fifo_ctrl =  UX_RX_DCD_D1FIFOCTR;
    
    }
#ifdef UX_RX71M
    if ((UX_RX_USB_BASE == UX_RX_USBA_BASE) || (dcd_rx -> ux_dcd_rx_base == UX_RX_USBA_BASE))
    {
        fifo_addr = (VOID *)((UINT)fifo_addr + 3);
    }
#endif
    /* Get the payload buffer address.  */
    payload_buffer =  ed -> ux_dcd_rx_ed_payload_address;
    
    /* Memorize packet length.  */
    ed -> ux_dcd_rx_ed_packet_length = payload_length;
    
    /* Write 16 bits values.  */
    while (payload_length != 0)                  
    {
        
        /* Read the 8 bit value.  */
        *((UCHAR *)fifo_addr) =  *payload_buffer;

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
        rx_register = _ux_dcd_rx_register_read(dcd_rx, fifo_ctrl);

        /* Check if the BVAL bit is already set.  */
        if ((rx_register & UX_RX_DCD_FIFOCTR_BVAL) == 0)

            /* No so set it.  This will enable the Buffer Memory Valid flag.  
               This bit is set to 1 when data has been written to the FIFO and
               this is a short packet or zero packet or a full packet but not the
               end of the transmission.  */
            _ux_dcd_rx_register_set(dcd_rx, fifo_ctrl, UX_RX_DCD_FIFOCTR_BVAL);
    }       

    /* Return status about buffer transfer.  */
    return(status);
}

