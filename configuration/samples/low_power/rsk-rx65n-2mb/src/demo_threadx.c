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
/***********************************************************************************************************************
* File Name    : demo_threadx.c
* Device(s)    : Renesas RX Family
* Tool-Chain   : Renesas RX Standard Toolchain
* H/W Platform :
* Description  : This is a small demo of the Low Power Consumption feature supported by ThreadX kernel.
*              : It includes each example of Sleep mode, Software Standby mode and Deep Software Standby mode.
*              : One thread runs for 3 seconds and then suspend itself to transit to the each Low Power Consumption mode.
*              : TX_LOW_POWER=1 is necessary in both compiler and assembler macro definition in C/C++ project settings.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY  Version Description
*         : 04.02.2022  1.00    First Release
***********************************************************************************************************************/

#include "tx_api.h"
#include "tx_low_power.h"

#include "hardware_setup.h"

#include "platform.h"
#include "r_gpio_rx_if.h"
#include "r_irq_rx_if.h"
#include "r_lpc_rx_if.h"
#include "r_irq_rx_pinset.h"
#include "r_cmt_rx_if.h"

/* Supported Low Power Mode for devices */
#if defined(BSP_MCU_RX110) || defined(BSP_MCU_111) || defined(BSP_MCU_RX113) || defined(BSP_MCU_RX130) || \
	defined(BSP_MCU_RX140) || defined(BSP_MCU_RX230) || defined(BSP_MCU_RX231) || defined(BSP_MCU_RX23W)

	#define DEMO_LP_SLEEP		(1)
	#define DEMO_LP_DEEP_SLEEP	(1)
	#define DEMO_LP_SW_STANDBY	(1)

#elif defined(BSP_MCU_RX64M) || defined(BSP_MCU_RX65N) || defined(BSP_MCU_66N) || defined(BSP_MCU_RX671) || \
	  defined(BSP_MCU_RX71M) || defined(BSP_MCU_RX72M) || defined(BSP_MCU_RX72N)

	#define DEMO_LP_SLEEP			(1)
	#define DEMO_LP_SW_STANDBY		(1)
	#define DEMO_LP_DEEP_SW_STANDBY	(1)

#else
	#error "Unsupported MCU is specified."
#endif


/* Define User LED port, User SW IRQ and RTC support for deep software standby
 * Need to define for your board
 */

#define DEMO_BOARD_RSKRX65N2MB	(1)	/* RSKRX65N-2MB RSK board */
#if DEMO_BOARD_RSKRX65N2MB
	#define DEMO_LED_PIN	(GPIO_PORT_7_PIN_3)
	#define DEMO_IRQ_NUM	(IRQ_NUM_11)
    #define DEMO_LED_ON		(GPIO_LEVEL_LOW)
    #define DEMO_LED_OFF	(GPIO_LEVEL_HIGH)
#endif

#define DEMO_BOARD_RX65NCK	(0)	/* RX65N Cloud Kit */
#if DEMO_BOARD_RX65NCK
	#define DEMO_LED_PIN	(GPIO_PORT_B_PIN_0)
	#define DEMO_IRQ_NUM	(IRQ_NUM_1)
    #define DEMO_LED_ON		(GPIO_LEVEL_LOW)
    #define DEMO_LED_OFF	(GPIO_LEVEL_HIGH)
#endif

#define DEMO_BOARD_CKRX65N	(0)	/* RX65N Cloud Kit */
#if DEMO_BOARD_CKRX65N
	#define DEMO_LED_PIN	(GPIO_PORT_1_PIN_7)
	#define DEMO_IRQ_NUM	(IRQ_NUM_1)
    #define DEMO_LED_ON		(GPIO_LEVEL_LOW)
    #define DEMO_LED_OFF	(GPIO_LEVEL_HIGH)
#endif

#define DEMO_BOARD_RSKRX671	(0)	/* RSKRX671 RSK board */
#if DEMO_BOARD_RSKRX671
	#define DEMO_LED_PIN	(GPIO_PORT_1_PIN_7)
	#define DEMO_IRQ_NUM	(IRQ_NUM_9)
    #define DEMO_LED_ON		(GPIO_LEVEL_LOW)
    #define DEMO_LED_OFF	(GPIO_LEVEL_HIGH)
#endif

#define DEMO_BOARD_RX130TB	(0)	/* RX130 Target Board */
#if DEMO_BOARD_RX130TB
	#define DEMO_LED_PIN	(GPIO_PORT_D_PIN_6)
	#define DEMO_IRQ_NUM	(IRQ_NUM_4)
    #define DEMO_LED_ON		(GPIO_LEVEL_LOW)
    #define DEMO_LED_OFF	(GPIO_LEVEL_HIGH)
#endif

#define DEMO_BOARD_RX72NEK	(0)	/* RX72N Envision Kit */
#if DEMO_BOARD_RX72NEK
	#define DEMO_LED_PIN							(GPIO_PORT_4_PIN_0)
	#define DEMO_IRQ_NUM							(IRQ_NUM_15)
    #define DEMO_LED_ON							    (GPIO_LEVEL_LOW)
    #define DEMO_LED_OFF							(GPIO_LEVEL_HIGH)

    /* Definitions for RTC */
    #define DEMO_DEEP_SW_STANDBY_USE_RTC            (1)
    #define DEMO_RTC_RFRH                           (1)
    #define DEMO_RTC_RFRL                           (0xe847)
    #define DEMO_DEEP_SW_STANDBY_TIMEOUT_SECONDS	(30)
    #define DEMO_WAIT_FOR_COMPLETION(reg, value)	{ while ((reg) != (value)) ; }
    void Demo_Init_RTC_For_Deep_SW_Standby(void);
#endif

#if !defined(DEMO_LED_PIN) || !defined(DEMO_IRQ_NUM)
#error "Pin number for LED or IRQ number for User Switch is not defined."
#endif

/*
 * Used to specify low power mode.
 *
 * LPC_LP_SLEEP
 * LPC_LP_ALL_MODULE_STOP
 * LPC_LP_SW_STANDBY
 * LPC_LP_DEEP_SW_STANDBY
 *
 */
lpc_low_power_mode_t	g_lpc_mode;

/* Define repeat count of Low Power Mode Demo  */
#define DEMO_LOOP_TIMES		(3)

/* Define the ThreadX object control block and stack. */
#define DEMO_STACK_SIZE (256*sizeof(ULONG))		/* 1024 byte */

TX_THREAD	thread_0;
ULONG		thread_0_stack[DEMO_STACK_SIZE/sizeof(ULONG)];


/* Define thread prototypes.  */
void		thread_0_entry(ULONG thread_input);


/* Used CMT channel. Global variable in hardware_setup.c */
extern uint32_t chan;


/* Define demo forever loop when something error was returned */
#define DEMO_FOREVER_LOOP()		{ while (1);  }


/*
 * Referred for debugging.
 */
lpc_err_t	g_lpc_status;			/* Define Low Power Consumption module statue */


/* Define main entry point.  */
int main()
{
    /* Setup the hardware. */
    platform_setup();

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */
void    tx_application_define(void *first_unused_memory)
{
    /* Create the Low Power DEMO thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            thread_0_stack, DEMO_STACK_SIZE,
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}

/* Define callback of User SW */
void Demo_callback(void *pargs)
{
	tx_thread_resume(&thread_0);	/* Resume when User SW is pushed. */
}


/* Define the Low Power DEMO thread.  */
void    thread_0_entry(ULONG thread_input)
{

	irq_handle_t irq_handle;
	int n;

	#if DEMO_DEEP_SW_STANDBY_USE_RTC
	Demo_Init_RTC_For_Deep_SW_Standby();
	#endif

	/* Prepare LED & LED ON */
	R_GPIO_PinDirectionSet(DEMO_LED_PIN, GPIO_DIRECTION_OUTPUT);
	R_GPIO_PinWrite(DEMO_LED_PIN, DEMO_LED_ON);

	/* Register User SW as a IRQ */
	R_ICU_PinSet();
	R_IRQ_Open(DEMO_IRQ_NUM, IRQ_TRIG_FALLING, IRQ_PRI_4, &irq_handle, &Demo_callback);

	for ( ;; )
	{
		#if DEMO_LP_SLEEP
		/*
		 * Sleep Mode repeats DEMO_LOOP_TIMES (default 3).
		 * Resume by pushing User SW.
		 */
		g_lpc_mode = LPC_LP_SLEEP;
		for (n = 0; n < DEMO_LOOP_TIMES; n++)
		{
			R_BSP_SoftwareDelay(3, BSP_DELAY_SECS);	/* Run for 3 seconds */
			tx_thread_suspend(&thread_0);   		/* Enter to low power mode automatically */
		}
		#endif	/* DEMO_LE_SLEEP */

		#if DEMO_LP_DEEP_SLEEP
		/*
		 * Software Standby Mode repeats DEMO_LOOP_TIMES (default 3).
		 * Resume by pushing User SW.
		 */
		g_lpc_mode = LPC_LP_DEEP_SLEEP;
		for (n = 0; n < DEMO_LOOP_TIMES; n++)
		{
			R_BSP_SoftwareDelay(3, BSP_DELAY_SECS);	/* Run for 3 seconds */
			tx_thread_suspend(&thread_0);   		/* Enter to low power mode automatically */
		}
		#endif	/* DEMO_LP_DEEP_SLEEP */

		#if DEMO_LP_SW_STANDBY
		/*
		 * Software Standby Mode repeats DEMO_LOOP_TIMES (default 3).
		 * Resume by pushing User SW.
		 */
		g_lpc_mode = LPC_LP_SW_STANDBY;
		for (n = 0; n < DEMO_LOOP_TIMES; n++)
		{
			R_BSP_SoftwareDelay(3, BSP_DELAY_SECS);	/* Run for 3 seconds */
			tx_thread_suspend(&thread_0);   		/* Enter to low power mode automatically */
		}
		#endif	/* DEMO_LP_SW_STANDBY */

		#if DEMO_LP_DEEP_SW_STANDBY
		/*
		 * Finally enter to Deep Software Standby Mode.
		 * Reboot by pushing User SW.
		 */
		g_lpc_mode = LPC_LP_DEEP_SW_STANDBY;
		R_BSP_SoftwareDelay(3, BSP_DELAY_SECS);	/* Run for 3 seconds */
		tx_thread_suspend(&thread_0);   		/* Enter to low power mode automatically */

	    /* Never reach here because the system is resumed by reset. */
	   	DEMO_FOREVER_LOOP();		/* Block just in case. */
		#endif	/* DEMO_LP_DEEP_SW_STANDBY */
	}
}

/* Define Low Power Enter function */
void Demo_LowPower_Enter(void)
{
	g_lpc_status = LPC_SUCCESS;
	#if DEMO_LP_DEEP_SLEEP
	int saved_status;
	#endif

	R_GPIO_PinWrite(DEMO_LED_PIN, DEMO_LED_OFF);  	/* LED OFF */

	switch (g_lpc_mode)
	{
	#if DEMO_LP_SLEEP
	default:	/* LPC_LP_SLEEP */
	case LPC_LP_SLEEP:
		/* Set to sleep mode*/
		g_lpc_status = R_LPC_LowPowerModeConfigure (LPC_LP_SLEEP);
		if (LPC_SUCCESS != g_lpc_status)
			DEMO_FOREVER_LOOP();

		/* Pause CMT that threadx uses, because it is one of the interrupts to release from the sleep mode.*/
		R_CMT_Control(chan, CMT_RX_CMD_PAUSE, NULL);

		/* Transition to sleep mode*/
		g_lpc_status = R_LPC_LowPowerModeActivate(FIT_NO_FUNC);
		if (LPC_SUCCESS != g_lpc_status)
			DEMO_FOREVER_LOOP();

		R_CMT_Control(chan, CMT_RX_CMD_RESUME, NULL);
		break;
	#endif	/* DEMO_LP_SLEEP */

	#if DEMO_LP_DEEP_SLEEP
	case LPC_LP_DEEP_SLEEP:
		/* Set to deep sleep mode*/
		g_lpc_status = R_LPC_LowPowerModeConfigure (LPC_LP_DEEP_SLEEP);
		if (LPC_SUCCESS != g_lpc_status)
			DEMO_FOREVER_LOOP();

		/* Pause CMT that threadx uses, because it is one of the interrupts to release from the sleep mode.*/
		R_CMT_Control(chan, CMT_RX_CMD_PAUSE, NULL);

		/* Stop DTC module enabled at reset, because it is one of the conditions to enter deep sleep mode. */
		R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
		saved_status = SYSTEM.MSTPCRA.BIT.MSTPA28;
		SYSTEM.MSTPCRA.BIT.MSTPA28 = 1;

		/* Transition to sleep mode*/
		g_lpc_status = R_LPC_LowPowerModeActivate(FIT_NO_FUNC);
		if (LPC_SUCCESS != g_lpc_status)
			DEMO_FOREVER_LOOP();

		/* Restore DTC module condition. */
		SYSTEM.MSTPCRA.BIT.MSTPA28 = saved_status;
		R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

		R_CMT_Control(chan, CMT_RX_CMD_RESUME, NULL);
		break;
	#endif	/* DEMO_LP_DEEP_SLEEP */

	#if DEMO_LP_SW_STANDBY
	case LPC_LP_SW_STANDBY:
		/* Transition to software standby mode*/
		g_lpc_status = R_LPC_LowPowerModeConfigure (LPC_LP_SW_STANDBY);
		if (LPC_SUCCESS != g_lpc_status)
			DEMO_FOREVER_LOOP();

		g_lpc_status = R_LPC_LowPowerModeActivate(FIT_NO_FUNC);
		if (LPC_SUCCESS != g_lpc_status)
			DEMO_FOREVER_LOOP();
		break;
	#endif	/* DEMO_LP_SW_STANDBY */

	#if DEMO_LP_DEEP_SW_STANDBY
	case LPC_LP_DEEP_SW_STANDBY:
		/* Set the warm start */
		SYSTEM.RSTSR1.BIT.CWSF = 1;

    	/* Setup to release from deep software standby */
		R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);

        #if DEMO_DEEP_SW_STANDBY_USE_RTC
		/* RTC Alarm for RX72N Envision Kit */
    	SYSTEM.DPSIFR2.BIT.DRTCAIF = 0;		/* Clear RTC Alarm request for Deep SW Standby */
    	SYSTEM.DPSIER2.BIT.DRTCAIE = 1;		/* Enable RTC Alarm interrupt for Deep SW Standby */
        #else
     	/* IRQ1 for RX65N Cloud Kit and IRQn for other boards */
    	SYSTEM.DPSIFR0.BIT.DIRQ1F = 0;		/* Clear IRQ request for Deep SW Standby */
    	SYSTEM.DPSIEGR0.BIT.DIRQ1EG = 0;
    	SYSTEM.DPSIER0.BIT.DIRQ1E = 1;		/* Enable IRQ interrupt for Deep SW Standby */
		#endif	/* DEMO_BOARD_RX65NCK */

		R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

		/* Transition to deep software standby mode*/
		g_lpc_status = R_LPC_LowPowerModeConfigure (LPC_LP_DEEP_SW_STANDBY);
		if (LPC_SUCCESS != g_lpc_status)
			DEMO_FOREVER_LOOP();
		g_lpc_status = R_LPC_LowPowerModeActivate(FIT_NO_FUNC);
		if (LPC_SUCCESS != g_lpc_status)
			DEMO_FOREVER_LOOP();
		break;
	#endif	/* DEMO_LP_DEEP_SW_STANDBY */
	}
}

/* Define Low Power Exit function */
void Demo_LowPower_Exit(void)
{
	R_GPIO_PinWrite(DEMO_LED_PIN, DEMO_LED_ON);  	/* LED ON */
}

/*
 * Define Low Power Timer Setup function.
 * This is just skeleton function, so you need to write program according to your device.
 */
#if defined(TX_LOW_POWER_TIMER_SETUP) && (!defined(TX_LOW_POWER_TICKLESS))

void Demo_LowPower_Timer_Setup(ULONG tx_low_power_next_expiration)
{
	if ((ULONG)0xffffffff != tx_low_power_next_expiration)
	{
		/* Because there is a active timer, we do not transit to low power mode. */
		return;
	}

	/* Start user code. */

	/*
	 * Set up RTC or User timer to wake up after tx_low_power_next_expiration passed.
	 */

	#if DEMO_DEEP_SW_STANDBY_USE_RTC

	if (LPC_LP_DEEP_SW_STANDBY != g_lpc_mode)
	{
		/* Do nothing since other mode will be wake up by pressing user SW. */
		return;
	}

	/*
	 * Set up RTC on LPC_LP_DEEP_SW_STANDBY mode to wake up after DEMO_DEEP_SW_STANDBY_TIMEOUT_SECONDS.
	 */
	uint32_t time;

	/* get current counter, and calculate alarm time counter */
	time = ( RTC.BCNT0.BIT.BCNT | RTC.BCNT1.BIT.BCNT << 8 | RTC.BCNT2.BIT.BCNT << 16 | RTC.BCNT3.BIT.BCNT << 24);
	time += DEMO_DEEP_SW_STANDBY_TIMEOUT_SECONDS;

    R_BSP_InterruptRequestDisable(VECT(RTC, ALM));	/* Disable alarm interrupt */

	RTC.BCNT0AR.BIT.BCNTAR = (time & 0xff);			/* Set alarm counter */
	RTC.BCNT1AR.BIT.BCNTAR = ((time >> 8) & 0xff);
	RTC.BCNT2AR.BIT.BCNTAR = ((time >> 16) & 0xff);
	RTC.BCNT3AR.BIT.BCNTAR = ((time >> 24) & 0xff);

	RTC.BCNT0AER.BIT.ENB = 0xff;	/* Enable to match all counter */
	RTC.BCNT1AER.BIT.ENB = 0xff;
	RTC.BCNT2AER.BIT.ENB = 0xff;
	RTC.BCNT3AER.BIT.ENB = 0xff;

    RTC.RCR1.BIT.AIE = 1;		    /* Enable alarm interrupt in RTC */
    DEMO_WAIT_FOR_COMPLETION(RTC.RCR1.BIT.AIE, 1);
    RTC.RCR1.BIT.PES = 0;		    /* no periodic */
    DEMO_WAIT_FOR_COMPLETION(RTC.RCR1.BIT.PES, 0);

    ICU.IPR[IPR_RTC_ALM].BIT.IPR = 6;
    ICU.IR[IR_RTC_CUP].BIT.IR = 0;
    R_BSP_InterruptRequestEnable(VECT(RTC, ALM));	/* Enable alarm interrupt in ICU */
	#endif 	/* DEMO_DEEP_SW_STANDBY_USE_RTC */

	/* End user code. */
}
#endif	/* defined(TX_LOW_POWER_TIMER_SETUP) && (!defined(TX_LOW_POWER_TICKLESS)) */

/*
 * Define Low Power User Timer Adjust function
 * This is just skeleton function, so you need to write program according to your device.
 */
#if defined(TX_LOW_POWER_USER_TIMER_ADJUST) && (!defined(TX_LOW_POWER_TICKLESS))
ULONG Demo_LowPower_User_Timer_Adjust(void)
{
	ULONG increment = (ULONG) 0;

	/* Start user code. */

		/*
		 *  Calculate increment of System Timer ticks for ThreadX:
		 * 		Passed_Time = RTC_Stop_Time - RTC_Start_Time;
		 * 		increment = ConvertTimeToTicks(Passed_Time);
		 */

	/* End user code. */
	return increment;
}
#endif	/* defined(TX_LOW_POWER_USER_TIMER_ADJUST) && (!defined(TX_LOW_POWER_TICKLESS)) */


#if DEMO_DEEP_SW_STANDBY_USE_RTC
void Demo_Init_RTC_For_Deep_SW_Standby(void)
{
	/* Start user code (Initialize RTC) */

	int i;

	/* Main clock oscillator is forcedly oscillated for RTC. */
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
	SYSTEM.MOFCR.BIT.MOFXIN = 1;
	R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_LPC_CGC_SWR);

	/* Setup RTC for binary counter mode */
	RTC.RCR4.BIT.RCKSEL = 1;	/* Select main clock */
	for (i = 0; i < 6; i++)
	{
		R_BSP_NOP();			/* Wait for over 6 clocks */
	}
	RTC.RCR2.BIT.START = 0;
	DEMO_WAIT_FOR_COMPLETION(RTC.RCR2.BIT.START, 0);

	RTC.RFRH.BIT.RFC = DEMO_RTC_RFRH;	/* Set value on main clock 16MHz described in Hardware Manual 33.2.21 */
	RTC.RFRL.BIT.RFC = DEMO_RTC_RFRL;
	RTC.RCR2.BIT.CNTMD = 1;	/* binary count mode */
	DEMO_WAIT_FOR_COMPLETION(RTC.RCR2.BIT.CNTMD, 1);

	RTC.RCR2.BIT.RESET = 1;
	DEMO_WAIT_FOR_COMPLETION(RTC.RCR2.BIT.RESET, 0);

	RTC.BCNT0.BIT.BCNT = 0;
	RTC.BCNT1.BIT.BCNT = 0;
	RTC.BCNT2.BIT.BCNT = 0;
	RTC.BCNT3.BIT.BCNT = 0;

	RTC.RCR2.BIT.START = 1;
	DEMO_WAIT_FOR_COMPLETION(RTC.RCR2.BIT.START, 1);
	/* End user code */
}
#endif	/* DEMO_DEEP_SW_STANDBY_USE_RTC */
