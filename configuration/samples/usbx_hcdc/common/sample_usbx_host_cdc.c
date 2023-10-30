/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_basic_config.h"

#include "r_usb_basic_pinset.h"

#include "ux_api.h"
#include "ux_system.h"
#include "ux_host_class_cdc_acm.h"

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#define USB_DEVICE_SPEED    USB_SPEED_FS
// #define USB_DEVICE_SPEED    USB_SPEED_HS

//#define DATA_LEN        (512)
#define DATA_LEN        (64)
#define MEMPOOL_SIZE    (18432 *4)
#define CDCACM_FLAG     ((ULONG)0x0001)

TX_EVENT_FLAGS_GROUP g_cdcacm_activate_event_flags0;

/***********************************************************************************************************************
    Private function prototypes
 ***********************************************************************************************************************/
static void buffer_clear(uint8_t *p);

/***********************************************************************************************************************
    Private global variables
 ***********************************************************************************************************************/
/* A pointer to store CDC-ACM instance. */
static UX_HOST_CLASS_CDC_ACM* p_cdc_acm = UX_NULL;

static uint8_t  g_read_buf[DATA_LEN];
static uint8_t  g_write_buf[DATA_LEN];

static uint8_t  g_ux_pool_memory[MEMPOOL_SIZE];

static ULONG    g_write_actual_length;
static ULONG    g_read_actual_length;

uint8_t         g_is_communicate;

void tx_application_define(void *first_unused_memory);
void usb_thread0_entry(void);

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
extern void platform_setup(void);
extern void tx_application_define_user (void);

/******************************************************************************
 Exported global variable (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Function Name   : main
 Description     : Define main entry point.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
int main(void)
{
    /* Setup the hardware. */
    platform_setup();

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
} /* End of function main() */

/******************************************************************************
 Function Name   : tx_application_define
 Description     : Application define function
 Arguments       : first_unused_memory : first available memory address
 Return value    : none
 ******************************************************************************/
void tx_application_define(void *first_unused_memory)
{
#ifdef TX_USER_ENABLE_TRACE
    TX_USER_ENABLE_TRACE;
#endif

    tx_application_define_user ();
} /* End of function tx_application_define() */

/******************************************************************************
 Function Name   : ux_host_usr_event_notification
 Description     : This is the function to notify a USB event from the USBX Host system.
 Arguments       : event      : Event code from happened to the USBX Host system.
                 : host_class : Pointer to a USBX Host class, which occurs a event.
                 : instance   : Pointer to a USBX Host class instance, which occurs a event.
 Return value    : USBX error code
 ******************************************************************************/
UINT ux_host_usr_event_notification(ULONG event, UX_HOST_CLASS * host_class, VOID * instance)
{
    if(_ux_utility_memory_compare(_ux_system_host_class_cdc_acm_name, host_class,
                                   _ux_utility_string_length_get(_ux_system_host_class_cdc_acm_name))
                                   ==UX_SUCCESS)
    {
        if(event==UX_DEVICE_INSERTION) /* Check if there is a device insertion. */
        {
            p_cdc_acm = (UX_HOST_CLASS_CDC_ACM*)instance;
            if(p_cdc_acm->ux_host_class_cdc_acm_interface->ux_interface_descriptor.bInterfaceClass!=
                    UX_HOST_CLASS_CDC_DATA_CLASS)
            {
                if (UX_NULL != p_cdc_acm->ux_host_class_cdc_acm_next_instance)
                {
                    /* It seems the DATA class is on the second interface. Or we hope !  */
                    p_cdc_acm = p_cdc_acm->ux_host_class_cdc_acm_next_instance;

                    /* Check again this interface, if this is not the data interface, we give up.  */
                    if(p_cdc_acm->ux_host_class_cdc_acm_interface->ux_interface_descriptor.bInterfaceClass!=
                            UX_HOST_CLASS_CDC_DATA_CLASS)
                    {
                        /* We did not find a proper data interface. */
                        p_cdc_acm = UX_NULL;
                        while (1);
                    }
                }
            }
            if(p_cdc_acm!=UX_NULL)
            {
                tx_event_flags_set(&g_cdcacm_activate_event_flags0, CDCACM_FLAG, TX_OR);
            }

        }
        else if(event==UX_DEVICE_REMOVAL) /* Check if there is a device removal. */
        {
            tx_event_flags_set(&g_cdcacm_activate_event_flags0, ~CDCACM_FLAG, TX_AND);
            p_cdc_acm = UX_NULL;
        }
    }
    return UX_SUCCESS;
}

void buffer_clear(uint8_t *p)
{
    uint16_t    counter;
    for (counter = 0; counter < DATA_LEN; counter++)
    {
        *p = 0U;
    }
}

/******************************************************************************
 Function Name   : usb_thread0_entry
 Description     : CDCACM Host Thread entry function
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_thread0_entry(void)
{
    ULONG       status;
    ULONG       actual_flags;
    uint16_t    counter = 0;
    usb_ctrl_t                  ctrl;
    usb_cfg_t                   cfg;

    for (counter = 0; counter < DATA_LEN; counter++)
    {
        g_write_buf[counter] = (uint8_t)counter;
    }

    ux_system_initialize ((CHAR *) g_ux_pool_memory, MEMPOOL_SIZE, UX_NULL, 0);

    ux_host_stack_initialize (ux_host_usr_event_notification);

    /* Register the USB device controllers available in this system */
    R_USB_PinSet_USB0_HOST();

    ctrl.module     = USB_IP0;
    ctrl.type       = USB_HCDC;
    cfg.usb_speed   = USB_FS;       /* USB_HS/USB_FS */
    cfg.usb_mode    = USB_HOST;
    R_USB_Open(&ctrl, &cfg);

    while (1)
    {
        status = tx_event_flags_get(&g_cdcacm_activate_event_flags0, CDCACM_FLAG,  TX_OR, &actual_flags,  TX_WAIT_FOREVER);
        if(p_cdc_acm != UX_NULL)
        {
            if (0 == g_is_communicate)
            {
                tx_thread_sleep(100);
                g_is_communicate = 1;
            }

            if (UX_NULL != p_cdc_acm)
            {
            status = ux_host_class_cdc_acm_write(p_cdc_acm, g_write_buf, DATA_LEN, &g_write_actual_length);
            if ((UX_SUCCESS == status) && (DATA_LEN == g_write_actual_length))
            {
                g_read_actual_length = 0;
                buffer_clear(g_read_buf);
                if (UX_NULL != p_cdc_acm)
                {
                status = ux_host_class_cdc_acm_read (p_cdc_acm, g_read_buf, DATA_LEN, &g_read_actual_length);
                if ((UX_SUCCESS == status) && (DATA_LEN == g_read_actual_length))
                {
                    for (counter = 0; counter < DATA_LEN; counter++)
                    {
                        if ((uint8_t)counter != g_read_buf[counter])
                        {
                            while(1);
                        }
                    }
                }
                }
            }
            }
        }
    }
}
