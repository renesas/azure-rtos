/************************************************************************//**
 *
 * @file
 * @version Preview
 * SPDX-License-Identifier: MIT
 * @copyright Copyright (c) Microsoft Corporation. All rights reserved.
 * @brief The Embedded Wireless Framework configuration template.
 * Copy this file into your own project, remove the .template extension
 * and customize its contents.
 *
 ****************************************************************************/


#ifndef __EWF__config__h__included__
#define __EWF__config__h__included__

#ifdef __cplusplus
extern "C" {
#endif

#include "demo_printf.h"

/************************************************************************//**
 * @defgroup group_configuration EWF configuration
 * @brief The definitions to configure the Embedded Wireless Framework.
 * @{
 ****************************************************************************/

 /** Define this symbol to enable compilation of debug code. When this symbol is not defined, debug code is not present and the footprint is reduced.  */
#define USE_EWF_DEBUG                          0
#if USE_EWF_DEBUG
#define EWF_DEBUG
#endif

/** Define this symbol to enable verbose logging */
#define USE_EWF_LOG_VERBOSE                    0
#if USE_EWF_LOG_VERBOSE
#define EWF_LOG_VERBOSE
#endif

/** Define this symbol to enable checking of function parameters. When this symbol is not defined, parameter checking code is not present and the footprint is reduced.  */
#define USE_EWF_PARAMETER_CHECKING              0
#if USE_EWF_PARAMETER_CHECKING
#define EWF_PARAMETER_CHECKING
#endif

/* Define the platform to be used, only one is valid */
#define EWF_PLATFORM_THREADX


#ifdef EWF_DEBUG
#define EWF_LOG       demo_printf
#else
#define EWF_LOG(...)
#endif

/** Define enable EWF usage with Azure RTOS NETX */
#define EWF_CONFIG_AZURE_RTOS_NETX             0

/************************************************************************//**
 * @} *** group_configuration
 ****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* __EWF__config__h__included__ */
