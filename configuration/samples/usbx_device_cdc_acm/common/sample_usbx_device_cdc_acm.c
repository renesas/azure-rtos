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

#include "ux_api.h"
#include "ux_system.h"
#include "ux_utility.h"
#include "ux_dcd_rx.h"
#include "ux_device_class_cdc_acm.h"
#include "ux_device_stack.h"
#include "platform.h"


#define SAMPLE_DEBUG_SIZE                  (4096*8)
#define SAMPLE_STACK_SIZE                  1024
#define SAMPLE_BUFFER_SIZE                 512
#define USBX_MEMORY_SIZE                 (32 * 1024)

#define DEVICE_FRAMEWORK_LENGTH_FULL_SPEED      93
#define DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED      103
#define STRING_FRAMEWORK_LENGTH                 47
#define LANGUAGE_ID_FRAMEWORK_LENGTH            2

/* Define local prototypes and definitions.  */
void                                        sample_thread_entry(ULONG arg);
VOID                                        sample_cdc_instance_activate(VOID  *cdc_instance);
VOID                                        sample_cdc_instance_deactivate(VOID *cdc_instance);

/* Define local variables.  */
UX_SLAVE_CLASS_CDC_ACM                      *cdc;
TX_THREAD                                   sample_thread;

UX_SLAVE_CLASS_CDC_ACM_PARAMETER            parameter;
unsigned char                               buffer[SAMPLE_BUFFER_SIZE];

static UCHAR usbx_memory[USBX_MEMORY_SIZE];
static UCHAR sample_thread_stack[SAMPLE_STACK_SIZE];


unsigned char device_framework_full_speed[] = {

    /* Device descriptor     18 bytes
       0x02 bDeviceClass:    CDC class code
       0x00 bDeviceSubclass: CDC class sub code
       0x00 bDeviceProtocol: CDC Device protocol

       idVendor & idProduct - http://www.linux-usb.org/usb.ids
    */
    0x12, 0x01, 0x10, 0x01,
    0xEF, 0x02, 0x01,
    0x40,
    0x84, 0x84, 0x00, 0x00,
    0x00, 0x01,
    0x01, 0x02, 03,
    0x01,

    /* Configuration 1 descriptor 9 bytes */
    0x09, 0x02, 0x4b, 0x00,
    0x02, 0x01, 0x00,
    0x40, 0x00,

    /* Interface association descriptor. 8 bytes.  */
    0x08, 0x0b, 0x00, 0x02, 0x02, 0x02, 0x00, 0x00,

    /* Communication Class Interface Descriptor Requirement. 9 bytes.   */
    0x09, 0x04, 0x00,
    0x00,
    0x01,
    0x02, 0x02, 0x01,
    0x00,

    /* Header Functional Descriptor 5 bytes */
    0x05, 0x24, 0x00,
    0x10, 0x01,

    /* ACM Functional Descriptor 4 bytes */
    0x04, 0x24, 0x02,
    0x0f,

    /* Union Functional Descriptor 5 bytes */
    0x05, 0x24, 0x06,
    0x00,                          /* Master interface */
    0x01,                          /* Slave interface  */

    /* Call Management Functional Descriptor 5 bytes */
    0x05, 0x24, 0x01,
    0x03,
    0x01,                          /* Data interface   */

    /* Endpoint 1 descriptor 7 bytes */
    0x07, 0x05, 0x83,
    0x03,
    0x08, 0x00,
    0xFF,

    /* Data Class Interface Descriptor Requirement 9 bytes */
    0x09, 0x04, 0x01,
    0x00,
    0x02,
    0x0A, 0x00, 0x00,
    0x00,

    /* First alternate setting Endpoint 1 descriptor 7 bytes*/
    0x07, 0x05, 0x02,
    0x02,
    0x40, 0x00,
    0x00,

    /* Endpoint 2 descriptor 7 bytes */
    0x07, 0x05, 0x81,
    0x02,
    0x40, 0x00,
    0x00,

};

unsigned char device_framework_high_speed[] = {

    /* Device descriptor
       0x02 bDeviceClass:    CDC class code
       0x00 bDeviceSubclass: CDC class sub code
       0x00 bDeviceProtocol: CDC Device protocol

       idVendor & idProduct - http://www.linux-usb.org/usb.ids
    */
    0x12, 0x01, 0x00, 0x02,
    0xEF, 0x02, 0x01,
    0x40,
    0x84, 0x84, 0x00, 0x00,
    0x00, 0x01,
    0x01, 0x02, 03,
    0x01,

    /* Device qualifier descriptor */
    0x0a, 0x06, 0x00, 0x02,
    0x02, 0x00, 0x00,
    0x40,
    0x01,
    0x00,

    /* Configuration 1 descriptor */
    0x09, 0x02, 0x4b, 0x00,
    0x02, 0x01, 0x00,
    0x40, 0x00,

    /* Interface association descriptor. */
    0x08, 0x0b, 0x00, 0x02, 0x02, 0x02, 0x00, 0x00,

    /* Communication Class Interface Descriptor Requirement */
    0x09, 0x04, 0x00,
    0x00,
    0x01,
    0x02, 0x02, 0x01,
    0x00,

    /* Header Functional Descriptor */
    0x05, 0x24, 0x00,
    0x10, 0x01,

    /* ACM Functional Descriptor */
    0x04, 0x24, 0x02,
    0x0f,

    /* Union Functional Descriptor */
    0x05, 0x24, 0x06,
    0x00,
    0x01,

    /* Call Management Functional Descriptor */
    0x05, 0x24, 0x01,
    0x00,
    0x01,

    /* Endpoint 1 descriptor */
    0x07, 0x05, 0x83,
    0x03,
    0x08, 0x00,
    0xFF,

    /* Data Class Interface Descriptor Requirement */
    0x09, 0x04, 0x01,
    0x00,
    0x02,
    0x0A, 0x00, 0x00,
    0x00,

    /* First alternate setting Endpoint 1 descriptor */
    0x07, 0x05, 0x02,
    0x02,
    0x00, 0x02,
    0x00,

    /* Endpoint 2 descriptor */
    0x07, 0x05, 0x81,
    0x02,
    0x00, 0x02,
    0x00,

};

unsigned char string_framework[] = {

    /* Manufacturer string descriptor : Index 1  */
        0x09, 0x04, 0x01, 0x0c,
        0x45, 0x78, 0x70, 0x72,0x65, 0x73, 0x20, 0x4c,
        0x6f, 0x67, 0x69, 0x63,

    /* Product string descriptor : Index 2  */
        0x09, 0x04, 0x02, 0x13,
        0x45, 0x4c, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x6f,
        0x73, 0x69, 0x74, 0x65, 0x20, 0x64, 0x65, 0x76,
        0x69, 0x63, 0x65,

    /* Serial Number string descriptor : Index 3  */
        0x09, 0x04, 0x03, 0x04,
        0x30, 0x30, 0x30, 0x31
    };


    /* Multiple languages are supported on the device, to add
       a language besides english, the unicode language code must
       be appended to the language_id_framework array and the length
       adjusted accordingly. */
unsigned char language_id_framework[] = {

    /* English. */
        0x09, 0x04
    };



VOID platform_setup(void);

/* Define main entry point.  */
int main(void)
{   
    /* Setup the hardware. */
    platform_setup();
    
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */
void    tx_application_define(void *first_unused_memory)
{


UINT                    status;


	/* Initialize USBX Memory */
	ux_system_initialize(usbx_memory, USBX_MEMORY_SIZE, UX_NULL,0);

	/* The code below is required for installing the device portion of USBX. No call back for
	   device status change in this example. */
	status =  ux_device_stack_initialize(device_framework_high_speed, DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED,
									   device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED,
									   string_framework, STRING_FRAMEWORK_LENGTH,
									   language_id_framework, LANGUAGE_ID_FRAMEWORK_LENGTH,UX_NULL);
	if(status != UX_SUCCESS)
		return;

	/* Set the parameters for callback when insertion/extraction of a CDC device.  */
	parameter.ux_slave_class_cdc_acm_instance_activate   =  sample_cdc_instance_activate;
	parameter.ux_slave_class_cdc_acm_instance_deactivate =  sample_cdc_instance_deactivate;

	/* Initialize the device CDC class. This class owns both interfaces starting with 0. */
	status =  ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name, ux_device_class_cdc_acm_entry,
												1,0,  &parameter);

	if(status != UX_SUCCESS)
		return;

	/* Create the main sample thread.  */
	tx_thread_create(&sample_thread, "USBX sample", sample_thread_entry, 0,
			sample_thread_stack, SAMPLE_STACK_SIZE,
			20, 20, 1, TX_AUTO_START);

}

VOID    sample_cdc_instance_activate(VOID *cdc_instance)
{

    /* Save the CDC instance.  */
    cdc = (UX_SLAVE_CLASS_CDC_ACM *) cdc_instance;
}
VOID    sample_cdc_instance_deactivate(VOID *cdc_instance)
{

    /* Reset the CDC instance.  */
    cdc = UX_NULL;
}

void    sample_thread_entry(ULONG arg)
{
ULONG   status;
ULONG   actual_length;
ULONG   requested_length;


    /* Register the USB device controllers available in this system */
    status =  _ux_dcd_rx_initialize(0);

    if (status != UX_SUCCESS)
        return;

    while(1)
    {

        /* Ensure the CDC class is mounted.  */
        while(cdc != UX_NULL)
        {

            /* Read from the CDC class.  */
            status = ux_device_class_cdc_acm_read(cdc, buffer, SAMPLE_BUFFER_SIZE, &actual_length);

            if (status != UX_SUCCESS)
                break;

            /* The actual length becomes the requested length.  */
            requested_length = actual_length;

            /* Check the status.  If OK, we will write to the CDC instance.  */
            status = ux_device_class_cdc_acm_write(cdc, buffer, requested_length, &actual_length);

            if (status != UX_SUCCESS)
                break;

            /* Check for CR/LF.  */
            if (buffer[requested_length - 1] == '\r')
            {

                /* Copy LF value into user buffer.  */
                ux_utility_memory_copy(buffer, "\n",  1);

                /* And send it again.  */
                status = ux_device_class_cdc_acm_write(cdc, buffer, 1, &actual_length);

                if (status != UX_SUCCESS)
                    break;
            }

            /* Fill buffer.  */
            buffer[0] = 'a';
            buffer[1] = 'b';
            buffer[2] = 'c';
            buffer[3] = 'd';
            buffer[4] = 'e';
            buffer[5] = 'f';
            buffer[6] = '\r';
            buffer[7] = '\n';

            /* And send 8 bytes.  */
            status = ux_device_class_cdc_acm_write(cdc, buffer, 8, &actual_length);

            if (status != UX_SUCCESS)
                break;

            /* And send 0 byte packet. Forced ZLP.  */
            status = ux_device_class_cdc_acm_write(cdc, buffer, 0, &actual_length);

            if (status != UX_SUCCESS)
                break;
        }
    }
}


VOID  _ux_dcd_rx_interrupt_handler(VOID);

R_BSP_PRAGMA_STATIC_INTERRUPT (usb0_isr,VECT(PERIB, INTB185))	/* INTB185 is pre-assinged for USBI0 in r_bsp */
R_BSP_ATTRIB_STATIC_INTERRUPT void usb0_isr (void)
{
	_ux_dcd_rx_interrupt_handler();
}
