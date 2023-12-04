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

#include <stdio.h>
#include <stdint.h>
#include "nx_azure_iot_adu_agent.h"
#include "r_flash_rx_if.h"
#include "r_fwup_if.h"
#include "r_bsp_config.h"
#include "demo_printf.h"

#if (BSP_CFG_CODE_FLASH_BANK_MODE != 0)
#error "Error! Please set a device as dual bank device on the Smart Configurator."
#endif


#if (BSP_CFG_RTOS_USED != 5)
#error "Error! Need to define Azure RTOS in BSP configuration"
#endif

void nx_azure_iot_adu_agent_driver (NX_AZURE_IOT_ADU_AGENT_DRIVER * driver_req_ptr);

/****** DRIVER SPECIFIC ******/
void nx_azure_iot_adu_agent_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER * driver_req_ptr)
{

    /* Default to successful return.  */
    driver_req_ptr->nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_SUCCESS;
        
    /* Process according to the driver request type.  */
    switch (driver_req_ptr -> nx_azure_iot_adu_agent_driver_command)
    {
        
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_INITIALIZE:
        {

            /* Process initialize requests.  */
            if (R_FWUP_Open() != FWUP_SUCCESS)
            {
                driver_req_ptr->nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
            }

            break;
        }
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_PREPROCESS:
        {
            /* Process firmware preprocess requests before writing firmware. */
            /* Do nothing */

            break;
        }
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_WRITE:
        {
            static uint8_t s_fragmented_data[FLASH_CF_MIN_PGM_SIZE];
            static uint8_t s_fragmented_data_size = 0;

            uint32_t current_data_offset = 0;
            uint32_t remaining_bytes     = driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_size;
            uint32_t blockcounts;

            e_fwup_err_t  fwup_err;

            /* Writing firmware. */
            if (s_fragmented_data_size)
            {
                if ((s_fragmented_data_size + remaining_bytes) >= FLASH_CF_MIN_PGM_SIZE)
                {
                    memcpy(&s_fragmented_data[s_fragmented_data_size],
                            driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_ptr,
                            FLASH_CF_MIN_PGM_SIZE - s_fragmented_data_size);

                    fwup_err = R_FWUP_WriteImage (FWUP_AREA_BUFFER, s_fragmented_data, FLASH_CF_MIN_PGM_SIZE);
                    if (FWUP_ERR_FLASH == fwup_err)
                    {
                        driver_req_ptr->nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
                        break;
                    }
                    else if (FWUP_SUCCESS == fwup_err)
                    {
                        break;
                    }
                    else
                    {
                        current_data_offset += (FLASH_CF_MIN_PGM_SIZE - s_fragmented_data_size);
                        remaining_bytes     -= (FLASH_CF_MIN_PGM_SIZE - s_fragmented_data_size);
                    }

                }
                else
                {
                    memcpy(&s_fragmented_data[s_fragmented_data_size],
                            driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_ptr,
                            remaining_bytes);
                    s_fragmented_data_size += remaining_bytes;
                    break;
                }

            }

            if (remaining_bytes >= FLASH_CF_MIN_PGM_SIZE)
            {
                blockcounts = remaining_bytes / FLASH_CF_MIN_PGM_SIZE;
                fwup_err    = R_FWUP_WriteImage(FWUP_AREA_BUFFER,
                        driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_ptr+current_data_offset,
                        (FLASH_CF_MIN_PGM_SIZE * blockcounts));
                if (FWUP_ERR_FLASH == fwup_err)
                {
                    driver_req_ptr->nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
                    break;
                }
                else if (FWUP_SUCCESS == fwup_err)
                {
                    break;
                }
                else
                {
                    current_data_offset += (FLASH_CF_MIN_PGM_SIZE * blockcounts);
                    remaining_bytes     -= (FLASH_CF_MIN_PGM_SIZE * blockcounts);
                }

            }

            /* store the g_remaining_bytes into fragmented_data for next receiving. */
            if (remaining_bytes)
            {
                memset(s_fragmented_data, 0, sizeof(s_fragmented_data));
                memcpy(s_fragmented_data,
                        driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_ptr+current_data_offset,
                        remaining_bytes);
            }
            s_fragmented_data_size = remaining_bytes;

            break;
        }
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_INSTALL:
        {

            /* Verify the new firmware.  */
            LOG_TERMINAL("Verifying the downloaded firmware has started...\r\n");

            if (R_FWUP_VerifyImage(FWUP_AREA_BUFFER) != FWUP_SUCCESS)
            {
                LOG_TERMINAL("Fail verify\r\n");
                if (R_FWUP_EraseArea(FWUP_AREA_BUFFER) != FWUP_SUCCESS)
                {
                    LOG_TERMINAL("Fail to Erase Downloaded firmware. \r\n");
                }
                driver_req_ptr->nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
            }
            else
            {
                LOG_TERMINAL("Successful validation\r\n");
            }

            break;
        } 
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_APPLY:
        {

            /* Apply the new firmware, and reboot device.*/
            LOG_TERMINAL("Bankswap and Reset ...\r\n");

            if (R_FWUP_ActivateImage() != FWUP_SUCCESS)
            {
                LOG_TERMINAL("Fail Active Image. \r\n");
                driver_req_ptr->nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
            }
            else
            {
                R_FWUP_SoftwareReset();
            }

            break;
        } 
        default:
        {
            /* Invalid driver request.  */
            /* This driver does not support NX_AZURE_IOT_ADU_AGENT_DRIVER_UPDATE_CHECK and
             * NX_AZURE_IOT_ADU_AGENT_DRIVER_CANCEL command.*/
            /* Fail return.  */
            driver_req_ptr->nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
        }
    }
}
