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
/*    _ux_dcd_rx_current_endpoint_change                  PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function change the endpoint in the FIFO                       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rx                              Pointer to rx controller        */ 
/*    ed                                    Register to the ed            */ 
/*    direction                             Direction to switch           */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
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
VOID  _ux_dcd_rx_current_endpoint_change(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed, ULONG direction)
{

ULONG               rx_register;
ULONG               rx_register_read_back;

    /* Select the fifo.  */
    switch (ed -> ux_dcd_rx_ed_fifo_index)
    {

        case UX_RX_DCD_FIFO_D0      :
            
            /* Set the current endpoint index (pipe) in the D0FIFOSEL register.  */
            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_D0FIFOSEL, ed -> ux_dcd_rx_ed_index);
            break;        
        
        
        case UX_RX_DCD_FIFO_D1      :
        
            /* Set the current endpoint index (pipe) in the D1FIFOSEL register.  */
            _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_D1FIFOSEL, ed -> ux_dcd_rx_ed_index);
            break;        

        case UX_RX_DCD_FIFO_C       :

            /* Read the current CFIFOSEL register.  */
            rx_register =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_CFIFOSEL);

            /* Clear the ISEL field, the reading/writing width and the previous endpoint index.  */
            rx_register &= ~(UX_RX_DCD_CFIFOSEL_CURPIPE_MASK | UX_RX_DCD_CFIFOSEL_ISEL | UX_RX_DCD_CFIFOSEL_MBW_MASK);
            
            /* Write back the register.  */
            _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_CFIFOSEL, rx_register);

            /* Check the register has been written.  */
            do 
            {

                /* Read the current CFIFOSEL register again.  */
                rx_register_read_back =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_CFIFOSEL);
    
                /* Mask ISEL and CURRPIPE.  */
                rx_register_read_back &= (UX_RX_DCD_CFIFOSEL_CURPIPE_MASK | UX_RX_DCD_CFIFOSEL_ISEL);
            
            }
            while ((rx_register & (UX_RX_DCD_CFIFOSEL_CURPIPE_MASK | UX_RX_DCD_CFIFOSEL_ISEL)) != rx_register_read_back);
            
            
            /* Add the current direction and the new endpoint index.  */
            rx_register |= (ed -> ux_dcd_rx_ed_index | direction);

            /* Write back the register.  */
            _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_CFIFOSEL, rx_register);

            /* Read the current CFIFOSEL register again.  */
            rx_register_read_back =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_CFIFOSEL);
    
            /* Mask ISEL and CURRPIPE.  */
            rx_register_read_back &= (UX_RX_DCD_CFIFOSEL_CURPIPE_MASK | UX_RX_DCD_CFIFOSEL_ISEL);
            
            /* Check the register has been written.  */
            do 
            {

                /* Read the current CFIFOSEL register again.  */
                rx_register_read_back =  _ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_CFIFOSEL);
    
                /* Mask ISEL and CURRPIPE.  */
                rx_register_read_back &= (UX_RX_DCD_CFIFOSEL_CURPIPE_MASK | UX_RX_DCD_CFIFOSEL_ISEL);
            
            }
            while ((rx_register & (UX_RX_DCD_CFIFOSEL_CURPIPE_MASK | UX_RX_DCD_CFIFOSEL_ISEL)) != rx_register_read_back);
            

            break;

        default                     :
            break;
        
    }
    
    return;
}

