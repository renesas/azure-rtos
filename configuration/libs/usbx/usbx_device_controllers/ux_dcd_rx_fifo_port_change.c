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
/*    _ux_dcd_rx_fifo_port_change                         PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function change the port of the FIFO                           */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                              Pointer to rx controller        */ 
/*    ed                                    Register to the ed            */ 
/*    direction                             Direction to switch           */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    fifo_access_status                                                  */ 
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
ULONG  _ux_dcd_rx_fifo_port_change(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed, ULONG direction)
{

ULONG   loop_index;
ULONG   rx_register;

    /* Set the current fifo port.  */
    _ux_dcd_rx_current_endpoint_change(dcd_rx, ed, direction);

    /* According to the RX spec, it make take several tries to perform this function.  */
    for (loop_index = 0; loop_index < 4; loop_index++) 
    {

        /* Select the fifo.  */
        switch (ed -> ux_dcd_rx_ed_fifo_index)
        {
    
            case UX_RX_DCD_FIFO_D0      :
                
                /* Read the current endpoint index (pipe) in the D0FIFOSEL register.  */
                rx_register =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_D0FIFOCTR);
                break;        
            
            case UX_RX_DCD_FIFO_D1      :
            
                /* Read the current endpoint index (pipe) in the D1FIFOSEL register.  */
                rx_register =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_D1FIFOCTR);
                break;        
    
            case UX_RX_DCD_FIFO_C       :
    
                /* Set the current endpoint index (pipe) in the CFIFOSEL register.  */
                rx_register =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_CFIFOCTR);
                break;
    
            default                     :

                /* We should never get here.  */
                rx_register = 0;
            
        }
        
        /* Now check the status of the FRDY bit.  */
        if ((rx_register & UX_RX_DCD_FIFOCTR_FRDY) == UX_RX_DCD_FIFOCTR_FRDY)
            
            return(rx_register);

    }

    /* Just waste a little time ...  */
    rx_register =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_SYSCFG);
    rx_register =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_SYSSTS);

    /* Return to caller with an error. */
    return(UX_ERROR);
}

