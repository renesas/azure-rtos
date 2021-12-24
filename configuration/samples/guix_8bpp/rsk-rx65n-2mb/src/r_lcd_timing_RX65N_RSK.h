/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name    : r_lcd_timing_RX65N_RSK.h
* Description  : LCD panel for glcdc channel 0 definition header
******************************************************************************/
#ifndef LCD_TIMING_H
#define LCD_TIMING_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_glcdc_rx_if.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define     LCD_CH0_W_VSYNC         (10u)                    /* Vertical Pulse Width(VPW)                                   */
#define     LCD_CH0_DISP_VS         (10u + 4u)                    /* LCD display area vertical start position
                                                                 = Vertical Pulse Width(VPW) + Vertical Back Porch(VBP)     */
#define     LCD_CH0_DISP_VW         (272u)                   /* Vertical Display Period(VDP)                                */

#define     LCD_CH0_W_HSYNC         (41u)                    /* Horizontal Pulse Width(HPW)                                 */
#define     LCD_CH0_DISP_HS         (41u + 41u)                    /* LCD display area horizontal start position
                                                                 = Horizontal Pulse Width(HPW) + Horizontal Back Porch(HBP) */
#define     LCD_CH0_DISP_HW         (480u)                   /* Horizontal Display Period(HDP)                              */

#define     LCD_CH0_SIG_FV          (289u)              /* Vertical Total Period(VTP)                                  */
#define     LCD_CH0_SIG_FH          (578u)             /* Horizontal Total Period(HTP)                                */

#define     LCD_CH0_W_VFP           (3u)             /* Vertical Front Porch(VFP)                                   */
#define     LCD_CH0_W_HFP           (16u)             /* Horizontal Front Porch(HFP)                                 */

#define     LCD_CH0_W_VBP           (4u)             /* Vertical Back Porch(VBP)                                  */
#define     LCD_CH0_W_HBP           (41u)             /* Horizontal Back Porch(HBP)                                */

#define     LCD_CH0_TCON_PIN_VSYNC  GLCDC_TCON_PIN_0      /* Select TCON of a Vsync signal (Vsync_TCON_select)           */
#define     LCD_CH0_TCON_PIN_HSYNC  GLCDC_TCON_PIN_2      /* Select TCON of a Hsync signal (Hsync_TCON_select)           */
#define     LCD_CH0_TCON_PIN_DE     GLCDC_TCON_PIN_3      /* Select TCON of a DE signal    (DE_TCON_select)              */

#define     LCD_CH0_TCON_POL_VSYNC  GLCDC_SIGNAL_POLARITY_LOACTIVE    /* Select polarity of a Vsync signal (Vsync_POL)               */
#define     LCD_CH0_TCON_POL_HSYNC  GLCDC_SIGNAL_POLARITY_LOACTIVE    /* Select polarity of a Hsync signal (Hsync_POL)               */
#define     LCD_CH0_TCON_POL_DE     GLCDC_SIGNAL_POLARITY_HIACTIVE/* Select polarity of a Hsync signal (DE_POL)                  */

#define     LCD_CH0_OUT_EDGE        (GLCDC_SIGNAL_SYNC_EDGE_RISING)       /* Output phase control of LCD_DATA signal(LCD_DATA_OUT_EDGE)  */
#define     LCD_CH0_OUT_FORMAT      (GLCDC_OUT_FORMAT_16BITS_RGB565) /* LCD output format select (LCD_OUT_FORMAT)                */

#endif  /* LCD_TIMING_H */

