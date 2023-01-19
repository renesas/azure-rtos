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

#include "connect_info_dataflash.h"

#if (defined(__CCRX__) || defined(__GNUC__))
R_BSP_ATTRIB_SECTION_CHANGE(D, _CONNECT_INFO , 1)
#ifndef ENABLE_DPS_SAMPLE
volatile uint8_t df_host_name[HOST_NAME_MAX_LENGTH + 1] = HOST_NAME;
R_BSP_ATTRIB_SECTION_CHANGE(D, _CONNECT_INFO , 1)
volatile uint8_t df_device_id[DEVICE_ID_MAX_LENGTH + 1] = DEVICE_ID;
#endif
R_BSP_ATTRIB_SECTION_CHANGE(D, _CONNECT_INFO , 1)
volatile uint8_t df_device_symmetric_key[DEVICE_SYMMETRIC_KEY_MAX_LENGTH + 1] = DEVICE_SYMMETRIC_KEY;
#ifndef ENABLE_WIFI
R_BSP_ATTRIB_SECTION_CHANGE(D, _CONNECT_INFO , 1)
volatile uint8_t df__netx_driver_rx_fit_mac_address[6] = {0x00,0x04,0x0,0x00,0x00,0x00};
#endif
R_BSP_ATTRIB_SECTION_CHANGE_END
#else
volatile const uint8_t df_host_name[HOST_NAME_MAX_LENGTH + 1] = "dummy";
volatile const uint8_t df_device_id[DEVICE_ID_MAX_LENGTH + 1] = "dummy";
volatile const uint8_t df_device_symmetric_key[DEVICE_SYMMETRIC_KEY_MAX_LENGTH + 1] = "dummy";
volatile const uint8_t df__netx_driver_rx_fit_mac_address[6] = {0x00,0x04,0x0,0x00,0x00,0x00};
#endif

void connect_info_flash_callback_function(void *event);

bool flash_operation_done = false;

uint32_t connect_info_length[CONNECT_INFO_MAX_NUM] =
{
#ifndef ENABLE_DPS_SAMPLE
	sizeof(HOST_NAME),
	sizeof(DEVICE_ID),
#endif
	sizeof(DEVICE_SYMMETRIC_KEY),
};

uint8_t  * connect_info_pdata[CONNECT_INFO_MAX_NUM] =
{
#ifndef ENABLE_DPS_SAMPLE
	(uint8_t *)(HOST_NAME),
	(uint8_t *)(DEVICE_ID),
#endif
	(uint8_t *)(DEVICE_SYMMETRIC_KEY),
};

bool connect_info_write_to_dataflash (void)
{
	flash_interrupt_config_t cb_func_info;
	flash_err_t flash_ret;
	uint32_t current_data_size = 0;
	uint8_t connect_info_data[DATA_FLASH_CONNECT_INFO_MAX_SIZE] = {0};

	cb_func_info.pcallback = connect_info_flash_callback_function;
	cb_func_info.int_priority = 15;
	R_FLASH_Control(FLASH_CMD_SET_BGO_CALLBACK, (void *)&cb_func_info);

	for (int i = 0; i < CONNECT_INFO_MAX_NUM  ; i++)
	{
		memcpy(&connect_info_data[current_data_size], &connect_info_length[i], sizeof(uint32_t));
		current_data_size += sizeof(uint32_t);
		memcpy(&connect_info_data[current_data_size], connect_info_pdata[i], connect_info_length[i]);
		current_data_size += connect_info_length[i];
	}

	if (current_data_size < DATA_FLASH_CONNECT_INFO_MAX_SIZE)
	{
		uint32_t num_blocks = (current_data_size / FLASH_DF_BLOCK_SIZE);
		if (0 != (current_data_size % FLASH_DF_BLOCK_SIZE))
		{
			num_blocks++;
		}

		flash_ret = R_FLASH_Erase(DATA_FLASH_CONNECT_INFO_ADDRESS, num_blocks);
		if (FLASH_SUCCESS != flash_ret)
		{
			return false;
		}

		while (false == flash_operation_done);
		flash_operation_done = false;

		current_data_size = (current_data_size + (FLASH_DF_MIN_PGM_SIZE - (current_data_size % FLASH_DF_MIN_PGM_SIZE)));
		flash_ret = R_FLASH_Write((uint32_t)connect_info_data, DATA_FLASH_CONNECT_INFO_ADDRESS, current_data_size);
		if (FLASH_SUCCESS != flash_ret)
		{
			return false;
		}

		while (false == flash_operation_done);
		flash_operation_done = false;
	}
	else
	{
		return false;
	}

	return true;
}

bool connect_info_read_from_dataflash (connect_info_t connect_info, uint8_t ** pdata, uint32_t *data_length)
{
	uint32_t temp_length = 0;
	uint32_t current_data_address = DATA_FLASH_CONNECT_INFO_ADDRESS;

	for (int i = 0; i < CONNECT_INFO_MAX_NUM; i++)
	{
		if (connect_info == i)
		{
			memcpy(data_length, (void *)current_data_address, sizeof(uint32_t));
			(*pdata) = (uint8_t *)(current_data_address + sizeof(uint32_t));
			break;
		}
		memcpy(&temp_length, (void *)current_data_address, sizeof(uint32_t));
		current_data_address = (current_data_address + temp_length +sizeof(uint32_t));
	}

	return true;
}

void connect_info_flash_callback_function(void *event)
{
	uint32_t event_code;
	event_code = *((uint32_t*)event);

	switch(event_code)
	{
		case FLASH_INT_EVENT_ERASE_COMPLETE:
			flash_operation_done = true;
			break;
		case FLASH_INT_EVENT_WRITE_COMPLETE:
			flash_operation_done = true;
			break;
		default:
			break;
	}
}
