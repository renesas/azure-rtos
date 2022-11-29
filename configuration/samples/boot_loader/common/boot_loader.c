/***********************************************************************
*
*  FILE        : boot_loader.c
*  DATE        : 2019-05-18
*  DESCRIPTION : Main Program
*
*  NOTE:THIS IS A TYPICAL EXAMPLE.
*
***********************************************************************/
#include <stdio.h>
#include <string.h>
#include "r_smc_entry.h"
#include "r_flash_rx_if.h"
#include "r_sci_rx_if.h"
#include "r_sys_time_rx_if.h"
#include "r_fwup_if.h"

#include "r_sci_rx_pinset.h"

void main(void)
{
    int32_t result_secure_boot;
    nop();

    if (FWUP_SUCCESS != R_FWUP_Open())
    {
        while(1)
        {
            /* Open error. infinity loop */
        }
    }

    while(1)
    {
		result_secure_boot = R_FWUP_SecureBoot();
		if (FWUP_SUCCESS == result_secure_boot)
		{
			R_FWUP_ExecuteFirmware();
			while(1); /* infinite loop */
		}
		else if (FWUP_FAIL == result_secure_boot)
		{
			while(1)
			{
				/* infinity loop */
			}
		}
		else if (FWUP_END_OF_LIFE == result_secure_boot)
		{
			while(1)
			{
				/* infinity loop */
			}
		}
		else if (FWUP_IN_PROGRESS == result_secure_boot)
		{
			continue;
		}
		else
		{
			while(1)
			{
				/* infinite loop */
			}
		}
    }
}
