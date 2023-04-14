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


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** NetX Component                                                        */
/**                                                                       */
/**   Callback functions for the Renesas RX FIT driver.                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#include <stdio.h>

#include "nx_driver_rx_fit_callback.h"


#define CALLBACK_INFO_ALIGN_SIZE ((sizeof(ether_cb_arg_t) + sizeof(ALIGN_TYPE) - 1) / sizeof(ALIGN_TYPE) * sizeof(ALIGN_TYPE))
#define CALLBACK_INFO_MAX_MESSAGE (3)


TX_THREAD callback_LinkProcess_thread;
UCHAR callback_LinkProcess_thread_stack[1024];
TX_BLOCK_POOL callback_LinkProcess_block_pool;
UCHAR callback_LinkProcess_memory_area[(CALLBACK_INFO_ALIGN_SIZE + sizeof(ether_cb_arg_t*)) * CALLBACK_INFO_MAX_MESSAGE];
TX_QUEUE callback_LinkProcess_queue;
ULONG callback_LinkProcess_queue_pool[TX_1_ULONG * CALLBACK_INFO_MAX_MESSAGE];


UINT _initialize_resources_for_callback_LinkProcess(UINT priority)
{
    UINT status;

    /* Create a block memory pool from which to allocate callback info. */
    status = tx_block_pool_create(&callback_LinkProcess_block_pool,
        "LinkProcess callback block pool", 
        sizeof(ether_cb_arg_t), 
        callback_LinkProcess_memory_area, 
        sizeof(callback_LinkProcess_memory_area));
    if(status != TX_SUCCESS)
    {
        return status;
    }
    
    status = tx_queue_create(
        &callback_LinkProcess_queue, 
        "LinkProcess callback queue",
        TX_1_ULONG,
        (VOID *)callback_LinkProcess_queue_pool,
        sizeof(callback_LinkProcess_queue_pool));
    if(status != TX_SUCCESS)
    {
        return status;
    }
    
    status = tx_thread_create(
        &callback_LinkProcess_thread, 
        "LinkProcess callback thread", 
        _callback_LinkProcess_entry, 
        (ULONG)TX_NULL,
        (VOID *)callback_LinkProcess_thread_stack, 
        sizeof(callback_LinkProcess_thread_stack), 
        priority, 
        priority, 
        TX_NO_TIME_SLICE, 
        TX_AUTO_START);
    if(status != TX_SUCCESS)
    {
        return status;
    }

    return status;
}


UINT _uninitialize_resources_for_callback_LinkProcess(VOID)
{
    UINT status;

    status = tx_thread_delete(&callback_LinkProcess_thread);
    if(status != TX_SUCCESS)
    {
        return status;
    }

    status = tx_queue_delete(&callback_LinkProcess_queue);
    if(status != TX_SUCCESS)
    {
        return status;
    }

    /* Delete a block memory pool from which to allocate callback info.  */
    status = tx_block_pool_delete(&callback_LinkProcess_block_pool);
    if(status != TX_SUCCESS)
    {
        return status;
    }

    return status;

}


VOID _callback_LinkProcess_entry(ULONG thread_input)
{
    UINT status;
    ULONG message;
    ether_cb_arg_t *ether_cb_arg_ptr;

    (VOID)thread_input;

    while(1)
    {
        /* Retrieve a message from the queue. */
        status = tx_queue_receive(&callback_LinkProcess_queue, &message, TX_WAIT_FOREVER);
        if(status == TX_SUCCESS)
        {
            ether_cb_arg_ptr = (ether_cb_arg_t *)message;
            switch (ether_cb_arg_ptr->event_id)
            {
                /* Callback function that notifies user to have detected magic packet. */
                case ETHER_CB_EVENT_ID_WAKEON_LAN:
                    /* Not supported. */
                    break;

                /* Callback function that notifies user to have become Link up. */
                case ETHER_CB_EVENT_ID_LINK_ON:
                    _nx_driver_user_callback_link_on(ether_cb_arg_ptr);
                    break;

                /* Callback function that notifies user to have become Link down. */
                case ETHER_CB_EVENT_ID_LINK_OFF:
                    _nx_driver_user_callback_link_off(ether_cb_arg_ptr);
                    break;

                default:
                    break;
            }

            /* Release a block memory. */
            status = tx_block_release((VOID *)ether_cb_arg_ptr);
            if(status != TX_SUCCESS)
            {
                /* Error trap. */
                R_BSP_NOP();
            }
        }
    }
}


VOID _nx_driver_user_callback_link_on(ether_cb_arg_t *ether_cb_arg)
{
    /* Start user code for callback invoked when link status turns on. */
    printf("ETHERC%u is link up.\r\n", ether_cb_arg->channel);
    /* End user code for callback invoked when link status turns on. */
}


VOID _nx_driver_user_callback_link_off(ether_cb_arg_t *ether_cb_arg)
{
    /*Start user code for callback invoked when link status turns off. */
    printf("ETHERC%u is link down.\r\n", ether_cb_arg->channel);
    /* End user code for callback invoked when link status turns off. */
}
