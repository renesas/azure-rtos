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

#if (GX_USE_DAVE2D_DRAW == 1)
#include    "dave_driver.h"
#endif


#include    "gx_api.h"
#include    "gx_display.h"
#include    "gx_utility.h"

#define IMAGE_WIDTH          (480u)  /* Width of image used in this sample. */
#define IMAGE_HEIGHT         (272u)  /* Height of image used in this sample. */
#define IMAGE_PIXEL_SIZE     (2u)    /* Graphic format: RGB565, 2 bytes per pixel */

/* Frame buffer address of graphics 2 is the FRAME_BUFFER section top address */

#define FRAME_BUF_BASE_ADDR_A 0x00800000
#define FRAME_BUF_BASE_ADDR_B 0x00000400

USHORT   *visible_frame;
USHORT   *working_frame;

R_BSP_VOLATILE_EVENACCESS st_glcdc_gr_t *graphics_regs[GLCDC_FRAME_LAYER_NUM];

/***********************************************************************************************************************
  RX GUIX display driver function prototypes (called by GUIX)
 **********************************************************************************************************************/


// Studio generated setup function
UINT _gx_rx_display_driver_setup(GX_DISPLAY *display);

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static VOID     gx_copy_visible_to_working(GX_CANVAS  *canvas, GX_RECTANGLE *copy);
static VOID     gx_flip_canvas_to_working_16bpp(GX_CANVAS  *canvas, GX_RECTANGLE *copy);
static VOID     gx_flip_canvas_to_working_16bpp_draw(USHORT *pGetRow, USHORT *pPutRow, INT width, INT height,
                                                                                                        INT stride);
static VOID     gx_flip_canvas_to_working_32bpp(GX_CANVAS  *canvas, GX_RECTANGLE *copy);
static VOID     gx_flip_canvas_to_working_32bpp_draw(ULONG *pGetRow, ULONG *pPutRow, INT width, INT height,
                                                                                                     INT stride);

/*******************************************************************************************************************/
VOID rx_hardware_frame_toggle(GX_CANVAS *canvas)
{

	/* swap the frame buffers */
    volatile ULONG *regptr;

	USHORT *temp = visible_frame;
	visible_frame = working_frame;

    // wait for VEN bit cleared
    regptr = (ULONG *) 0x000E1200;

	while(*regptr == 1)
    {

    }

    regptr = (ULONG *) 0x000E1000;

    // wait for BGEN.VEN = 0
    while(*regptr & 0x0100)
    {
    }

    // assign frame buffer pointer
    regptr = (ULONG *) 0x000E120C;
	//graphics_regs[1]->grxflm2 = (uint32_t) visible_frame;
    *regptr = (ULONG) visible_frame;

    // set the VEN bit
    regptr = (ULONG *) 0x000E1200;
    *regptr = 1;

    // wait for VEN bit to be cleared
	while(*regptr == 1)
    {

    }

	working_frame = temp;
	canvas->gx_canvas_memory = (GX_COLOR *) working_frame;
}

/*******************************************************************************************************************/
VOID rx_frame_pointers_get(ULONG display_handle, GX_UBYTE **pvisible, GX_UBYTE **pworking)
{
	*pvisible = (GX_UBYTE *) visible_frame;
	*pworking = (GX_UBYTE *) working_frame;

	//gp_gr[1]->grxflm2 = (uint32_t) visible_frame;
}


/*******************************************************************************************************************/
VOID _gx_rx_buffer_toggle(GX_CANVAS  *canvas, GX_RECTANGLE *dirty)
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
			gx_flip_canvas_to_working_16bpp(canvas, &copy);
		}
	}

	rx_hardware_frame_toggle(canvas);

	if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &copy))
	{
		if(rotation_angle == 0)
		{
			gx_copy_visible_to_working(canvas, &copy);
		}
		else
		{
			gx_flip_canvas_to_working_16bpp(canvas, &copy);
		}
	}
}


/*******************************************************************************************************************/
static VOID gx_copy_visible_to_working(GX_CANVAS  *canvas, GX_RECTANGLE *copy)
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
    gx_utility_rectangle_define(&display_size, 0, 0,
                                (GX_VALUE)(display->gx_display_width - 1),
                                (GX_VALUE)(display->gx_display_height - 1));
    copy_clip = *copy;

    /** Is color format RGB565? */
    if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
    {
        /** If yes, align copy region on 32-bit boundary. */
        copy_clip.gx_rectangle_left  = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_left & 0xfffeU);
        copy_clip.gx_rectangle_right = (GX_VALUE)((USHORT)copy_clip.gx_rectangle_right | 1U);
    }

    /** Offset canvas within frame buffer. */
    gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    /** Return if copy width or height was invalid. */
    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (ULONG *) visible_frame;
    pPutRow = (ULONG *) working_frame;

    /** Calculate copy width, canvas stride, source and destination pointer for data copy. */
    if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
    {
        copy_width /= 2;
        canvas_stride = canvas->gx_canvas_x_resolution / 2;
        pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_top * canvas_stride);
        pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_left / 2);

        display_stride = display->gx_display_width / 2;
        pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * display_stride);
        pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left) / 2);
    }
    else
    {
        canvas_stride = canvas->gx_canvas_x_resolution;
        pPutRow = pPutRow + ((INT)copy_clip.gx_rectangle_top * canvas_stride);
        pPutRow = pPutRow + (INT)copy_clip.gx_rectangle_left;

        display_stride = display->gx_display_width;
        pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * display_stride);
        pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);
    }

    /** Copy data by software. */
    for (row = 0; row < copy_height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;

        for (col = 0; col < copy_width; col++)
        {
            *pPut++ = *pGet++;
        }

        pPutRow += canvas_stride;
        pGetRow += display_stride;
    }
}

/*******************************************************************************************************************/
static VOID gx_flip_canvas_to_working_16bpp_draw(USHORT *pGetRow, USHORT *pPutRow, INT width,
                                                                                   INT height, INT stride)
{
    USHORT     *pGet;
    USHORT     *pPut;

    for (INT row = 0; row < height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;
        for (INT col = 0; col < width; col++)
        {
            *pPut = *pGet;
            --pPut;
            ++pGet;
        }
        pGetRow += stride;
        pPutRow -= stride;
    }
}



/*******************************************************************************************************************/
static VOID gx_flip_canvas_to_working_16bpp(GX_CANVAS  *canvas, GX_RECTANGLE *copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;

    USHORT     *pGetRow;
    USHORT     *pPutRow;

    INT          copy_width;
    INT          copy_height;
    INT          canvas_stride;

    GX_DISPLAY *display = canvas->gx_canvas_display;
    gx_utility_rectangle_define(&display_size, 0, 0,
                                (GX_VALUE)(display->gx_display_width - 1),
                                (GX_VALUE)(display->gx_display_height - 1));
    copy_clip = *copy;

    /** Offset canvas within frame buffer. */
    gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (USHORT *) canvas->gx_canvas_memory;
    pPutRow = (USHORT *) working_frame;

    canvas_stride = canvas->gx_canvas_x_resolution;

    pPutRow = pPutRow + (INT)((display->gx_display_height - copy_clip.gx_rectangle_top) * canvas_stride);
    pPutRow = pPutRow - (INT)(copy_clip.gx_rectangle_left + 1);

    pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * canvas_stride);
    pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

    gx_flip_canvas_to_working_16bpp_draw(pGetRow, pPutRow, copy_width, copy_height, canvas_stride);
}



/*******************************************************************************************************************/
static VOID gx_flip_canvas_to_working_32bpp_draw(ULONG *pGetRow, ULONG *pPutRow, INT width,
                                                                                            INT height, INT stride)
{
    ULONG       *pGet;
    ULONG       *pPut;

    for (INT row = 0; row < height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;
        for (INT col = 0; col < width; col++)
        {
            *pPut = *pGet;
            --pPut;
            ++pGet;
        }
        pGetRow += stride;
        pPutRow -= stride;
    }
}



/*******************************************************************************************************************/
static VOID gx_flip_canvas_to_working_32bpp(GX_CANVAS  *canvas, GX_RECTANGLE * copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;

    ULONG       *pGetRow;
    ULONG       *pPutRow;

    INT          copy_width;
    INT          copy_height;
    INT          canvas_stride;

    GX_DISPLAY *display = canvas->gx_canvas_display;
    gx_utility_rectangle_define(&display_size, 0, 0,
                                (GX_VALUE)(display->gx_display_width - 1),
                                (GX_VALUE)(display->gx_display_height - 1));
    copy_clip = *copy;

    /** Offset canvas within frame buffer. */
    gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = ((copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1);
    copy_height = (copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (ULONG *) canvas->gx_canvas_memory;
    pPutRow = (ULONG *) working_frame;

    canvas_stride  = (INT)canvas->gx_canvas_x_resolution;

    pPutRow = pPutRow + ((INT)(display->gx_display_height - copy_clip.gx_rectangle_top) * canvas_stride);
    pPutRow = pPutRow - (INT)(copy_clip.gx_rectangle_left + 1);

    pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) * canvas_stride);
    pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

    gx_flip_canvas_to_working_32bpp_draw(pGetRow, pPutRow, copy_width, copy_height, canvas_stride);
}



static void glcdc_initialize (void);
void qe_for_display_parameter_set (glcdc_cfg_t * glcdc_qe_cfg);
void glcdc_callback (void * pdata);
void board_port_setting (void);

glcdc_cfg_t gs_glcdc_init_cfg;                   /* Argument of R_GLCDC_Open function. */
volatile bool gs_first_interrupt_flag;           /* It is used for interrupt control of GLCDC module */

static void glcdc_initialize (void)
{
    glcdc_err_t ret;

    /* ==== QE for Display [RX] setting parameter ==== */
    qe_for_display_parameter_set( &gs_glcdc_init_cfg);

    /* ==== User setting parameter ==== */
    /* ---- Graphic 2 setting ---- */
    /* Image format */
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].p_base        = (uint32_t *) FRAME_BUF_BASE_ADDR_A;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].hsize         = IMAGE_WIDTH;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].vsize         = IMAGE_HEIGHT;
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].offset        = (IMAGE_WIDTH * IMAGE_PIXEL_SIZE);
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_2].format        = GLCDC_IN_FORMAT_16BITS_RGB565;
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
    //gs_glcdc_init_cfg.clut[GLCDC_FRAME_LAYER_2].enable = true;

    /* When enable is false, setting values of structure members under clut are ignored. */
    //gs_glcdc_init_cfg.clut[GLCDC_FRAME_LAYER_2].p_base = (uint32_t *) main_display_theme_1_palette;
    //gs_glcdc_init_cfg.clut[GLCDC_FRAME_LAYER_2].start  = 0;
    //gs_glcdc_init_cfg.clut[GLCDC_FRAME_LAYER_2].size   = 256;

    /* ---- Graphic 1 setting ---- */
    gs_glcdc_init_cfg.input[GLCDC_FRAME_LAYER_1].p_base = (uint32_t *) NULL; /* Graphic 1 is disable */

    /* When p_base is NULL, setting values of structure members under input[GLCDC_FRAME_LAYER_1] are ignored. */

    /* ---- Hard ware and other setting ---- */
    /* Back ground color */
    gs_glcdc_init_cfg.output.bg_color.argb = 0x00CCCCCC;

    /* Endian */
    gs_glcdc_init_cfg.output.endian = GLCDC_ENDIAN_LITTLE;

    /* Color order */
    gs_glcdc_init_cfg.output.color_order = GLCDC_COLOR_ORDER_RGB; /* RX72N Envision Kit board dependent setting */

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

} /* End of function glcdc_initialize() */


/* ---- Setting gamma parameters to be adjusted with QE for Display [RX] ---- */
/* Gamma Red data table */
static const gamma_correction_t gs_gamma_table_r =
{
    /* Gain (Adjusted value by QE) */
    { IMGC_GAMMA_R_GAIN_00, IMGC_GAMMA_R_GAIN_01, IMGC_GAMMA_R_GAIN_02, IMGC_GAMMA_R_GAIN_03,
      IMGC_GAMMA_R_GAIN_04, IMGC_GAMMA_R_GAIN_05, IMGC_GAMMA_R_GAIN_06, IMGC_GAMMA_R_GAIN_07,
      IMGC_GAMMA_R_GAIN_08, IMGC_GAMMA_R_GAIN_09, IMGC_GAMMA_R_GAIN_10, IMGC_GAMMA_R_GAIN_11,
      IMGC_GAMMA_R_GAIN_12, IMGC_GAMMA_R_GAIN_13, IMGC_GAMMA_R_GAIN_14, IMGC_GAMMA_R_GAIN_15 },

    /* Threshold (Adjusted value by QE) */
    { IMGC_GAMMA_R_TH_01, IMGC_GAMMA_R_TH_02, IMGC_GAMMA_R_TH_03, IMGC_GAMMA_R_TH_04,
      IMGC_GAMMA_R_TH_05, IMGC_GAMMA_R_TH_06, IMGC_GAMMA_R_TH_07, IMGC_GAMMA_R_TH_08,
      IMGC_GAMMA_R_TH_09, IMGC_GAMMA_R_TH_10, IMGC_GAMMA_R_TH_11, IMGC_GAMMA_R_TH_12,
      IMGC_GAMMA_R_TH_13, IMGC_GAMMA_R_TH_14, IMGC_GAMMA_R_TH_15 }
};

/* Gamma Green data table */
static const gamma_correction_t gs_gamma_table_g =
{
    /* Gain (Adjusted value by QE) */
    { IMGC_GAMMA_G_GAIN_00, IMGC_GAMMA_G_GAIN_01, IMGC_GAMMA_G_GAIN_02, IMGC_GAMMA_G_GAIN_03,
      IMGC_GAMMA_G_GAIN_04, IMGC_GAMMA_G_GAIN_05, IMGC_GAMMA_G_GAIN_06, IMGC_GAMMA_G_GAIN_07,
      IMGC_GAMMA_G_GAIN_08, IMGC_GAMMA_G_GAIN_09, IMGC_GAMMA_G_GAIN_10, IMGC_GAMMA_G_GAIN_11,
      IMGC_GAMMA_G_GAIN_12, IMGC_GAMMA_G_GAIN_13, IMGC_GAMMA_G_GAIN_14, IMGC_GAMMA_G_GAIN_15 },

    /* Threshold (Adjusted value by QE) */
    { IMGC_GAMMA_G_TH_01, IMGC_GAMMA_G_TH_02, IMGC_GAMMA_G_TH_03, IMGC_GAMMA_G_TH_04,
      IMGC_GAMMA_G_TH_05, IMGC_GAMMA_G_TH_06, IMGC_GAMMA_G_TH_07, IMGC_GAMMA_G_TH_08,
      IMGC_GAMMA_G_TH_09, IMGC_GAMMA_G_TH_10, IMGC_GAMMA_G_TH_11, IMGC_GAMMA_G_TH_12,
      IMGC_GAMMA_G_TH_13, IMGC_GAMMA_G_TH_14, IMGC_GAMMA_G_TH_15 }
};

/* Gamma Blue data table */
static const gamma_correction_t gs_gamma_table_b =
{
    /* Gain (Adjusted value by QE) */
    { IMGC_GAMMA_B_GAIN_00, IMGC_GAMMA_B_GAIN_01, IMGC_GAMMA_B_GAIN_02, IMGC_GAMMA_B_GAIN_03,
      IMGC_GAMMA_B_GAIN_04, IMGC_GAMMA_B_GAIN_05, IMGC_GAMMA_B_GAIN_06, IMGC_GAMMA_B_GAIN_07,
      IMGC_GAMMA_B_GAIN_08, IMGC_GAMMA_B_GAIN_09, IMGC_GAMMA_B_GAIN_10, IMGC_GAMMA_B_GAIN_11,
      IMGC_GAMMA_B_GAIN_12, IMGC_GAMMA_B_GAIN_13, IMGC_GAMMA_B_GAIN_14, IMGC_GAMMA_B_GAIN_15 },

    /* Threshold (Adjusted value by QE) */
    { IMGC_GAMMA_B_TH_01, IMGC_GAMMA_B_TH_02, IMGC_GAMMA_B_TH_03, IMGC_GAMMA_B_TH_04,
      IMGC_GAMMA_B_TH_05, IMGC_GAMMA_B_TH_06, IMGC_GAMMA_B_TH_07, IMGC_GAMMA_B_TH_08,
      IMGC_GAMMA_B_TH_09, IMGC_GAMMA_B_TH_10, IMGC_GAMMA_B_TH_11, IMGC_GAMMA_B_TH_12,
      IMGC_GAMMA_B_TH_13, IMGC_GAMMA_B_TH_14, IMGC_GAMMA_B_TH_15 }
};


/*******************************************************************************************************************/
void qe_for_display_parameter_set (glcdc_cfg_t *glcdc_qe_cfg)
{
    /* Output timing */
    glcdc_qe_cfg->output.htiming.front_porch = LCD_CH0_W_HFP;
    glcdc_qe_cfg->output.htiming.back_porch  = LCD_CH0_W_HBP;
    glcdc_qe_cfg->output.htiming.display_cyc = LCD_CH0_DISP_HW;
    glcdc_qe_cfg->output.htiming.sync_width  = LCD_CH0_W_HSYNC;

    glcdc_qe_cfg->output.vtiming.front_porch = LCD_CH0_W_VFP;
    glcdc_qe_cfg->output.vtiming.back_porch  = LCD_CH0_W_VBP;
    glcdc_qe_cfg->output.vtiming.display_cyc = LCD_CH0_DISP_VW;
    glcdc_qe_cfg->output.vtiming.sync_width  = LCD_CH0_W_VSYNC;

    /* Output format */
    glcdc_qe_cfg->output.format = LCD_CH0_OUT_FORMAT;

    /* Tcon polarity*/
    glcdc_qe_cfg->output.data_enable_polarity = LCD_CH0_TCON_POL_DE;
    glcdc_qe_cfg->output.hsync_polarity       = LCD_CH0_TCON_POL_HSYNC;
    glcdc_qe_cfg->output.vsync_polarity       = LCD_CH0_TCON_POL_VSYNC;

    /* Sync signal edge */
    glcdc_qe_cfg->output.sync_edge = LCD_CH0_OUT_EDGE;

    /* Output tcon pin */
    glcdc_qe_cfg->output.tcon_hsync = LCD_CH0_TCON_PIN_HSYNC;
    glcdc_qe_cfg->output.tcon_vsync = LCD_CH0_TCON_PIN_VSYNC;
    glcdc_qe_cfg->output.tcon_de    = LCD_CH0_TCON_PIN_DE;

    /* Correction circuit sequence */
    gs_glcdc_init_cfg.output.correction_proc_order = IMGC_OUTCTL_CALIB_ROUTE;

    /* Brightness */
    glcdc_qe_cfg->output.brightness.enable = IMGC_BRIGHT_OUTCTL_ACTIVE;
    glcdc_qe_cfg->output.brightness.r      = IMGC_BRIGHT_OUTCTL_OFFSET_R;
    glcdc_qe_cfg->output.brightness.g      = IMGC_BRIGHT_OUTCTL_OFFSET_G;
    glcdc_qe_cfg->output.brightness.b      = IMGC_BRIGHT_OUTCTL_OFFSET_B;

    /* Contrast */
    glcdc_qe_cfg->output.contrast.enable = IMGC_CONTRAST_OUTCTL_ACTIVE;
    glcdc_qe_cfg->output.contrast.r      = IMGC_CONTRAST_OUTCTL_GAIN_R;
    glcdc_qe_cfg->output.contrast.g      = IMGC_CONTRAST_OUTCTL_GAIN_G;
    glcdc_qe_cfg->output.contrast.b      = IMGC_CONTRAST_OUTCTL_GAIN_B;

    /* Gamma */
    glcdc_qe_cfg->output.gamma.enable = IMGC_GAMMA_ACTIVE;
    glcdc_qe_cfg->output.gamma.p_r    = (gamma_correction_t *) &gs_gamma_table_r;
    glcdc_qe_cfg->output.gamma.p_g    = (gamma_correction_t *) &gs_gamma_table_g;
    glcdc_qe_cfg->output.gamma.p_b    = (gamma_correction_t *) &gs_gamma_table_b;

    /* Dethering */
    glcdc_qe_cfg->output.dithering.dithering_on        = IMGC_DITHER_ACTIVE;
    glcdc_qe_cfg->output.dithering.dithering_mode      = IMGC_DITHER_MODE;
    glcdc_qe_cfg->output.dithering.dithering_pattern_a = IMGC_DITHER_2X2_PA;
    glcdc_qe_cfg->output.dithering.dithering_pattern_b = IMGC_DITHER_2X2_PB;
    glcdc_qe_cfg->output.dithering.dithering_pattern_c = IMGC_DITHER_2X2_PC;
    glcdc_qe_cfg->output.dithering.dithering_pattern_d = IMGC_DITHER_2X2_PD;

} /* End of function qe_for_display_parameter_set() */


/*******************************************************************************************************************/
void glcdc_callback (void * pdata)
{

    if (false == gs_first_interrupt_flag)
    {
        gs_first_interrupt_flag = true;

        /* Do nothing */
    }
    else
    {
        glcdc_callback_args_t * p_decode;
        p_decode = (glcdc_callback_args_t *) pdata;

        if (GLCDC_EVENT_GR1_UNDERFLOW == p_decode->event)
        {
             /* GR1 underflow , if necessary */
        }
        else if (GLCDC_EVENT_GR2_UNDERFLOW == p_decode->event)
        {
            /* GR2 underflow , if necessary */
        }
        else /* GLCDC_EVENT_LINE_DETECTION */
        {
            /* GR2 line detection , if necessary */
        }
    }

} /* End of function glcdc_callback() */


/*******************************************************************************************************************/
void board_port_setting (void)
{
    /* ---- Port setting ---- */
    /* LCD back light and display-on */
    PORT6.PODR.BIT.B7 = 1;  /* Back light */
    PORTB.PODR.BIT.B3 = 1;  /* Display */
    PORT6.PDR.BIT.B7  = 1;
    PORTB.PDR.BIT.B3  = 1;


} /* End of function board_port_setting() */


/*******************************************************************************************************************/
UINT gx_rx_graphics_driver_setup(GX_DISPLAY *display)
{
	// invoke Studio generated setup/configuration
	_gx_rx_display_driver_setup(display);

	visible_frame = (USHORT *) FRAME_BUF_BASE_ADDR_A;
	working_frame = (USHORT *) FRAME_BUF_BASE_ADDR_B;

    /* ---- Initialization of the GLCDC , and start display ---- */
    glcdc_initialize();

#if (GX_USE_DAVE2D_DRAW == 1)
    extern void drw_int_isr(void);

    R_BSP_InterruptWrite(BSP_INT_SRC_AL1_DRW2D_DRW_IRQ, (bsp_int_cb_t)drw_int_isr);
    display->gx_display_accelerator = d2_opendevice(0);
    d2_inithw(display->gx_display_accelerator, 0);

#endif

    return GX_SUCCESS;
}
