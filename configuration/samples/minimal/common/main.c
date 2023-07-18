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
* Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : main.c
 * Version      : 1.0
 * Description  : main
 **********************************************************************************************************************/
/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "tx_api.h"
#include "hardware_setup.h"

extern void tx_application_define_user (void);

/**********************************************************************************************************************
* Function Name: main
* Description  : Define main entry point.
* Arguments    : void
* Return Value : None
**********************************************************************************************************************/
void main(void)
{
    /* Setup the hardware. */
    platform_setup();

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}
/**********************************************************************************************************************
* End of function main(void)
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: tx_application_define
* Description  : Define what the initial system looks like.
* Arguments    : void
* Return Value : None
**********************************************************************************************************************/
void    tx_application_define(void *first_unused_memory)
{
    /* Initialize user thread */
    tx_application_define_user ();
}
/**********************************************************************************************************************
* End of function tx_application_define(void *first_unused_memory)
**********************************************************************************************************************/
