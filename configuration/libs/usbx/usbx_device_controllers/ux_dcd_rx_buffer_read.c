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
#include "ux_system.h"
#include "ux_utility.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_dcd_rx_buffer_read                              PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function reads from a specified pipe into a buffer.            */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                              Pointer to rx controller        */ 
/*    ed                                    Register to the ed            */ 
/*    transfer_request                      Transfer request              */ 
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
UINT  _ux_dcd_rx_buffer_read(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed)
{

UINT    status;
    
    
    /* Read from the FIFO C or D0 or D1.  */
    status = _ux_dcd_rx_fifo_read(dcd_rx, ed);

    /* Analyze return status.  */
    switch (status)
    {
    
        case UX_RX_DCD_FIFO_READING     :
        
            /* Enable the Ready interrupt.  */
            _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_ENABLE);
            
            /* Operation was OK.  */
            status = UX_SUCCESS;
            
            break;      

        case UX_RX_DCD_FIFO_READ_END        :
        case UX_RX_DCD_FIFO_READ_SHORT  :

            /* Disable the Ready interrupt.  */
            _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
            
            /* Operation was OK.  */
            status = UX_SUCCESS;
            
            break;      
        
        case UX_RX_DCD_FIFO_READ_DMA        :
            break;
            
        case UX_RX_DCD_FIFO_READ_ERROR  :
        default                             :
        
            /* Disable the Ready interrupt.  */
            _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
            
            /* Disable the Buffer empty interrupt.  */
            _ux_dcd_rx_buffer_empty_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
            
            /* Operation was not OK.  */
            status = UX_ERROR;
            
            break;      

    }

    /* Return to caller. */
    return(status);
}

