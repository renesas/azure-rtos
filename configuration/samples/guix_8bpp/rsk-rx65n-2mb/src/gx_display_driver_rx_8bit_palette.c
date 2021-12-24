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

/***********************************************************************************************************************
 * Copyright [2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** GUIX Display Driver component                                         */
/**************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include    <stdio.h>
#include    <string.h>

#include    "platform.h"
#include    "r_glcdc_rx_if.h"
#include    "r_glcdc_rx_pinset.h"
#include    "src/r_glcdc_private.h"

#include    "gx_api.h"
#include    "gx_display.h"
#include    "gx_utility.h"


static void glcdc_initialize_8bit_palette (void);

/* functions shared with 16/32 bpp driver */
extern void qe_for_display_parameter_set (glcdc_cfg_t * glcdc_qe_cfg);
extern void glcdc_callback (void * pdata);
extern void board_port_setting (void);

// Studio generated setup function
UINT _gx_rx_display_driver_setup(GX_DISPLAY *display);
VOID rx_hardware_frame_toggle(GX_CANVAS *canvas);

//extern R_BSP_VOLATILE_EVENACCESS st_glcdc_gr_t *graphics_regs[GLCDC_FRAME_LAYER_NUM];

#define IMAGE_WIDTH          (480u)  /* Width of image used in this sample. */
#define IMAGE_HEIGHT         (272u)  /* Height of image used in this sample. */
#define IMAGE_PIXEL_SIZE     (2u)    /* Graphic format: RGB565, 2 bytes per pixel */

#define FRAME_BUF_BASE_ADDR_A 0x00800000
#define FRAME_BUF_BASE_ADDR_B 0x00822000

extern glcdc_cfg_t gs_glcdc_init_cfg;                   /* Argument of R_GLCDC_Open function. */
extern volatile bool gs_first_interrupt_flag;           /* It is used for interrupt control of GLCDC module */
extern const GX_COLOR main_display_theme_1_palette[256];

/***********************************************************************************************************************
 * Macro definitions
 ***********************************************************************************************************************/

/** Space used to store int to fixed point polygon vertices. */
#define MAX_POLYGON_VERTICES GX_POLYGON_MAX_EDGE_NUM

/*LDRA_INSPECTED 77 S This macro does not work when surrounded by parentheses. */
#define DRAW_PIXEL  if (glyph_data & mask)              \
                    {                                   \
                        *put = text_color;              \
                    }                                   \
                    put++;                              \
                    mask = (GX_UBYTE)(mask << 1);


/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** indicator for the number of visible frame buffer */
typedef enum e_frame_buffers
{
    FRAME_BUFFER_A = 0,
    FRAME_BUFFER_B
} frame_buffers_t;


/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/** indicator for the number of visible frame buffer */
extern USHORT *visible_frame;
extern USHORT *working_frame;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static VOID     gx_copy_visible_to_working_8bpp(GX_CANVAS *canvas, GX_RECTANGLE *copy);
static VOID     gx_flip_canvas_to_working_8bpp(GX_CANVAS *canvas, GX_RECTANGLE *copy);


VOID _gx_display_driver_8bit_palette_assign(GX_DISPLAY *display, GX_COLOR *palette, INT count)
{
    display -> gx_display_palette = palette;
    display -> gx_display_palette_size = (UINT)count;
}

VOID _gx_rx_buffer_toggle_8bpp(GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{
    GX_PARAMETER_NOT_USED(dirty);

    GX_RECTANGLE Limit;
    GX_RECTANGLE copy;
    GX_DISPLAY *display;
    INT rotation_angle;

    display = canvas->gx_canvas_display;

    _gx_utility_rectangle_define(&Limit, 0, 0,
                                 (GX_VALUE)(canvas->gx_canvas_x_resolution - 1),
                                 (GX_VALUE)(canvas->gx_canvas_y_resolution - 1));

    rotation_angle = display->gx_display_rotation_angle;

    if(rotation_angle != 0)
    {
        if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &copy))
        {
            gx_flip_canvas_to_working_8bpp(canvas, &copy);
        }
    }


    rx_hardware_frame_toggle(canvas);

    if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &copy))
    {
        if(rotation_angle == 0)
        {
            gx_copy_visible_to_working_8bpp(canvas, &copy);
        }
        else
        {
            gx_flip_canvas_to_working_8bpp(canvas, &copy);
        }
    }
}

GX_RECTANGLE copy_of_copy;

/*******************************************************************************************************************//**
 * @brief  GUIX display driver for draw2d, Frame buffer toggle operation with copying data by software without
 * D/AVE 2D acceleration and screen rotation.
 * This function is called by _gx_draw2d_buffer_toggle_8bpp().
 * @param[in]     canvas            Pointer to a canvas
 * @param[in]     copy              Pointer to a rectangle region to copy
 **********************************************************************************************************************/
static VOID gx_copy_visible_to_working_8bpp(GX_CANVAS *canvas, GX_RECTANGLE *copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;

    ULONG        *pGetRow;
    ULONG        *pPutRow;

    INT          copy_width;
    INT          copy_height;
    INT          canvas_stride;
    INT          display_stride;

    ULONG        *pGet;
    ULONG        *pPut;
    INT          row;
    INT          col;

    GX_DISPLAY *display = canvas->gx_canvas_display;

    _gx_utility_rectangle_define(&display_size, 0, 0,
            (GX_VALUE)(display->gx_display_width - 1),
            (GX_VALUE)(display->gx_display_height - 1));
    copy_clip = *copy;
    copy_of_copy = *copy;

    /** Align copy region on 32-bit boundary. */
    copy_clip.gx_rectangle_left  = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_left & 0xfffcU);
    copy_clip.gx_rectangle_right = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_right | 3U);

    /** Offset canvas within frame buffer. */
    _gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    _gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (INT)(copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (INT)(copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (ULONG *) visible_frame;
    pPutRow = (ULONG *) working_frame;

    copy_width /= 4;
    canvas_stride = (INT) (canvas->gx_canvas_x_resolution / 4);
    pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_top * canvas_stride);
    pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_left / 4);

    display_stride = (INT)display->gx_display_width / 4;
    pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * display_stride);
    pGetRow = pGetRow + (INT)((canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left) / 4);

    for (row = 0; row < copy_height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;

        for (col = 0; col < copy_width; col++)
        {
            *pPut = *pGet;
            ++pPut;
            ++pGet;
        }
        pPutRow += canvas_stride;
        pGetRow += display_stride;
    }
}


static VOID gx_flip_canvas_to_working_8bpp(GX_CANVAS *canvas, GX_RECTANGLE *copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;

    GX_UBYTE *pGet;
    GX_UBYTE *pPut;
    GX_UBYTE *pGetRow;
    GX_UBYTE *pPutRow;

    INT          copy_width;
    INT          copy_height;
    INT          canvas_stride;
    INT          display_stride;
    INT          row;
    INT          col;

    GX_DISPLAY *display = canvas->gx_canvas_display;
    gx_utility_rectangle_define(&display_size, 0, 0,
                                (GX_VALUE)(display->gx_display_width - 1),
                                (GX_VALUE)(display->gx_display_height - 1));
    copy_clip = *copy;

    /** Offset canvas within frame buffer. */
    gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (INT)(copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (INT)(copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (GX_UBYTE *) canvas->gx_canvas_memory;
    pPutRow = (GX_UBYTE *) working_frame;

    canvas_stride = (INT)canvas->gx_canvas_x_resolution;
    display_stride = (INT)display->gx_display_width;

    pPutRow +=  display->gx_display_height * display_stride;
    pPutRow -= copy_clip.gx_rectangle_top * display_stride;
    pPutRow -= copy_clip.gx_rectangle_left + 1;

    pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) *canvas_stride);
    pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

    for (row = 0; row < copy_height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;

        for (col = 0; col < copy_width; col++)
        {
            *pPut-- = *pGet++;
        }
        pPutRow -= display_stride;
        pGetRow += canvas_stride;
    }
}

static void glcdc_initialize_8bit_palette (void)
{
    glcdc_err_t ret;

    /* ==== QE for Display [RX] setting parameter ==== */
    qe_for_display_parameter_set( &gs_glcdc_init_cfg);

    /* ==== User setting parameter ==== */
    /* ---- Graphic 2 setting ---- */
    /* Image format */
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].p_base        = (uint32_t *) visible_frame;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].hsize         = IMAGE_WIDTH;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].vsize         = IMAGE_HEIGHT;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].offset        = 512;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].format        = GLCDC_IN_FORMAT_CLUT8;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].frame_edge    = false;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].coordinate.x  = 0;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].coordinate.y  = 0;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].bg_color.argb = 0x00CCCCCC;

    /* Alpha blend */
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].visible            = true;
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].blend_control      = GLCDC_BLEND_CONTROL_NONE;

    /* When blend_control is GLCDC_BLEND_CONTROL_NONE, setting values of following blend structure members are ignored. */
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].frame_edge         = false;
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].fixed_blend_value  = 0x00;
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].fade_speed         = 0x00;
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].start_coordinate.x = 0;
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].start_coordinate.y = 0;
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].end_coordinate.x   = IMAGE_WIDTH;
    gs_glcdc_init_cfg.blend[GLCDC_FRAME_LAYER_2].end_coordinate.y   = IMAGE_HEIGHT;

    /* Chromakey */
    gs_glcdc_init_cfg.chromakey[GLCDC_FRAME_LAYER_2].enable      = false;

    /* When enable is false, setting values of structure members under chromakey are ignored. */
    gs_glcdc_init_cfg.chromakey[GLCDC_FRAME_LAYER_2].before.argb = 0x00FFFFFF;
    gs_glcdc_init_cfg.chromakey[GLCDC_FRAME_LAYER_2].after.argb  = 0xFFFFFFFF;

    /* Color look-up table */
    gs_glcdc_init_cfg.clut[GLCDC_FRAME_LAYER_2].enable = true;

    /* When enable is false, setting values of structure members under clut are ignored. */
    gs_glcdc_init_cfg.clut[GLCDC_FRAME_LAYER_2].p_base = (uint32_t *) main_display_theme_1_palette;
    gs_glcdc_init_cfg.clut[GLCDC_FRAME_LAYER_2].start  = 0;
    gs_glcdc_init_cfg.clut[GLCDC_FRAME_LAYER_2].size   = 256;

    /* ---- Graphic 1 setting ---- */
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_1].p_base = (uint32_t *) NULL; /* Graphic 1 is disable */

    /* When p_base is NULL, setting values of structure members under input[GLCDC_FRAME_LAYER_1] are ignored. */

    /* ---- Hard ware and other setting ---- */
    /* Back ground color */
    gs_glcdc_init_cfg.output.bg_color.argb = 0x00CCCCCC;

    /* Endian */
    gs_glcdc_init_cfg.output.endian = GLCDC_ENDIAN_LITTLE;

    /* Color order */
    gs_glcdc_init_cfg.output.color_order = GLCDC_COLOR_ORDER_BGR; /* RSKRX65N-2MB board dependent setting */

    /* Output clock */
    gs_glcdc_init_cfg.output.clksrc          = GLCDC_CLK_SRC_INTERNAL;
    gs_glcdc_init_cfg.output.clock_div_ratio = GLCDC_PANEL_CLK_DIVISOR_24;

    /* Detection */
    gs_glcdc_init_cfg.detection.vpos_detect  = true;
    gs_glcdc_init_cfg.detection.gr1uf_detect = false;
    gs_glcdc_init_cfg.detection.gr2uf_detect = true;

    /* Interrupt */
    gs_glcdc_init_cfg.interrupt.vpos_enable  = true;
    gs_glcdc_init_cfg.interrupt.gr1uf_enable = false;
    gs_glcdc_init_cfg.interrupt.gr2uf_enable = true;

    /* Call back function register */
    gs_glcdc_init_cfg.p_callback = (void (*) (void *)) glcdc_callback;

    /* -- Initialize a first time interrupt flag -- */
    /* Unintended specified line notification from graphic 2 and graphic 1, 2 underflow is detected only
     for first time after release GLCDC software reset.
     This variable is a flag to skip the first time interrupt processing.
     Refer to Graphic LCD Controller (GLCDC) section of User's Manual: Hardware for details. */
    gs_first_interrupt_flag = false;

    /* ==== GLCDC initialize and start GLCDC operation ==== */

    /* GLCDC initialize */
    ret = R_GLCDC_Open( &gs_glcdc_init_cfg);
    if (GLCDC_SUCCESS != ret)
    {
        while (1)
        {
        }
    }

    /* GLCDC port setting */
    R_GLCDC_PinSet();

    /* Board dependent port setting */
    board_port_setting();

    /* Start displaying graphics */
    ret = R_GLCDC_Control(GLCDC_CMD_START_DISPLAY, NULL);
    if (GLCDC_SUCCESS != ret)
    {
        while (1)
        {
        }
    }

    memset(visible_frame, 0x00, 480*272);
    memset(working_frame, 0x00, 480*272);
} 

/*******************************************************************************************************************/
void rx_graphics_driver_setup_8bit_palette(GX_DISPLAY *display)
{

	// call the Studio generated initialization function:
	_gx_rx_display_driver_setup(display);

	visible_frame = (USHORT *) FRAME_BUF_BASE_ADDR_A;
	working_frame = (USHORT *) FRAME_BUF_BASE_ADDR_B;

    /* ---- Initialization of the GLCDC , and start display ---- */
    glcdc_initialize_8bit_palette();
}

