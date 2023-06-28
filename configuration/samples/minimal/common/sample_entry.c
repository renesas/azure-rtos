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
/***********************************************************************************************************************
 * File Name    : thread_x_entry.c
 * Version      : 1.0
 * Description  : declare Thread entry function  
 **********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "azurertos_object_init.h"
#include "platform.h"
#include "r_gpio_rx_if.h"
#include "demo_printf.h"
#include "board_led.h"

/* New Thread entry function */
void sample_entry(ULONG entry_input)
{

    UINT log_index = 0;

    /* LED Initialize */

    /* Set Low to LED pin  */
    R_GPIO_PinWrite (LED_PIN, LED_OFF);

    /* Set LED Output Direction */
    R_GPIO_PinDirectionSet (LED_PIN, GPIO_DIRECTION_OUTPUT);

    /* Initialize Log output. */
    LogPrintInit ();

    while (1)
    {
        /* Output log*/
        LogPrint ("Hello, RX AzureRTOS sample. [%d]\r\n", log_index++);

        /* LED toggle */
        if (R_GPIO_PinRead (LED_PIN) != LED_ON)
        {
            /* LED ON */
            R_GPIO_PinWrite (LED_PIN, LED_ON);
        }
        else
        {
            /* LED OFF */
            R_GPIO_PinWrite (LED_PIN, LED_OFF);
        }

        /* Sleep 1 second */
        tx_thread_sleep (100);
    }
}

