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

#include "sample_device_identity.h"

const UCHAR sample_device_cert_ptr[] = DEVICE_CERT;
const UINT sample_device_cert_len = sizeof(sample_device_cert_ptr);
const UCHAR sample_device_private_key_ptr[] = DEVICE_PRIVATE_KEY;
const UINT sample_device_private_key_len = sizeof(sample_device_private_key_ptr);
