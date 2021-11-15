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


//#include <demo_printf.h>
#include "tx_api.h"


void platform_setup(void);
void guix_setup(void);
VOID start_touch_thread(VOID);

/* Define main entry point.  */
int main(void)
{   
    /* Setup the hardware. */
    platform_setup();
    
    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}


void    tx_application_define(void *first_unused_memory)
{
	guix_setup();
	start_touch_thread();

}

void my_sw_charput_function(char c)
{

}
