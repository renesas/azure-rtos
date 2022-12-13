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

#include "nx_azure_iot_adu_agent.h"
#include <r_flash_rx_if.h>
#include <stdio.h>
#include "r_fwup_if.h"
void nx_azure_iot_adu_agent_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr);

/****** DRIVER SPECIFIC ******/
void nx_azure_iot_adu_agent_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr)
{
    /* Default to successful return.  */
    driver_req_ptr -> nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_SUCCESS;
        
    /* Process according to the driver request type.  */
    switch (driver_req_ptr -> nx_azure_iot_adu_agent_driver_command)
    {
        
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_INITIALIZE:
        {
           
            /* Process initialize requests.  */
            fwup_err_t ret;

            ret = R_FWUP_Open();

            if (FWUP_SUCCESS != ret)
            {
                driver_req_ptr -> nx_azure_iot_adu_agent_driver_status =  NX_AZURE_IOT_FAILURE;
            }

            uint32_t bank_info = 255;
            R_FLASH_Control(FLASH_CMD_BANK_GET, &bank_info);
            printf("bank info = %d. (start bank = %d)\r\n", bank_info, (bank_info ^ 0x01));
            break;
        }
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_PREPROCESS:
        {
            /* Process firmware preprocess requests before writing firmware.
               Such as: erase the flash at once to improve the speed.  */

            /* step1: erase all buffer bank */
            /* R_FLASH_Erase() arguments: specify "high address (low block number)" and process to "low address (high block number)" */
            printf("erase all buffer bank = 0x%02x...", FLASH_CF_LO_BANK_LO_ADDR);

            fwup_err_t ret;

            ret = R_FWUP_Initialize();

            if (FWUP_SUCCESS != ret)
            {
                driver_req_ptr -> nx_azure_iot_adu_agent_driver_status =  NX_AZURE_IOT_FAILURE;
            }
            printf("completed.\n");
            break;
        }
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_WRITE:
        {
#if 0
        	uint32_t pr_saddr   = (0xFFE00000 + driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_offset);
        	uint32_t pr_blocks  = (driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_size / FLASH_CF_MIN_PGM_SIZE);
        	uint32_t pr_surplus = (driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_size % FLASH_CF_MIN_PGM_SIZE);
        	uint32_t pr_eaddr   = (pr_saddr + (pr_blocks * FLASH_CF_MIN_PGM_SIZE));

            printf("requested %4d bytes writing (address from %08x to %08x (128 byte block(s) = %2d)) are completed with %3d bytes are fragmented.\n",
                    driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_size,
					pr_saddr, pr_eaddr, pr_blocks, pr_surplus);
            printf("Azure ADU library puts these arguments: \n\
                    nx_azure_iot_adu_agent_driver_firmware_size = %d,\n\
                    nx_azure_iot_adu_agent_driver_firmware_data_offset = 0x%x,\n\
                    nx_azure_iot_adu_agent_driver_firmware_data_size = %d\n",\
		            driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_size,
		            driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_offset,
		            driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_size);

#endif
            if (R_FWUP_PutFirmwareChunk(driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_offset,
                    driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_ptr,
                    driver_req_ptr->nx_azure_iot_adu_agent_driver_firmware_data_size) != FWUP_IN_PROGRESS)
            {
                driver_req_ptr -> nx_azure_iot_adu_agent_driver_status =  NX_AZURE_IOT_FAILURE;
            }
            break;
        } 
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_INSTALL:
        {

            /* Set the new firmware for next boot.  */
            /* step3: bankswap */
            printf("starting software reset...\r\n");
            break;
        } 
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_APPLY:
        {

            /* Apply the new firmware, and reboot device from that.*/
        
            /* step4: software reset */
            /* close all peripherals before software reset */
            R_FWUP_SoftwareReset();
            break;
        } 
        default:
        {
                
            /* Invalid driver request.  */

            /* Default to successful return.  */
            driver_req_ptr -> nx_azure_iot_adu_agent_driver_status =  NX_AZURE_IOT_FAILURE;
        }
    }
}
