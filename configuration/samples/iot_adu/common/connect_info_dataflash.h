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
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/* C runtime includes. */
#include <stdio.h>
#include <string.h>

/* for using FIT Module */
#include "platform.h"
#include "r_flash_rx_if.h"

/* Client credential information includes */
#include "sample_config.h"

#define DATA_FLASH_CONNECT_INFO_ADDRESS		(FLASH_DF_BLOCK_0)
#define DATA_FLASH_CONNECT_INFO_MAX_SIZE	(4096)

#define HOST_NAME_MAX_LENGTH				(256)
#define DEVICE_ID_MAX_LENGTH				(256)
#define DEVICE_SYMMETRIC_KEY_MAX_LENGTH		(256)
#define WIFI_SSID_MAX_LENGTH				(32)
#define WIFI_PASSWORD_MAX_LENGTH			(64)

typedef enum connect_info
{
	CONNECT_INFO_HOST_NAME = 0,
	CONNECT_INFO_DEVICE_ID,
	CONNECT_INFO_DEVICE_SYMMETRIC_KEY,
	CONNECT_INFO_WIFI_SSID,
	CONNECT_INFO_WIFI_PASSWORD,
	CONNECT_INFO_MAX_NUM
} connect_info_t;

bool connect_info_write_to_dataflash (void);
bool connect_info_read_from_dataflash (connect_info_t connect_info, uint8_t ** pdata, uint32_t * data_length);
