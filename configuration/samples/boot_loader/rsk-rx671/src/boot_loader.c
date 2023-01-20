/**********************************************************************************************************************
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
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name     : boot_loader.c
 * Description   : Main program for Bootloader.
   NOTE:THIS IS A TYPICAL EXAMPLE.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *           16.02.2021 1.00    First Release
 *           19.05.2021 1.01    Added support for RX72N,RX66T,RX130
 *           08.07.2021 1.02    Added support for GCC
 *           10.08.2021 1.03    Added support for IAR
*********************************************************************************************************************/
#include <stdio.h>
#include "platform.h"
#include "r_fwup_if.h"

/******************************************************************************
 * Function Name: main
 *****************************************************************************/
void main(void)
{
    int32_t  result_secure_boot;
    uint32_t module_version;
    R_BSP_NOP();

    if (R_FWUP_Open() != FWUP_SUCCESS)
    {
        while (1)
        {
            R_BSP_NOP(); /* Open error. infinity loop */
        }
    }

    while (1)
    {
        result_secure_boot = R_FWUP_SecureBoot();

        if (FWUP_SUCCESS == result_secure_boot)
        {
            R_FWUP_ExecuteFirmware();
            while (1)
            {
                R_BSP_NOP(); /* infinity loop */
            }
        }
        else if (FWUP_FAIL == result_secure_boot)
        {
            while (1)
            {
                R_BSP_NOP(); /* infinity loop */
            }
        }
        else if (FWUP_END_OF_LIFE == result_secure_boot)
        {
            while (1)
            {
                R_BSP_NOP(); /* infinity loop */
            }
        }
        else if (FWUP_IN_PROGRESS == result_secure_boot)
        {
            R_BSP_NOP(); /* Continue Secure_Boot processing */
        }
        else
        {
            while (1)
            {
                R_BSP_NOP(); /* infinity loop */
            }
        }
    }
}
/******************************************************************************
 End of function main
 *****************************************************************************/
