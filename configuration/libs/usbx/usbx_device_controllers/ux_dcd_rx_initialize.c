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
/*    _ux_dcd_rx_initialize                               PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the USB slave controller of the ATMEL     */
/*    rx chipset.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd                                   Address of DCD                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_dcd_rx_register_write             Read register                 */ 
/*    _ux_utility_memory_allocate           Allocate memory               */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    USBX Device Stack                                                   */ 
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
UINT  _ux_dcd_rx_initialize(ULONG dcd_io)
{

UX_SLAVE_DCD        *dcd;
UX_DCD_RX           *dcd_rx;
#if defined(UX_RX71M) && !defined(UX_RX71M_USBA_FULL_SPEED)
ULONG               retry_count = 0xfff;
#endif

    /* Get the pointer to the DCD.  */
    dcd =  &_ux_system_slave -> ux_system_slave_dcd;

    /* The controller initialized here is of rx type.  */
    dcd -> ux_slave_dcd_controller_type =  UX_DCD_RX_SLAVE_CONTROLLER;
    
    /* Allocate memory for this rx DCD instance.  */
    dcd_rx =  _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, sizeof(UX_DCD_RX));

    /* Check if memory was properly allocated.  */
    if(dcd_rx == UX_NULL)
        return(UX_MEMORY_INSUFFICIENT);

    /* Set the pointer to the rx DCD.  */
    dcd -> ux_slave_dcd_controller_hardware =  (VOID *) dcd_rx;

    /* Save the base address of the controller.  */
    dcd -> ux_slave_dcd_io   =  dcd_io;
    dcd_rx -> ux_dcd_rx_base =  dcd_io;

    /* Set the generic DCD owner for the rx DCD.  */
    dcd_rx -> ux_dcd_rx_dcd_owner =  dcd;

    /* Initialize the function collector for this DCD.  */
    dcd -> ux_slave_dcd_function =  _ux_dcd_rx_function;

    /*  Buffer number starts at 8 , till 127 */
    dcd_rx -> ux_dcd_rx_next_available_bufnum =  8;

#ifdef UX_RX71M
    /* Check if USBA is being initialized.  */
    if (UX_RX_USB_BASE == UX_RX_USBA_BASE || dcd_io == UX_RX_USBA_BASE)
    {

        /* Disable pulling down of D+/D- lines.  */
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_DRPD);

#ifdef UX_RX71M_USBA_FULL_SPEED
        /* Disable high-speed operation.  */
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_HSE);

        /* Enable Classic only mode.  */
        _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_PHYSET, UX_RX_DCD_PHYSET_HSEB);

        /* Wait for at least 1 microsecond.  */
        _ux_utility_delay_ms(1);
#else
        /* Enable high-speed operation.  */
        _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_HSE);

        /* Disable Classic only mode.  */
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_PHYSET, UX_RX_DCD_PHYSET_HSEB);
#endif

#ifndef UX_RX_VBUS_DETECTION
        /* Enable single end receiver.  */
        _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_CNEN);
#endif

        /* Power on PHY.  */
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_PHYSET, UX_RX_DCD_PHYSET_DIRPD);
        
        /* Wait for 1 millisecond.  */
        _ux_utility_delay_ms(1);

#ifndef UX_RX71M_USBA_FULL_SPEED
        /* Start PLL.  */
        _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_PHYSET, UX_RX_DCD_PHYSET_PLLRESET);
#endif
        
        /* Enable PHY clock.  */
        _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_LPSTS, UX_RX_DCD_LPSTS_SUSPENDM);
        
#ifndef UX_RX71M_USBA_FULL_SPEED
        /* Confirm PLL oscillation.  */
        while((_ux_dcd_rx_register_read(dcd_rx, UX_RX_DCD_PLLSTA) & UX_RX_DCD_PLLSTA_PLLLOCK) == 0)
        {
            retry_count --;
            if (retry_count == 0)
                return(UX_CONTROLLER_INIT_FAILED);
        }
#endif
        
        /* Setup bus access cycles.  */
        _ux_dcd_rx_register_write(dcd_rx, UX_RX_DCD_BUSWAIT, UX_RX_DCD_BUSWAIT_VALUE);    

    }

    /* Check if USBA is being initialized.  */
    if (UX_RX_USB_BASE != UX_RX_USBA_BASE && dcd_io != UX_RX_USBA_BASE)
    {
        /* Supply the clock to the USB module. */
        _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_SCKE);
    }

#else

    /* Supply the clock to the USB module. */
    _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_SCKE);
#endif
    /* Reset USB Module.  */
    _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_USBE);
    
    /* Make sure we are in Device mode.  */
    _ux_dcd_rx_register_clear(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_DCFM);

#ifndef UX_RX_VBUS_DETECTION
    /* Enable D+ pull up*/
    _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_DPRPU);
#endif

    /* USB Module Operation Enable.  */
    _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_SYSCFG, UX_RX_DCD_SYSCFG_USBE);
    
    /* Enable the required interrupts.  */
    /* Enable only the interrupts we will use: SOF, ATTCH/BCHG. */
    _ux_dcd_rx_register_set(dcd_rx, UX_RX_DCD_INTENB0, 
                                (UX_RX_DCD_INTENB0_VBSE |
                                UX_RX_DCD_INTENB0_DVSE  |
                                UX_RX_DCD_INTENB0_CTRE  |
                                UX_RX_DCD_INTENB0_BEMPE |
                                UX_RX_DCD_INTENB0_NRDYE |
                                UX_RX_DCD_INTENB0_BRDYE));

    /* Set the state of the controller to OPERATIONAL now.  */
    dcd -> ux_slave_dcd_status =  UX_DCD_STATUS_OPERATIONAL;

    /* Return successful completion.  */
    return(UX_SUCCESS);
}

