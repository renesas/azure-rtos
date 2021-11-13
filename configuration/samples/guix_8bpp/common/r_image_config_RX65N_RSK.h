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
* File Name    : r_image_config_RX65N_RSK.h
* Description  : Defines for Image Correction
******************************************************************************/
#ifndef IMG_CONFIG_H
#define IMG_CONFIG_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_glcdc_rx_if.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/* Calibration Route Setting */
#define IMGC_OUTCTL_CALIB_ROUTE              (GLCDC_BRIGHTNESS_CONTRAST_TO_GAMMA)

/* Brightness Setting */
#define IMGC_BRIGHT_OUTCTL_ACTIVE            (true)
#define IMGC_BRIGHT_OUTCTL_OFFSET_G          (512u)
#define IMGC_BRIGHT_OUTCTL_OFFSET_B          (512u)
#define IMGC_BRIGHT_OUTCTL_OFFSET_R          (512u)

/* Contrast Setting */
#define IMGC_CONTRAST_OUTCTL_ACTIVE          (true)
#define IMGC_CONTRAST_OUTCTL_GAIN_G          (128u)
#define IMGC_CONTRAST_OUTCTL_GAIN_B          (128u)
#define IMGC_CONTRAST_OUTCTL_GAIN_R          (128u)

/* Gamma correction */
#define IMGC_GAMMA_ACTIVE                    (true)
#define IMGC_GAMMA_G_GAIN_00                 (1317u)
#define IMGC_GAMMA_G_GAIN_01                 (1157u)
#define IMGC_GAMMA_G_GAIN_02                 (1103u)
#define IMGC_GAMMA_G_GAIN_03                 (1069u)
#define IMGC_GAMMA_G_GAIN_04                 (1045u)
#define IMGC_GAMMA_G_GAIN_05                 (1026u)
#define IMGC_GAMMA_G_GAIN_06                 (1010u)
#define IMGC_GAMMA_G_GAIN_07                 (997u)
#define IMGC_GAMMA_G_GAIN_08                 (986u)
#define IMGC_GAMMA_G_GAIN_09                 (976u)
#define IMGC_GAMMA_G_GAIN_10                 (967u)
#define IMGC_GAMMA_G_GAIN_11                 (959u)
#define IMGC_GAMMA_G_GAIN_12                 (952u)
#define IMGC_GAMMA_G_GAIN_13                 (945u)
#define IMGC_GAMMA_G_GAIN_14                 (939u)
#define IMGC_GAMMA_G_GAIN_15                 (919u)
#define IMGC_GAMMA_G_TH_01                   (64u)
#define IMGC_GAMMA_G_TH_02                   (128u)
#define IMGC_GAMMA_G_TH_03                   (192u)
#define IMGC_GAMMA_G_TH_04                   (256u)
#define IMGC_GAMMA_G_TH_05                   (320u)
#define IMGC_GAMMA_G_TH_06                   (384u)
#define IMGC_GAMMA_G_TH_07                   (448u)
#define IMGC_GAMMA_G_TH_08                   (512u)
#define IMGC_GAMMA_G_TH_09                   (576u)
#define IMGC_GAMMA_G_TH_10                   (640u)
#define IMGC_GAMMA_G_TH_11                   (704u)
#define IMGC_GAMMA_G_TH_12                   (768u)
#define IMGC_GAMMA_G_TH_13                   (832u)
#define IMGC_GAMMA_G_TH_14                   (896u)
#define IMGC_GAMMA_G_TH_15                   (960u)
#define IMGC_GAMMA_B_GAIN_00                 (1317u)
#define IMGC_GAMMA_B_GAIN_01                 (1157u)
#define IMGC_GAMMA_B_GAIN_02                 (1103u)
#define IMGC_GAMMA_B_GAIN_03                 (1069u)
#define IMGC_GAMMA_B_GAIN_04                 (1045u)
#define IMGC_GAMMA_B_GAIN_05                 (1026u)
#define IMGC_GAMMA_B_GAIN_06                 (1010u)
#define IMGC_GAMMA_B_GAIN_07                 (997u)
#define IMGC_GAMMA_B_GAIN_08                 (986u)
#define IMGC_GAMMA_B_GAIN_09                 (976u)
#define IMGC_GAMMA_B_GAIN_10                 (967u)
#define IMGC_GAMMA_B_GAIN_11                 (959u)
#define IMGC_GAMMA_B_GAIN_12                 (952u)
#define IMGC_GAMMA_B_GAIN_13                 (945u)
#define IMGC_GAMMA_B_GAIN_14                 (939u)
#define IMGC_GAMMA_B_GAIN_15                 (919u)
#define IMGC_GAMMA_B_TH_01                   (64u)
#define IMGC_GAMMA_B_TH_02                   (128u)
#define IMGC_GAMMA_B_TH_03                   (192u)
#define IMGC_GAMMA_B_TH_04                   (256u)
#define IMGC_GAMMA_B_TH_05                   (320u)
#define IMGC_GAMMA_B_TH_06                   (384u)
#define IMGC_GAMMA_B_TH_07                   (448u)
#define IMGC_GAMMA_B_TH_08                   (512u)
#define IMGC_GAMMA_B_TH_09                   (576u)
#define IMGC_GAMMA_B_TH_10                   (640u)
#define IMGC_GAMMA_B_TH_11                   (704u)
#define IMGC_GAMMA_B_TH_12                   (768u)
#define IMGC_GAMMA_B_TH_13                   (832u)
#define IMGC_GAMMA_B_TH_14                   (896u)
#define IMGC_GAMMA_B_TH_15                   (960u)
#define IMGC_GAMMA_R_GAIN_00                 (1317u)
#define IMGC_GAMMA_R_GAIN_01                 (1157u)
#define IMGC_GAMMA_R_GAIN_02                 (1103u)
#define IMGC_GAMMA_R_GAIN_03                 (1069u)
#define IMGC_GAMMA_R_GAIN_04                 (1045u)
#define IMGC_GAMMA_R_GAIN_05                 (1026u)
#define IMGC_GAMMA_R_GAIN_06                 (1010u)
#define IMGC_GAMMA_R_GAIN_07                 (997u)
#define IMGC_GAMMA_R_GAIN_08                 (986u)
#define IMGC_GAMMA_R_GAIN_09                 (976u)
#define IMGC_GAMMA_R_GAIN_10                 (967u)
#define IMGC_GAMMA_R_GAIN_11                 (959u)
#define IMGC_GAMMA_R_GAIN_12                 (952u)
#define IMGC_GAMMA_R_GAIN_13                 (945u)
#define IMGC_GAMMA_R_GAIN_14                 (939u)
#define IMGC_GAMMA_R_GAIN_15                 (919u)
#define IMGC_GAMMA_R_TH_01                   (64u)
#define IMGC_GAMMA_R_TH_02                   (128u)
#define IMGC_GAMMA_R_TH_03                   (192u)
#define IMGC_GAMMA_R_TH_04                   (256u)
#define IMGC_GAMMA_R_TH_05                   (320u)
#define IMGC_GAMMA_R_TH_06                   (384u)
#define IMGC_GAMMA_R_TH_07                   (448u)
#define IMGC_GAMMA_R_TH_08                   (512u)
#define IMGC_GAMMA_R_TH_09                   (576u)
#define IMGC_GAMMA_R_TH_10                   (640u)
#define IMGC_GAMMA_R_TH_11                   (704u)
#define IMGC_GAMMA_R_TH_12                   (768u)
#define IMGC_GAMMA_R_TH_13                   (832u)
#define IMGC_GAMMA_R_TH_14                   (896u)
#define IMGC_GAMMA_R_TH_15                   (960u)

/* Dither Process */
#define IMGC_DITHER_ACTIVE                   (false)
#define IMGC_DITHER_MODE                     (GLCDC_DITHERING_MODE_TRUNCATE)
#define IMGC_DITHER_2X2_PA                   (GLCDC_DITHERING_PATTERN_11)
#define IMGC_DITHER_2X2_PB                   (GLCDC_DITHERING_PATTERN_00)
#define IMGC_DITHER_2X2_PC                   (GLCDC_DITHERING_PATTERN_10)
#define IMGC_DITHER_2X2_PD                   (GLCDC_DITHERING_PATTERN_01)

#endif  /* IMG_CONFIG_H */
