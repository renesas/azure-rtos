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

#ifndef NX_DRIVER_RX_FIT_CALLBACK_H
#define NX_DRIVER_RX_FIT_CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <tx_api.h>

#include <r_ether_rx_if.h>


extern TX_THREAD callback_LinkProcess_thread;
extern TX_BLOCK_POOL callback_LinkProcess_block_pool;
extern TX_QUEUE callback_LinkProcess_queue;
extern ether_cb_arg_t tmp_ether_cb_arg;


UINT _initialize_resources_for_callback_LinkProcess(UINT priority);
UINT _uninitialize_resources_for_callback_LinkProcess(VOID);
VOID _callback_LinkProcess_entry(ULONG thread_input);

/* Callback, called when changing link status, that can be coded by users. */
VOID _nx_driver_user_callback_link_on(ether_cb_arg_t *ether_cb_arg);
VOID _nx_driver_user_callback_link_off(ether_cb_arg_t *ether_cb_arg);


#ifdef __cplusplus
}
#endif

#endif // #ifndef NX_DRIVER_RX_FIT_CALLBACK_H

