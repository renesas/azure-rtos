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
/*    _ux_dcd_rx_buffer_write                             PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function writes a buffer to the specified PIPE                 */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                              Pointer to rx controller        */ 
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
UINT  _ux_dcd_rx_buffer_write(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed)
{

UINT    status;

    /* Select the fifo.  */
    switch (ed -> ux_dcd_rx_ed_fifo_index)
    {

        case UX_RX_DCD_FIFO_D0        :
        case UX_RX_DCD_FIFO_D1        :
        
            /* Write to the FIFO D0 or D1.  */
            status = _ux_dcd_rx_fifod_write(dcd_rx, ed);
            break;

        case UX_RX_DCD_FIFO_C         :
        
            /* Write to the FIFO C1.  */
            status = _ux_dcd_rx_fifoc_write(dcd_rx, ed);
            break;

        default                        :
            break;

    }

    /* Analyze return status.  */
    switch (status)
    {
    
        case UX_RX_DCD_FIFO_WRITING        :
        
            /* Check Pipe. DCP is different.  */
            if (ed -> ux_dcd_rx_ed_index == 0)
            
                /* Enable the Buffer empty interrupt. Only for DCP. */
                _ux_dcd_rx_buffer_empty_interrupt(dcd_rx, ed, UX_DCD_RX_ENABLE);
            
            /* Enable the Ready interrupt.  */
            _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_ENABLE);

            /* Set success.  */
            status = UX_SUCCESS;

            break;        

        case UX_RX_DCD_FIFO_WRITE_END        :
        case UX_RX_DCD_FIFO_WRITE_SHORT    :

            /* Disable the Ready interrupt.  */
            _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
            
            /* Enable the Buffer empty interrupt.  */
            _ux_dcd_rx_buffer_empty_interrupt(dcd_rx, ed, UX_DCD_RX_ENABLE);
            
            /* Set success.  */
            status = UX_SUCCESS;

            break;        
        
        case UX_RX_DCD_FIFO_WRITE_DMA        :
            break;
            
        case UX_RX_DCD_FIFO_WRITE_ERROR    :
        default                             :
        
            /* Disable the Ready interrupt.  */
            _ux_dcd_rx_buffer_ready_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
            
            /* Disable the Buffer empty interrupt.  */
            _ux_dcd_rx_buffer_empty_interrupt(dcd_rx, ed, UX_DCD_RX_DISABLE);
            
            /* Set error.  */
            status = UX_ERROR;
            
            break;        

    }

    /* Return to caller. */
    return(status);
}

