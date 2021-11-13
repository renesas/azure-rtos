/**********************************************************************************************************************
 *Includes
 **********************************************************************************************************************/
#include    <stdio.h>
#include    <string.h>

#include    "gx_api.h"
#include    "gx_display.h"
#include    "gx_utility.h"

#define RENESAS_RX

#if (GX_USE_DAVE2D_DRAW == 1)
 #include    "dave_driver.h"
#endif
#if (GX_USE_HARDWARE_JPEG == 1)
 #include    "r_jpeg.h"
#endif

#if defined(RENESAS_RX)
void rx_frame_pointers_get(ULONG display_handle, GX_UBYTE **pp_visible, GX_UBYTE **pp_working);
void rx_hardware_frame_toggle(GX_CANVAS *canvas);
#endif

/**********************************************************************************************************************
 *Macro definitions
 ***********************************************************************************************************************/
#ifndef GX_DISABLE_ERROR_CHECKING
#define LOG_DAVE_ERRORS

/*Max number of error values we will keep */
#define DAVE_ERROR_LIST_SIZE                         (4)
#endif

/* Space used to store int to fixed point polygon vertices. */
#define MAX_POLYGON_VERTICES                          GX_POLYGON_MAX_EDGE_NUM

/* Number of display list command issues to issue a display list flush */
#define GX_RENESAS_DRW_DL_COMMAND_COUNT_TO_REFRESH    (85)

#if defined(LOG_DAVE_ERRORS)

/* Macro to check for and log status code from Dave2D engine. */
#define CHECK_DAVE_STATUS(a)    gx_log_dave_error(a);
#else

/*here is error logging not enabled */
#define CHECK_DAVE_STATUS(a)    a;
#endif

/* Buffer alignment for JPEG draw */
#define JPEG_ALIGNMENT_8     (0x07U)
#define JPEG_ALIGNMENT_16    (0x0FU)
#define JPEG_ALIGNMENT_32    (0x1FU)

/**********************************************************************************************************************
 *Typedef definitions
 **********************************************************************************************************************/
#if (GX_USE_DAVE2D_DRAW == 1)

/* DAVE 2D screen rotation parameters */
typedef struct st_d2_rotation_param
{
    d2_border    xmin;
    d2_border    ymin;
    d2_border    xmax;
    d2_border    ymax;
    d2_point     x_texture_zero;
    d2_point     y_texture_zero;
    d2_s32       dxu;
    d2_s32       dxv;
    d2_s32       dyu;
    d2_s32       dyv;
    GX_VALUE     x_resolution;
    GX_VALUE     y_resolution;
    INT          copy_width;
    INT          copy_height;
    INT          copy_width_rotated;
    INT          copy_height_rotated;
    GX_RECTANGLE copy_clip;
    INT          angle;
} d2_rotation_param_t;

/* DAVE 2D rendering parameters */
typedef struct st_d2_render_param
{
    GX_BYTE      error_list_index;
    GX_BYTE      error_count;
    UCHAR        alpha;
    GX_BOOL      display_list_flushed;
    GX_RECTANGLE clip_rect;
    GX_COLOR     color0;
    GX_COLOR     color1;
    GX_FIXED_VAL line_width;
    INT          font_bits;
    GX_UBYTE     display_list_count;
    d2_u32       src_blend_mode;
    d2_u32       dst_blend_mode;
    d2_u32       render_mode;
    d2_u32       fill_mode;
    d2_u32       line_join;
    d2_u32       line_cap;
    d2_s32       aa_mode;
} d2_render_param_t;

#endif

#if (GX_USE_HARDWARE_JPEG == 1)

/* JPEG output streaming control parameters */
typedef struct st_jpeg_output_streaming_param
{
    jpeg_instance_t  *p_jpeg;
    GX_DRAW_CONTEXT  *p_context;
    GX_PIXELMAP      *p_pixelmap;
    UCHAR            *output_buffer;
    INT               jpeg_buffer_size;
    INT               x;
    INT               y;
    GX_VALUE          image_width;
    GX_VALUE          image_height;
    UINT              bytes_per_pixel;
} jpeg_output_streaming_param_t;
#endif

/* Data structure to save pixel color data for four adjacent pixels */
typedef struct st_pixelmap_adjacent_pixels
{
    GX_COLOR a;
    GX_COLOR b;
    GX_COLOR c;
    GX_COLOR d;
} pixelmap_adjacent_pixels_t;

/**********************************************************************************************************************
 *Private global variables
 **********************************************************************************************************************/

/* indicator for the number of visible frame buffer */
static GX_UBYTE *visible_frame = NULL;
static GX_UBYTE *working_frame = NULL;

#if (GX_USE_DAVE2D_DRAW == 1)
static GX_BOOL gx_dave2d_first_draw = GX_TRUE;

static d2_color (*gx_d2_color)(GX_COLOR color) = NULL;

/*Variable to hold last state of common rendering params and flags */
static d2_render_param_t g_dave2d = {0};

/* Partial palettes used for drawing 1bpp and 4bpp fonts */
static const d2_color g_mono_palette[2] =
{
    0x00ffffff,
    0xffffffff
};

static const d2_color g_gray_palette[16] =
{
    0x00ffffff,
    0x11ffffff,
    0x22ffffff,
    0x33ffffff,
    0x44ffffff,
    0x55ffffff,
    0x66ffffff,
    0x77ffffff,
    0x88ffffff,
    0x99ffffff,
    0xaaffffff,
    0xbbffffff,
    0xccffffff,
    0xddffffff,
    0xeeffffff,
    0xffffffff,
};

/*16-bit sine look-up table */
static const USHORT d2_sin_lut[90] =
{
    0x0000, 0x0477, 0x08EF, 0x0D65, 0x11DB, 0x164F, 0x1AC2, 0x1F32,
    0x23A0, 0x280C, 0x2C74, 0x30D8, 0x3539, 0x3996, 0x3DEE, 0x4241,
    0x4690, 0x4AD8, 0x4F1B, 0x5358, 0x578E, 0x5BBE, 0x5FE6, 0x6406,
    0x681F, 0x6C30, 0x7039, 0x7438, 0x782F, 0x7C1C, 0x7FFF, 0x83D9,
    0x87A8, 0x8B6D, 0x8F27, 0x92D5, 0x9679, 0x9A10, 0x9D9B, 0xA11B,
    0xA48D, 0xA7F3, 0xAB4C, 0xAE97, 0xB1D5, 0xB504, 0xB826, 0xBB39,
    0xBE3E, 0xC134, 0xC41B, 0xC6F3, 0xC9BB, 0xCC73, 0xCF1B, 0xD1B3,
    0xD43B, 0xD6B3, 0xD919, 0xDB6F, 0xDDB3, 0xDFE7, 0xE208, 0xE419,
    0xE617, 0xE803, 0xE9DE, 0xEBA6, 0xED5B, 0xEEFF, 0xF08F, 0xF20D,
    0xF378, 0xF4D0, 0xF615, 0xF746, 0xF865, 0xF970, 0xFA67, 0xFB4B,
    0xFC1C, 0xFCD9, 0xFD82, 0xFE17, 0xFE98, 0xFF06, 0xFF60, 0xFFA6,
    0xFFD8, 0xFFF6
};

 #if defined(LOG_DAVE_ERRORS)
static d2_s32 dave_error_list[DAVE_ERROR_LIST_SIZE] = {0};
 #endif
#endif                                 /*(GX_USE_DAVE2D_DRAW) */

#if (GX_USE_HARDWARE_JPEG == 1)
extern TX_SEMAPHORE  gx_renesas_jpeg_semaphore;
static jpeg_status_t g_jpeg_status;
#endif

/**********************************************************************************************************************
 *Private function prototypes
 **********************************************************************************************************************/
static VOID gx_flip_canvas_to_working_16bpp(GX_CANVAS  *canvas, GX_RECTANGLE *copy);
static VOID gx_flip_canvas_to_working_16bpp_draw(USHORT *pGetRow, USHORT *pPutRow, INT width, INT height,
                                                 INT stride);

static VOID gx_flip_canvas_to_working_32bpp(GX_CANVAS  *canvas, GX_RECTANGLE *copy);
static VOID     gx_flip_canvas_to_working_32bpp_draw(ULONG *pGetRow, ULONG*pPutRow, INT width, INT height,
                                                     INT stride);


#if (GX_USE_DAVE2D_DRAW == 1)
static d2_color gx_rgb565_to_888(GX_COLOR color);
static d2_color gx_xrgb_to_xrgb(GX_COLOR color);
static d2_s32   gx_dave2d_fast_sin(INT angle);
static d2_u32   gx_dave2d_format_set(GX_PIXELMAP *map);
static VOID     gx_dave2d_set_texture(GX_DRAW_CONTEXT *context, d2_device *dave, INT xpos, INT ypos,
                                      GX_PIXELMAP *map);
static VOID gx_dave2d_glyph_8bit_draw(GX_DRAW_CONTEXT *context,
                                      GX_RECTANGLE    *draw_area,
                                      GX_POINT        *map_offset,
                                      const GX_GLYPH  *glyph,
                                      d2_u32           mode);
static VOID gx_dave2d_glyph_4bit_draw(GX_DRAW_CONTEXT *context,
                                      GX_RECTANGLE    *draw_area,
                                      GX_POINT        *map_offset,
                                      const GX_GLYPH  *glyph,
                                      d2_u32           mode);
static VOID gx_dave2d_glyph_1bit_draw(GX_DRAW_CONTEXT *context,
                                      GX_RECTANGLE    *draw_area,
                                      GX_POINT        *map_offset,
                                      const GX_GLYPH  *glyph,
                                      d2_u32           mode);
static VOID gx_dave2d_copy_visible_to_working(GX_CANVAS *canvas, GX_RECTANGLE *copy);

static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_simple_rotate(GX_DRAW_CONTEXT *p_context,
                            INT xpos, INT ypos, GX_PIXELMAP *p_pixelmap, INT angle, INT cx, INT cy);
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate(GX_DRAW_CONTEXT *p_context,
                            INT xpos, INT ypos, GX_PIXELMAP *p_pixelmap, INT angle, INT cx, INT cy);
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get(
                            pixelmap_adjacent_pixels_t *pixels, GX_PIXELMAP *pixelmap, INT x, INT y);
static INT  gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_value_clip(INT value);
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_left_edge
                            (pixelmap_adjacent_pixels_t *pixels, GX_PIXELMAP *pixelmap, INT y);
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_top_edge
                            (pixelmap_adjacent_pixels_t *pixels, GX_PIXELMAP *pixelmap, INT x);
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_right_edge
                            (pixelmap_adjacent_pixels_t *pixels, GX_PIXELMAP *pixelmap, INT x, INT y);
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_bottom_edge
                            (pixelmap_adjacent_pixels_t *pixels, GX_PIXELMAP *pixelmap, INT x, INT y);
#endif

#if (GX_USE_HARDWARE_JPEG == 1)
void        _gx_renesas_jpeg_callback(jpeg_callback_args_t *p_args);
static INT  gx_renesas_jpeg_wait(jpeg_status_t *p_status, uint32_t timeout);
static INT  gx_renesas_jpeg_draw_open(GX_DRAW_CONTEXT *p_context, jpeg_instance_t *p_jpeg, UINT *p_bytes_per_pixel);
static VOID gx_renesas_jpeg_draw_output_streaming(jpeg_output_streaming_param_t *p_param);
static UINT gx_renesas_jpeg_draw_minimum_height_get(jpeg_color_space_t format, GX_VALUE width, GX_VALUE height);
static UINT gx_renesas_jpeg_draw_minimum_height_ycbcr422(GX_VALUE width, GX_VALUE height);
static UINT gx_renesas_jpeg_draw_minimum_height_ycbcr411(GX_VALUE width, GX_VALUE height);
static UINT gx_renesas_jpeg_draw_minimum_height_ycbcr420(GX_VALUE width, GX_VALUE height);
static UINT gx_renesas_jpeg_draw_minimum_height_ycbcr444(GX_VALUE width, GX_VALUE height);

static INT gx_renesas_jpeg_draw_output_streaming_wait(void);

#endif

/* functions shared in GUIX display driver files */
#if (GX_USE_DAVE2D_DRAW == 1)
 #if defined(LOG_DAVE_ERRORS)
VOID gx_log_dave_error(d2_s32 status);
INT  gx_get_dave_error(INT get_index);

 #endif
VOID        gx_dave2d_display_list_count(GX_DISPLAY *display);
GX_BOOL     gx_dave2d_convex_polygon_test(GX_POINT *vertex, INT num);
VOID        gx_display_list_flush(GX_DISPLAY *display);
VOID        gx_display_list_open(GX_DISPLAY *display);
VOID        gx_dave2d_cliprect_set(d2_device *p_dave, GX_RECTANGLE *clip);
d2_device *gx_dave2d_context_clip_set(GX_DRAW_CONTEXT *context);
VOID        gx_dave2d_outline_width_set(d2_device *dave, GX_FIXED_VAL width);
VOID        gx_dave2d_line_join_set(d2_device *dave, d2_u32 mode);
VOID        gx_dave2d_line_cap_set(d2_device *dave, d2_u32 mode);
VOID        gx_dave2d_color0_set(d2_device *dave, GX_COLOR color);
VOID        gx_dave2d_color1_set(d2_device *dave, GX_COLOR color);
VOID        gx_dave2d_alpha_set(d2_device *dave, UCHAR alpha);
VOID        gx_dave2d_fill_mode_set(d2_device *dave, d2_u32 mode);
VOID        gx_dave2d_blend_mode_set(d2_device *dave, d2_u32 srcmode, d2_u32 dstmode);
VOID        gx_dave2d_render_mode_set(d2_device *dave, d2_u32 mode);
VOID        gx_dave2d_anti_aliasing_set(d2_device *dave, d2_s32 mode);
VOID        gx_dave2d_glyph_palette_set(d2_device *dave, INT font_format);
VOID        gx_dave2d_pixelmap_palette_set(d2_device *dave, GX_PIXELMAP *map);
d2_u32      gx_dave2d_format_set(GX_PIXELMAP *map);


#endif

/* functions provided by rm_guix_port.c */
#if defined(RENESAS_RA)
extern void rm_guix_port_frame_toggle(ULONG display_handle, GX_UBYTE **visible);
extern void rm_guix_port_frame_pointers_get(ULONG display_handle, GX_UBYTE **visible, GX_UBYTE **working);
extern INT  rm_guix_port_display_rotation_get(ULONG display_handle);
extern void rm_guix_port_display_actual_size_get(ULONG display_handle, INT *width, INT *height);
#endif

#if (GX_USE_HARDWARE_JPEG == 1)
extern void *rm_guix_port_jpeg_buffer_get(ULONG display_handle, INT *memory_size);
extern void *rm_guix_port_jpeg_instance_get(ULONG display_handle);

#endif

/**********************************************************************************************************************
 *GUIX display driver function prototypes (called by GUIX)
 **********************************************************************************************************************/
VOID _gx_synergy_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty);

#if (GX_USE_HARDWARE_JPEG == 1)
VOID _gx_synergy_jpeg_draw(GX_DRAW_CONTEXT *p_context, INT x, INT y, GX_PIXELMAP *p_pixelmap);

#endif

#if (GX_USE_DAVE2D_DRAW == 1)
VOID _gx_dave2d_drawing_initiate(GX_DISPLAY *display, GX_CANVAS *canvas);
VOID _gx_dave2d_drawing_complete(GX_DISPLAY *display, GX_CANVAS *canvas);
VOID _gx_dave2d_horizontal_line(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos, INT width, GX_COLOR color);
VOID _gx_dave2d_vertical_line(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos, INT width, GX_COLOR color);
VOID _gx_dave2d_canvas_copy(GX_CANVAS *canvas, GX_CANVAS *composite);
VOID _gx_dave2d_canvas_blend(GX_CANVAS *canvas, GX_CANVAS *composite);
VOID _gx_dave2d_simple_line_draw(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend);
VOID _gx_dave2d_simple_wide_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend);
VOID _gx_dave2d_aliased_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend);
VOID _gx_dave2d_horizontal_pattern_line_draw_565(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos);
VOID _gx_dave2d_horizontal_pattern_line_draw_888(GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos);
VOID _gx_dave2d_vertical_pattern_line_draw_565(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos);
VOID _gx_dave2d_vertical_pattern_line_draw_888(GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos);
VOID _gx_dave2d_aliased_wide_line(GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend);
VOID _gx_dave2d_pixelmap_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap);
VOID _gx_dave2d_pixelmap_rotate_16bpp(GX_DRAW_CONTEXT *context,
                                      INT              xpos,
                                      INT              ypos,
                                      GX_PIXELMAP     *pixelmap,
                                      INT               angle,
                                      INT               rot_cx,
                                      INT               rot_cy);
VOID _gx_dave2d_pixelmap_blend(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha);
VOID _gx_dave2d_horizontal_pixelmap_line_draw(GX_DRAW_CONTEXT       *context,
                                              INT                     xstart,
                                              INT                     xend,
                                              INT                     y,
                                              GX_FILL_PIXELMAP_INFO *info);
VOID _gx_dave2d_polygon_draw(GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num);
VOID _gx_dave2d_polygon_fill(GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num);
VOID _gx_dave2d_pixel_write_565(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color);
VOID _gx_dave2d_pixel_write_888(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color);
VOID _gx_dave2d_pixel_blend_565(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
VOID _gx_dave2d_pixel_blend_888(GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha);
VOID _gx_dave2d_block_move(GX_DRAW_CONTEXT *context, GX_RECTANGLE *block, INT xshift, INT yshift);
VOID _gx_dave2d_alphamap_draw(GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap);
VOID _gx_dave2d_compressed_glyph_8bit_draw(GX_DRAW_CONTEXT *context,
                                           GX_RECTANGLE    *draw_area,
                                           GX_POINT        *map_offset,
                                           const GX_GLYPH  *glyph);
VOID _gx_dave2d_raw_glyph_8bit_draw(GX_DRAW_CONTEXT *context,
                                    GX_RECTANGLE    *draw_area,
                                    GX_POINT        *map_offset,
                                    const GX_GLYPH  *glyph);
VOID _gx_dave2d_compressed_glyph_4bit_draw(GX_DRAW_CONTEXT *context,
                                           GX_RECTANGLE    *draw_area,
                                           GX_POINT        *map_offset,
                                           const GX_GLYPH  *glyph);
VOID _gx_dave2d_raw_glyph_4bit_draw(GX_DRAW_CONTEXT *context,
                                    GX_RECTANGLE    *draw_area,
                                    GX_POINT        *map_offset,
                                    const GX_GLYPH  *glyph);
VOID _gx_dave2d_compressed_glyph_1bit_draw(GX_DRAW_CONTEXT *context,
                                           GX_RECTANGLE    *draw_area,
                                           GX_POINT        *map_offset,
                                           const GX_GLYPH  *glyph);
VOID _gx_dave2d_raw_glyph_1bit_draw(GX_DRAW_CONTEXT *context,
                                    GX_RECTANGLE    *draw_area,
                                    GX_POINT        *map_offset,
                                    const GX_GLYPH  *glyph);
VOID _gx_dave2d_aliased_circle_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r);
VOID _gx_dave2d_circle_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r);
VOID _gx_dave2d_circle_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r);
VOID _gx_dave2d_pie_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
VOID _gx_dave2d_aliased_arc_draw(GX_DRAW_CONTEXT *context,
                                 INT               xcenter,
                                 INT               ycenter,
                                 UINT              r,
                                 INT               start_angle,
                                 INT               end_angle);
VOID _gx_dave2d_arc_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
VOID _gx_dave2d_arc_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle);
VOID _gx_dave2d_aliased_ellipse_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b);
VOID _gx_dave2d_ellipse_draw(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b);
VOID _gx_dave2d_ellipse_fill(GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b);
VOID _gx_dave2d_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty);

#endif

/**********************************************************************************************************************
 *Functions
 ***********************************************************************************************************************/
#if (GX_USE_DAVE2D_DRAW == 1)

/******************************************************************************************************************//* 
 *@addtogroup RM_GUIX_PORT
 *@{
 **********************************************************************************************************************/

/******************************************************************************************************************//* 
 *@brief  Subroutine to define the width of geometry outlines.
 *@param  dave[in]            Pointer to dave device structure
 *@param  width[in]           Outline width in pixels
 **********************************************************************************************************************/
VOID gx_dave2d_outline_width_set (d2_device *dave, GX_FIXED_VAL width)
{
    if (g_dave2d.line_width != width)
    {
        CHECK_DAVE_STATUS(d2_outlinewidth(dave, (d2_width) ((ULONG) (D2_FIX4((ULONG) width)) >> GX_FIXED_VAL_SHIFT)))
        g_dave2d.line_width = width;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to specify polyline connection style. This is used while drawing the polygons.
 *@param  dave[in]            Pointer to dave device structure
 *@param  mode[in]            Linejoin mode
 **********************************************************************************************************************/
VOID gx_dave2d_line_join_set (d2_device *dave, d2_u32 mode)
{
    if (g_dave2d.line_join != mode)
    {
        CHECK_DAVE_STATUS(d2_setlinejoin(dave, mode))
        g_dave2d.line_join = mode;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to specify lineend style. This is used while drawing the lines.
 *@param  dave[in]            Pointer to dave device structure
 *@param  mode[in]            Linecap mode
 **********************************************************************************************************************/
VOID gx_dave2d_line_cap_set (d2_device *dave, d2_u32 mode)
{
    if (g_dave2d.line_cap != mode)
    {
        CHECK_DAVE_STATUS(d2_setlinecap(dave, mode))
        g_dave2d.line_cap = mode;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to set color registers. It set the color register with index 0. This is called when only one color
 *is used(when blending is not used).
 *@param  dave[in]            Pointer to dave device structure
 *@param  color[in]           24bit rgb color value
 **********************************************************************************************************************/
VOID gx_dave2d_color0_set (d2_device *dave, GX_COLOR color)
{
    if (g_dave2d.color0 != color)
    {
        CHECK_DAVE_STATUS(d2_setcolor(dave, 0, gx_d2_color(color)))
        g_dave2d.color0 = color;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to set color registers. It set the color register with index 1.
 *@param  dave[in]            Pointer to dave device structure
 *@param  color[in]           24bit rgb color value
 **********************************************************************************************************************/
VOID gx_dave2d_color1_set (d2_device *dave, GX_COLOR color)
{
    if (g_dave2d.color1 != color)
    {
        CHECK_DAVE_STATUS(d2_setcolor(dave, 1, gx_d2_color(color)))
        g_dave2d.color1 = color;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to set constant alpha value.
 *@param  dave[in]            Pointer to dave device structure
 *@param  alpha[in]           Alpha value (0 for transparent, 255 for opaque)
 **********************************************************************************************************************/
VOID gx_dave2d_alpha_set (d2_device *dave, UCHAR alpha)
{
    if (g_dave2d.alpha != alpha)
    {
        CHECK_DAVE_STATUS(d2_setalpha(dave, alpha))
        g_dave2d.alpha = alpha;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to set fill mode.
 *@param  dave[in]            Pointer to dave device structure
 *@param  mode[in]            fill mode(solid,patter,texture etc)
 **********************************************************************************************************************/
VOID gx_dave2d_fill_mode_set (d2_device *dave, d2_u32 mode)
{
    if (g_dave2d.fill_mode != mode)
    {
        CHECK_DAVE_STATUS(d2_setfillmode(dave, mode))
        g_dave2d.fill_mode = mode;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to set a rendering mode.
 *@param  dave[in]            Pointer to dave device structure
 *@param  mode[in]            rendering mode
 **********************************************************************************************************************/
VOID gx_dave2d_render_mode_set (d2_device *dave, d2_u32 mode)
{
    if (g_dave2d.render_mode != mode)
    {
        CHECK_DAVE_STATUS(d2_selectrendermode(dave, mode))
        g_dave2d.render_mode = mode;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to set a blending for the RGB channel. It defines how the RGB channels of new pixels (source) are
 *combined with already existing data in the framebuffer (destination).
 *@param  dave[in]            Pointer to dave device structure
 *@param  srcmode[in]         source blend factor
 *@param  dstmode[in]         destination blend factor
 **********************************************************************************************************************/
VOID gx_dave2d_blend_mode_set (d2_device *dave, d2_u32 srcmode, d2_u32 dstmode)
{
    if ((g_dave2d.src_blend_mode != srcmode) || (g_dave2d.dst_blend_mode != dstmode))
    {
        CHECK_DAVE_STATUS(d2_setblendmode(dave, srcmode, dstmode))
        g_dave2d.src_blend_mode = srcmode;
        g_dave2d.dst_blend_mode = dstmode;
    }
}

/******************************************************************************************************************//* 
 *@brief  Subroutine to globally enable or disable the anti aliasing.
 *@param  dave[in]            Pointer to dave device structure
 *@param  mode[in]            boolean value(0 = disable, 1 = enable)
 **********************************************************************************************************************/
VOID gx_dave2d_anti_aliasing_set (d2_device *dave, d2_s32 mode)
{
    if (g_dave2d.aa_mode != mode)
    {
        CHECK_DAVE_STATUS(d2_setantialiasing(dave, mode))
        g_dave2d.aa_mode = mode;
    }
}

/******************************************************************************************************************/
VOID gx_dave2d_glyph_palette_set(d2_device* dave, INT font_format)
{
    switch (font_format)
    {
    case GX_FONT_FORMAT_1BPP:
        if (g_dave2d.font_bits != 1)
        {
            /* set up a 2-entry alpha palette for drawing glyph */
            CHECK_DAVE_STATUS(d2_settexclut_part(dave, (d2_color*)g_mono_palette, 0, 2))
                g_dave2d.font_bits = 1;
        }
        break;

    case GX_FONT_FORMAT_4BPP:
        if (g_dave2d.font_bits != 4)
        {
            /* setup a 16 entry alpha palette for drawing glyph */
            CHECK_DAVE_STATUS(d2_settexclut_part(dave, (d2_color*)g_gray_palette, 0, 16))
                g_dave2d.font_bits = 4;
        }
        break;
    }
}

/******************************************************************************************************************/
VOID gx_dave2d_pixelmap_palette_set(d2_device* dave, GX_PIXELMAP* map)
{
    CHECK_DAVE_STATUS(d2_settexclut(dave, (d2_color*)map->gx_pixelmap_aux_data))
        g_dave2d.font_bits = 0;
}



/******************************************************************************************************************/
VOID _gx_dave2d_drawing_initiate (GX_DISPLAY *display, GX_CANVAS *canvas)
{
    d2_u32 mode = d2_mode_rgb565;
    GX_VALUE x_resolution;
    GX_VALUE y_resolution;

    if (GX_TRUE == gx_dave2d_first_draw)
    {
        d2_device *dave = display->gx_display_accelerator;

        /* Initialize dave2d rendering parameters. */
        memset(&g_dave2d, 0, sizeof(d2_render_param_t));
        g_dave2d.clip_rect.gx_rectangle_left   = -1;
        g_dave2d.clip_rect.gx_rectangle_top    = -1;
        g_dave2d.clip_rect.gx_rectangle_right  = -1;
        g_dave2d.clip_rect.gx_rectangle_bottom = -1;
        g_dave2d.src_blend_mode                = d2_bm_alpha;
        g_dave2d.dst_blend_mode                = d2_bm_one_minus_alpha;
        g_dave2d.display_list_flushed          = GX_FALSE;
        g_dave2d.display_list_count            = (GX_UBYTE) 0;

        /*Set default mode */
        CHECK_DAVE_STATUS(d2_setalphablendmode(dave, d2_bm_one, d2_bm_one_minus_alpha))
        CHECK_DAVE_STATUS(d2_setblendmode(dave, g_dave2d.src_blend_mode, g_dave2d.dst_blend_mode))
        CHECK_DAVE_STATUS(d2_outlinewidth(dave,
                                          (d2_width) ((ULONG) (D2_FIX4((ULONG) g_dave2d.line_width)) >>
                                                      GX_FIXED_VAL_SHIFT)))
        CHECK_DAVE_STATUS(d2_setlinejoin(dave, g_dave2d.line_join))
        CHECK_DAVE_STATUS(d2_setlinecap(dave, g_dave2d.line_cap))
        CHECK_DAVE_STATUS(d2_setcolor(dave, 0, g_dave2d.color0))
        CHECK_DAVE_STATUS(d2_setcolor(dave, 1, g_dave2d.color1))
        CHECK_DAVE_STATUS(d2_setalpha(dave, g_dave2d.alpha))
        CHECK_DAVE_STATUS(d2_setfillmode(dave, g_dave2d.fill_mode))
        CHECK_DAVE_STATUS(d2_selectrendermode(dave, g_dave2d.render_mode))
        CHECK_DAVE_STATUS(d2_setantialiasing(dave, g_dave2d.aa_mode))

        gx_dave2d_first_draw = GX_FALSE;
    }

    switch ((INT) display->gx_display_color_format)
    {
        case GX_COLOR_FORMAT_565RGB:
        {
            mode        = d2_mode_rgb565;
            gx_d2_color = gx_rgb565_to_888;
            break;
        }

        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
        {
            mode        = d2_mode_argb8888;
            gx_d2_color = gx_xrgb_to_xrgb;
            break;
        }

        default:                       /*including GX_COLOR_FORMAT_8BIT_PALETTE */
        {
            mode        = d2_mode_alpha8;
            gx_d2_color = gx_xrgb_to_xrgb;
            break;
        }
    }

    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(display);

    /* Open next display list before we go. */
    gx_display_list_open(display);

    if (display->gx_display_rotation_angle == 0 ||
        display->gx_display_rotation_angle == 180)
    {
        x_resolution = canvas->gx_canvas_x_resolution;
        y_resolution = canvas->gx_canvas_y_resolution;
    }
    else
    {
        x_resolution = canvas->gx_canvas_y_resolution;
        y_resolution = canvas->gx_canvas_x_resolution;
    }

    CHECK_DAVE_STATUS(d2_framebuffer(display->gx_display_accelerator, canvas->gx_canvas_memory,
                                     (d2_s32) (x_resolution),
                                     (d2_u32) (x_resolution),
                                     (d2_u32) (y_resolution), (d2_s32) mode))
}

/******************************************************************************************************************/
VOID _gx_dave2d_drawing_complete (GX_DISPLAY *display, GX_CANVAS *canvas)
{
    GX_PARAMETER_NOT_USED(display);

    GX_PARAMETER_NOT_USED(canvas);
}

/******************************************************************************************************************/
VOID _gx_dave2d_canvas_copy (GX_CANVAS *canvas, GX_CANVAS *composite)
{
    d2_u32       mode;
    GX_DISPLAY *display = canvas->gx_canvas_display;
    d2_device  *dave    = display->gx_display_accelerator;

    gx_dave2d_cliprect_set(dave, &composite->gx_canvas_dirty_area);

    switch ((INT) display->gx_display_color_format)
    {
        case GX_COLOR_FORMAT_565RGB:
            mode = d2_mode_rgb565;
            break;

        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
            mode = d2_mode_argb8888;
            break;

        case GX_COLOR_FORMAT_8BIT_PALETTE:
            mode = d2_mode_alpha8;
            break;

        default:
            return;
    }

    /* Set the alpha blend value to opaque. */
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) canvas->gx_canvas_memory, canvas->gx_canvas_x_resolution,
                                    canvas->gx_canvas_x_resolution, canvas->gx_canvas_y_resolution, mode))

    CHECK_DAVE_STATUS(d2_blitcopy(dave, canvas->gx_canvas_x_resolution, canvas->gx_canvas_y_resolution, 0, 0,
                                  (d2_width) (D2_FIX4((USHORT) canvas->gx_canvas_x_resolution)),
                                  (d2_width) (D2_FIX4((USHORT) canvas->gx_canvas_y_resolution)),
                                  (d2_point) (D2_FIX4((USHORT) canvas->gx_canvas_display_offset_x)),
                                  (d2_point) (D2_FIX4((USHORT) canvas->gx_canvas_display_offset_y)),
                                  d2_bf_no_blitctxbackup))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_canvas_blend (GX_CANVAS *canvas, GX_CANVAS *composite)
{
    d2_u32       mode;
    GX_DISPLAY *display = canvas->gx_canvas_display;
    d2_device  *dave    = display->gx_display_accelerator;

    gx_dave2d_cliprect_set(dave, &composite->gx_canvas_dirty_area);

    switch ((INT) display->gx_display_color_format)
    {
        case GX_COLOR_FORMAT_565RGB:
        {
            mode = d2_mode_rgb565;
            break;
        }

        case GX_COLOR_FORMAT_24XRGB:
        case GX_COLOR_FORMAT_32ARGB:
        {
            mode = d2_mode_argb8888;
            break;
        }

        case GX_COLOR_FORMAT_8BIT_PALETTE:
        {
            mode = d2_mode_alpha8;
            break;
        }

        default:

            return;
    }

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) canvas->gx_canvas_memory, canvas->gx_canvas_x_resolution,
                                    canvas->gx_canvas_x_resolution, canvas->gx_canvas_y_resolution, mode))

    /* Set the alpha blend value as specified. */
    gx_dave2d_alpha_set(dave, canvas->gx_canvas_alpha);

    CHECK_DAVE_STATUS(d2_blitcopy(dave, canvas->gx_canvas_x_resolution, canvas->gx_canvas_y_resolution, 0, 0,
                                  (d2_width) (D2_FIX4((USHORT) canvas->gx_canvas_x_resolution)),
                                  (d2_width) (D2_FIX4((USHORT) canvas->gx_canvas_y_resolution)),
                                  (d2_point) (D2_FIX4((USHORT) canvas->gx_canvas_display_offset_x)),
                                  (d2_point) (D2_FIX4((USHORT) canvas->gx_canvas_display_offset_y)),
                                  d2_bf_no_blitctxbackup))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_horizontal_line (GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos, INT width, GX_COLOR color)
{
    d2_device *dave = gx_dave2d_context_clip_set(context);

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    gx_dave2d_fill_mode_set(dave, d2_fm_color);
    gx_dave2d_color0_set(dave, color);
    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    CHECK_DAVE_STATUS(d2_renderbox(dave, (d2_point) (D2_FIX4((USHORT) xstart)), (d2_point) (D2_FIX4((USHORT) ypos)),
                                   (d2_point) (D2_FIX4((USHORT) ((xend - xstart) + 1))),
                                   (d2_point) (D2_FIX4((USHORT) width))))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_vertical_line (GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos, INT width, GX_COLOR color)
{
    d2_device *dave = gx_dave2d_context_clip_set(context);

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    gx_dave2d_fill_mode_set(dave, d2_fm_color);
    gx_dave2d_color0_set(dave, color);
    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    CHECK_DAVE_STATUS(d2_renderbox(dave, (d2_point) (D2_FIX4((USHORT) xpos)), (d2_point) (D2_FIX4((USHORT) ystart)),
                                   (d2_point) (D2_FIX4((USHORT) width)),
                                   (d2_point) (D2_FIX4((USHORT) ((yend - ystart) + 1)))))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_simple_line_draw (GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device *dave = gx_dave2d_context_clip_set(context);

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    gx_dave2d_render_mode_set(dave, d2_rm_solid);
    gx_dave2d_anti_aliasing_set(dave, 0);
    gx_dave2d_color0_set(dave, context->gx_draw_context_brush.gx_brush_line_color);

    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point) (D2_FIX4((USHORT) xstart)), (d2_point) (D2_FIX4((USHORT) ystart)),
                                    (d2_point) (D2_FIX4((USHORT) xend)), (d2_point) (D2_FIX4((USHORT) yend)),
                                    (d2_width) (D2_FIX4((USHORT) context->gx_draw_context_brush.gx_brush_width)),
                                    d2_le_exclude_none))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_simple_wide_line (GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device *dave  = gx_dave2d_context_clip_set(context);
    GX_BRUSH  *brush = &context->gx_draw_context_brush;

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    /* Set a rendering mode to solid. */
    gx_dave2d_render_mode_set(dave, d2_rm_solid);
    gx_dave2d_anti_aliasing_set(dave, 0);
    gx_dave2d_color0_set(dave, context->gx_draw_context_brush.gx_brush_line_color);

    /* Set the lineend style based on brush style. */
    if (brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        gx_dave2d_line_cap_set(dave, d2_lc_round);
    }
    else
    {
        gx_dave2d_line_cap_set(dave, d2_lc_butt);
    }

    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point) (D2_FIX4((USHORT) xstart)), (d2_point) (D2_FIX4((USHORT) ystart)),
                                    (d2_point) (D2_FIX4((USHORT) xend)), (d2_point) (D2_FIX4((USHORT) yend)),
                                    (d2_width) (D2_FIX4((USHORT) brush->gx_brush_width)), d2_le_exclude_none))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_aliased_line (GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device *dave = gx_dave2d_context_clip_set(context);

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    /* Set a rendering mode to solid. */
    gx_dave2d_render_mode_set(dave, d2_rm_solid);
    gx_dave2d_anti_aliasing_set(dave, 1);
    gx_dave2d_color0_set(dave, context->gx_draw_context_brush.gx_brush_line_color);

    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point) (D2_FIX4((USHORT) xstart)), (d2_point) (D2_FIX4((USHORT) ystart)),
                                    (d2_point) (D2_FIX4((USHORT) xend)), (d2_point) (D2_FIX4((USHORT) yend)),
                                    (d2_width) (D2_FIX4((USHORT) context->gx_draw_context_brush.gx_brush_width)),
                                    d2_le_exclude_none))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_horizontal_pattern_line_draw_565 (GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    /* Call the GUIX generic 16bpp horizontal pattern line draw routine. */
    _gx_display_driver_16bpp_horizontal_pattern_line_draw(context, xstart, xend, ypos);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_horizontal_pattern_line_draw_888 (GX_DRAW_CONTEXT *context, INT xstart, INT xend, INT ypos)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    /* Call the GUIX generic 32bpp horizontal pattern line draw routine. */
    _gx_display_driver_32bpp_horizontal_pattern_line_draw(context, xstart, xend, ypos);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_vertical_pattern_line_draw_565 (GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    /* Call the GUIX generic 16bpp vertical pattern line draw routine. */
    _gx_display_driver_16bpp_vertical_pattern_line_draw(context, ystart, yend, xpos);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_vertical_pattern_line_draw_888 (GX_DRAW_CONTEXT *context, INT ystart, INT yend, INT xpos)
{
    gx_display_list_flush(context->gx_draw_context_display);

    /* Call the GUIX generic 32bpp vertical pattern line draw routine. */
    _gx_display_driver_32bpp_vertical_pattern_line_draw(context, ystart, yend, xpos);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_aliased_wide_line (GX_DRAW_CONTEXT *context, INT xstart, INT ystart, INT xend, INT yend)
{
    d2_device *dave  = gx_dave2d_context_clip_set(context);
    GX_BRUSH  *brush = &context->gx_draw_context_brush;

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    gx_dave2d_render_mode_set(dave, d2_rm_solid);
    gx_dave2d_anti_aliasing_set(dave, 1);
    gx_dave2d_color0_set(dave, context->gx_draw_context_brush.gx_brush_line_color);

    if (brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        gx_dave2d_line_cap_set(dave, d2_lc_round);
    }
    else
    {
        gx_dave2d_line_cap_set(dave, d2_lc_butt);
    }

    CHECK_DAVE_STATUS(d2_renderline(dave, (d2_point) (D2_FIX4((USHORT) xstart)), (d2_point) (D2_FIX4((USHORT) ystart)),
                                    (d2_point) (D2_FIX4((USHORT) xend)), (d2_point) (D2_FIX4((USHORT) yend)),
                                    (d2_width) (D2_FIX4((USHORT) brush->gx_brush_width)), d2_le_exclude_none))

    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************//* 
 *@brief  Sine function optimized for use with D/AVE 2D d2_s32 16:16 fixed point type.
 *@param   angle[in]         Angle in whole degrees
 **********************************************************************************************************************/
static d2_s32 gx_dave2d_fast_sin (INT angle)
{
    /*Put angle in 0-359 range */
    while (angle < 0)
    {
        angle += 360;
    }

    while (angle >= 360)
    {
        angle -= 360;
    }

    /*Get sign of result */
    LONG sign = 1;
    if (angle >= 180)
    {
        sign  *= -1;
        angle -= 180;
    }

    LONG sin_val;

    /*Handle multiples of 90 degrees quickly but retrieve all others from look-up table */
    if (!angle || (angle == 90))
    {
        sin_val = ((LONG) (angle > 0)) << 16;
    }
    else if (angle > 90)
    {
        sin_val = d2_sin_lut[180 - angle];
    }
    else
    {
        sin_val = d2_sin_lut[angle];
    }

    return (d2_s32) (sin_val *sign);
}

/******************************************************************************************************************/
VOID _gx_dave2d_pixelmap_rotate_16bpp (GX_DRAW_CONTEXT *context,
                                       INT              xpos,
                                       INT              ypos,
                                       GX_PIXELMAP     *pixelmap,
                                       INT              angle,
                                       INT              rot_cx,
                                       INT              rot_cy)
{
    GX_DISPLAY* display;

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        return;
    }

    display = context->gx_draw_context_display;

    angle = angle % 360;

    if (angle < 0)
    {
        angle += 360;
    }

    if (angle == 0)
    {
        if (display->gx_display_driver_pixelmap_draw)
        {
            display->gx_display_driver_pixelmap_draw(context, xpos, ypos, pixelmap);
            return;
        }
    }

    switch ((INT)pixelmap->gx_pixelmap_format)
    {
    case GX_COLOR_FORMAT_565RGB:
    case GX_COLOR_FORMAT_4444ARGB:
        _gx_display_driver_16bpp_pixelmap_rotate(context, xpos, ypos, pixelmap, angle, rot_cx, rot_cy);
        break;

    default:    /* GX_COLOR_FORMAT_24XRGB or GX_COLOR_FORMAT_32ARGB */
        if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
        {
            if ((angle % 90) == 0)
            {
                /* Simple angle rotate: 90 degree, 180 degree and 270 degree.  */
                gx_dave2d_display_driver_16bpp_32argb_pixelmap_simple_rotate(context, xpos, ypos, pixelmap,
                    angle, rot_cx, rot_cy);
                return;
            }
            else
            {
                gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate(context, xpos, ypos, pixelmap, angle,
                    rot_cx, rot_cy);
            }
        }
        break;
    }
}

/******************************************************************************************************************/
VOID _gx_dave2d_pixelmap_draw (GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
    d2_u32      mode;
    d2_device *dave;

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;

    if (brush_alpha == 0U)
    {
        return;
    }

    if ((GX_UBYTE) GX_ALPHA_VALUE_OPAQUE == brush_alpha)
    {
        dave = gx_dave2d_context_clip_set(context);
        gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
    }
    else
    {
        _gx_dave2d_pixelmap_blend(context, xpos, ypos, pixelmap, brush_alpha);
        return;
    }

 #else
    dave = gx_dave2d_context_clip_set(context);
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    mode = gx_dave2d_format_set(pixelmap);

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        mode |= d2_mode_rle;
    }

    if ((mode & d2_mode_clut) == d2_mode_clut)
    {
        CHECK_DAVE_STATUS(d2_settexclut(dave, (d2_color *) pixelmap->gx_pixelmap_aux_data))
    }

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) pixelmap->gx_pixelmap_data, pixelmap->gx_pixelmap_width,
                                    pixelmap->gx_pixelmap_width, pixelmap->gx_pixelmap_height, mode))

    mode = (d2_u32) d2_bf_no_blitctxbackup;

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
        mode |= (d2_u32) d2_bf_usealpha;
    }

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_TRANSPARENT)
    {
        mode |= (d2_u32) d2_bf_usealpha;
    }

    CHECK_DAVE_STATUS(d2_blitcopy(dave, pixelmap->gx_pixelmap_width, pixelmap->gx_pixelmap_height, 0, 0,
                                  (d2_width) (D2_FIX4((USHORT) pixelmap->gx_pixelmap_width)),
                                  (d2_width) (D2_FIX4((USHORT) pixelmap->gx_pixelmap_height)),
                                  (d2_point) (D2_FIX4((USHORT) xpos)),
                                  (d2_point) (D2_FIX4((USHORT) ypos)), mode))

    /* Check pixelmap is dynamically allocated. */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_DYNAMICALLY_ALLOCATED)
    {
        /* Flush D/AVE 2D display list first to insure order of operation. */
        gx_display_list_flush(context->gx_draw_context_display);

        /* Open next display list before we go. */
        gx_display_list_open(context->gx_draw_context_display);
    }
    else
    {
        /* Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);
    }
}

/******************************************************************************************************************/
VOID _gx_dave2d_pixelmap_blend (GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap, GX_UBYTE alpha)
{
    if (0U == alpha)
    {
        return;
    }

    d2_u32 mode = gx_dave2d_format_set(pixelmap);

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        mode |= (d2_u32) d2_mode_rle;
    }

    d2_device *dave = gx_dave2d_context_clip_set(context);

    if ((mode & (d2_u32) d2_mode_clut) == (d2_u32) d2_mode_clut)
    {
        d2_settexclut(dave, (d2_color *) pixelmap->gx_pixelmap_aux_data);
    }

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) pixelmap->gx_pixelmap_data, pixelmap->gx_pixelmap_width,
                                    pixelmap->gx_pixelmap_width, pixelmap->gx_pixelmap_height, mode))

    mode = (d2_u32) d2_bf_no_blitctxbackup;

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        mode |= (d2_u32) d2_bf_usealpha;
    }

    /* Set the alpha blend value as specified. */
    gx_dave2d_alpha_set(dave, alpha);

    /* Do the bitmap drawing. */
    CHECK_DAVE_STATUS(d2_blitcopy(dave, pixelmap->gx_pixelmap_width, pixelmap->gx_pixelmap_height, 0, 0,
                                  (d2_width) (D2_FIX4((USHORT) pixelmap->gx_pixelmap_width)),
                                  (d2_width) (D2_FIX4((USHORT) pixelmap->gx_pixelmap_height)),
                                  (d2_point) (D2_FIX4((USHORT) xpos)),
                                  (d2_point) (D2_FIX4((USHORT) ypos)), mode))

    /* Reset the alpha value. */
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, horizontal pixelmap line draw function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to draw a horizontal pixelmap.
 *@param   context[in]         Pointer to a GUIX draw context
 *@param   xpos[in]            x axis position of a pixelmap in pixel unit
 *@param   ypos[in]            y axis position of a pixelmap in pixel unit
 *@param   xstart[in]          x axis start position of a horizontal pixelmap line in pixel unit
 *@param   xend[in]            x axis end position of a horizontal pixelmap line in pixel unit
 *@param   y[in]               y axis position of a horizontal pixelmap line in pixel unit
 *@param   pixelmap[in]        Pointer to a pixelmap
 **********************************************************************************************************************/
VOID _gx_dave2d_horizontal_pixelmap_line_draw (GX_DRAW_CONTEXT       *context,
                                               INT                    xstart,
                                               INT                    xend,
                                               INT                    y,
                                               GX_FILL_PIXELMAP_INFO *info)
{
    GX_RECTANGLE *old_clip;
    GX_RECTANGLE   clip;
    GX_PIXELMAP  *pixelmap = info->pixelmap;
    INT            xpos;
    INT            ypos;

    if ((info->draw) && (xstart <= xend))
    {
        old_clip = context->gx_draw_context_clip;
        context->gx_draw_context_clip = &clip;

        xpos = xstart - (info->x_offset % pixelmap->gx_pixelmap_width);
        ypos = y - info->y_offset;

        clip.gx_rectangle_right  = (GX_VALUE) xend;
        clip.gx_rectangle_top    = (GX_VALUE) y;
        clip.gx_rectangle_bottom = (GX_VALUE) y;

        while (xstart <= xend)
        {
            clip.gx_rectangle_left = (GX_VALUE) xstart;

            /* Draw pixelmap. */
            _gx_dave2d_pixelmap_draw(context, xpos, ypos, pixelmap);

            xpos  += pixelmap->gx_pixelmap_width;
            xstart = xpos;
        }

        context->gx_draw_context_clip = old_clip;
    }

    info->y_offset++;
    if (info->y_offset >= pixelmap->gx_pixelmap_height)
    {
        info->y_offset = 0;
    }
}

/******************************************************************************************************************/
VOID _gx_dave2d_polygon_draw (GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num)
{
    INT        index = 0;
    GX_VALUE   val;
    d2_point   data[MAX_POLYGON_VERTICES *2] = {0};
    GX_BRUSH *brush       = &context->gx_draw_context_brush;
    GX_UBYTE   brush_alpha = brush->gx_brush_alpha;

    /* Return to caller if brush width is 0. */
    if (brush->gx_brush_width < 1)
    {
        return;
    }

    /* Convert incoming point data to d2_point type. */
    for (INT loop = 0; loop < num; loop++)
    {
        val           = vertex[loop].gx_point_x;
        data[index++] = (d2_point) (D2_FIX4((USHORT) val));
        val           = vertex[loop].gx_point_y;
        data[index++] = (d2_point) (D2_FIX4((USHORT) val));
    }

    d2_device *dave = gx_dave2d_context_clip_set(context);

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    gx_dave2d_outline_width_set(dave, (GX_FIXED_VAL) (((USHORT) brush->gx_brush_width << GX_FIXED_VAL_SHIFT)));
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    /* Enable or Disable anti-aliasing based on the brush style set. */
    if (brush->gx_brush_style & GX_BRUSH_ALIAS)
    {
        gx_dave2d_anti_aliasing_set(dave, 1);
    }
    else
    {
        gx_dave2d_anti_aliasing_set(dave, 0);
    }

    if (brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        gx_dave2d_line_join_set(dave, d2_lj_round);
    }
    else
    {
        gx_dave2d_line_join_set(dave, d2_lj_miter);
    }

    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    CHECK_DAVE_STATUS(d2_renderpolygon(dave, (d2_point *) data, (d2_u32) num, 0))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, polygon fill function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to fill a polygon.
 *@param   context[in]         Pointer to a GUIX draw context
 *@param   vertex[in]          Pointer to GUIX point data
 *@param   num[in]             Number of points
 **********************************************************************************************************************/
VOID _gx_dave2d_polygon_fill (GX_DRAW_CONTEXT *context, GX_POINT *vertex, INT num)
{
    /* Check if polygon to be render is convex polygon. */
    if (!gx_dave2d_convex_polygon_test(vertex, num))
    {
        /* Flush D/AVE 2D display list first to insure order of operation. */
        gx_display_list_flush(context->gx_draw_context_display);

        /* Open next display list before we go. */
        gx_display_list_open(context->gx_draw_context_display);

        /* Call the GUIX generic polygon fill function. */
        _gx_display_driver_generic_polygon_fill(context, vertex, num);

        return;
    }

    INT        index = 0;
    GX_VALUE   val;
    d2_point   data[MAX_POLYGON_VERTICES *2] = {0};
    GX_BRUSH *brush = &context->gx_draw_context_brush;

    d2_device *dave = gx_dave2d_context_clip_set(context);

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if (0U == brush_alpha)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    /* Convert incoming point data to d2_point type. */
    for (INT loop = 0; loop < num; loop++)
    {
        val           = vertex[loop].gx_point_x;
        data[index++] = (d2_point) (D2_FIX4((USHORT) val));
        val           = vertex[loop].gx_point_y;
        data[index++] = (d2_point) (D2_FIX4((USHORT) val));
    }

    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        gx_dave2d_fill_mode_set(dave, d2_fm_texture);
        gx_dave2d_set_texture(context,
                              dave,
                              context->gx_draw_context_clip->gx_rectangle_left,
                              context->gx_draw_context_clip->gx_rectangle_top,
                              brush->gx_brush_pixelmap);
    }
    else
    {
        gx_dave2d_fill_mode_set(dave, (d2_u32) d2_fm_color);
        gx_dave2d_color0_set(dave, brush->gx_brush_fill_color);
    }

    CHECK_DAVE_STATUS(d2_renderpolygon(dave, (d2_point *) data, (d2_u32) num, 0))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_pixel_write_565 (GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */

    gx_display_list_flush(context->gx_draw_context_display);

    /* Call the GUIX generic 16bpp pixel write routine. */
    _gx_display_driver_16bpp_pixel_write(context, x, y, color);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_pixel_write_888 (GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR color)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    /* Call the GUIX generic RGB888 pixel write routine. */
    _gx_display_driver_32bpp_pixel_write(context, x, y, color);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_pixel_blend_565 (GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    /* Call the GUIX generic RGB565 pixel blend routine. */
    _gx_display_driver_565rgb_pixel_blend(context, x, y, fcolor, alpha);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_pixel_blend_888 (GX_DRAW_CONTEXT *context, INT x, INT y, GX_COLOR fcolor, GX_UBYTE alpha)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    /* Call the GUIX generic 32ARGB pixel blend routine. */
    _gx_display_driver_32argb_pixel_blend(context, x, y, fcolor, alpha);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_block_move (GX_DRAW_CONTEXT *context, GX_RECTANGLE *block, INT xshift, INT yshift)
{
    d2_device *dave = gx_dave2d_context_clip_set(context);

    INT width  = (block->gx_rectangle_right - block->gx_rectangle_left) + 1;
    INT height = (block->gx_rectangle_bottom - block->gx_rectangle_top) + 1;

    CHECK_DAVE_STATUS(d2_utility_fbblitcopy(dave, (d2_u16) width, (d2_u16) height,
                                            (d2_blitpos) (block->gx_rectangle_left),
                                            (d2_blitpos) (block->gx_rectangle_top),
                                            (d2_blitpos) (block->gx_rectangle_left + xshift),
                                            (d2_blitpos) (block->gx_rectangle_top + yshift),
                                            d2_bf_no_blitctxbackup))
}

/******************************************************************************************************************/
VOID _gx_dave2d_alphamap_draw (GX_DRAW_CONTEXT *context, INT xpos, INT ypos, GX_PIXELMAP *pixelmap)
{
    d2_u32 mode;

    mode = d2_mode_alpha8;

    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        mode |= d2_mode_rle;
    }

    d2_device *dave = gx_dave2d_context_clip_set(context);

 #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
 #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
 #endif

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) pixelmap->gx_pixelmap_data, pixelmap->gx_pixelmap_width,
                                    pixelmap->gx_pixelmap_width, pixelmap->gx_pixelmap_height, mode))

    mode = (d2_u32) d2_bf_usealpha | (d2_u32) d2_bf_colorize | (d2_u32) d2_bf_no_blitctxbackup;

    gx_dave2d_color0_set(dave, context->gx_draw_context_brush.gx_brush_fill_color);

    CHECK_DAVE_STATUS(d2_blitcopy(dave, pixelmap->gx_pixelmap_width, pixelmap->gx_pixelmap_height, 0, 0,
                                  (d2_width) (D2_FIX4((USHORT) (pixelmap->gx_pixelmap_width))),
                                  (d2_width) (D2_FIX4((USHORT) (pixelmap->gx_pixelmap_height))),
                                  (d2_point) (D2_FIX4((USHORT) xpos)),
                                  (d2_point) (D2_FIX4((USHORT) ypos)), mode))

    /* Check pixelmap is dynamically allocated. */
    if (pixelmap->gx_pixelmap_flags & GX_PIXELMAP_DYNAMICALLY_ALLOCATED)
    {
        /* Flush D/AVE 2D display list first to insure order of operation. */
        gx_display_list_flush(context->gx_draw_context_display);

        /* Open next display list before we go. */
        gx_display_list_open(context->gx_draw_context_display);
    }
    else
    {
        /* Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);
    }
}

/******************************************************************************************************************/
VOID _gx_dave2d_compressed_glyph_8bit_draw (GX_DRAW_CONTEXT *context,
                                            GX_RECTANGLE    *draw_area,
                                            GX_POINT        *map_offset,
                                            const GX_GLYPH  *glyph)
{
    d2_u32                mode = 0;
    GX_COMPRESSED_GLYPH *compressed_glyph;

    compressed_glyph = (GX_COMPRESSED_GLYPH *) glyph;
    if (compressed_glyph->gx_glyph_map_size & 0x8000)
    {
        mode |= d2_mode_rle;
    }

    gx_dave2d_glyph_8bit_draw(context, draw_area, map_offset, glyph, mode);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, 8-bit raw glyph draw function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to draw 8-bit raw glyph.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   draw_area[in]      Pointer to the draw rectangle area
 *@param   map_offset[in]     Mapping offset
 *@param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
VOID _gx_dave2d_raw_glyph_8bit_draw (GX_DRAW_CONTEXT *context,
                                     GX_RECTANGLE    *draw_area,
                                     GX_POINT        *map_offset,
                                     const GX_GLYPH  *glyph)
{
    gx_dave2d_glyph_8bit_draw(context, draw_area, map_offset, glyph, 0);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, 4-bit compressed glyph draw function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to draw 4-bit compressed glyph.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   draw_area[in]      Pointer to the draw rectangle area
 *@param   map_offset[in]     Mapping offset
 *@param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
VOID _gx_dave2d_compressed_glyph_4bit_draw (GX_DRAW_CONTEXT *context,
                                            GX_RECTANGLE    *draw_area,
                                            GX_POINT        *map_offset,
                                            const GX_GLYPH  *glyph)
{
    d2_u32                mode = 0;
    GX_COMPRESSED_GLYPH *compressed_glyph;

    compressed_glyph = (GX_COMPRESSED_GLYPH *) glyph;
    if (compressed_glyph->gx_glyph_map_size & 0x8000)
    {
        mode |= d2_mode_rle;
    }

    gx_dave2d_glyph_4bit_draw(context, draw_area, map_offset, glyph, mode);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, 4-bit raw glyph draw function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to draw 4-bit raw glyph.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   draw_area[in]      Pointer to the draw rectangle area
 *@param   map_offset[in]     Mapping offset
 *@param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
VOID _gx_dave2d_raw_glyph_4bit_draw (GX_DRAW_CONTEXT *context,
                                     GX_RECTANGLE    *draw_area,
                                     GX_POINT        *map_offset,
                                     const GX_GLYPH  *glyph)
{
    gx_dave2d_glyph_4bit_draw(context, draw_area, map_offset, glyph, 0);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, 1-bit compressed glyph draw function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to draw 1-bit compressed glyph.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   draw_area[in]      Pointer to the draw rectangle area
 *@param   map_offset[in]     Mapping offset
 *@param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
VOID _gx_dave2d_compressed_glyph_1bit_draw (GX_DRAW_CONTEXT *context,
                                            GX_RECTANGLE    *draw_area,
                                            GX_POINT        *map_offset,
                                            const GX_GLYPH  *glyph)
{
    d2_u32                mode = 0;
    GX_COMPRESSED_GLYPH *compressed_glyph;

    compressed_glyph = (GX_COMPRESSED_GLYPH *) glyph;
    if (compressed_glyph->gx_glyph_map_size & 0x8000)
    {
        mode |= d2_mode_rle;
    }

    gx_dave2d_glyph_1bit_draw(context, draw_area, map_offset, glyph, mode);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, 1-bit raw glyph draw function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to draw 1-bit raw glyph.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   draw_area[in]      Pointer to the draw rectangle area
 *@param   map_offset[in]     Mapping offset
 *@param   glyph[in]          Pointer to glyph data
 **********************************************************************************************************************/
VOID _gx_dave2d_raw_glyph_1bit_draw (GX_DRAW_CONTEXT *context,
                                     GX_RECTANGLE    *draw_area,
                                     GX_POINT        *map_offset,
                                     const GX_GLYPH  *glyph)
{
    gx_dave2d_glyph_1bit_draw(context, draw_area, map_offset, glyph, 0);
}

 #if defined(GX_ARC_DRAWING_SUPPORT)

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, anti-aliased circle outline draw function with D/AVE 2D acceleration
 *enabled. This function is called by GUIX to render anti-aliased circle outline.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   xcenter[in]        Center pixel position in the horizontal axis
 *@param   ycenter[in]        Center pixel position in the vertical axis
 *@param   r[in]              Radius in pixel unit
 **********************************************************************************************************************/
VOID _gx_dave2d_aliased_circle_draw (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r)
{
    GX_BRUSH *brush = &context->gx_draw_context_brush;

    /* Return to caller if brush width is 0. */
    if (brush->gx_brush_width < 1)
    {
        return;
    }

    if (r < (UINT) ((brush->gx_brush_width + 1) / 2))
    {
        r = 0U;
    }
    else
    {
        r = (UINT) (r - (UINT) ((brush->gx_brush_width + 1) / 2));
    }

    d2_device *dave = gx_dave2d_context_clip_set(context);

  #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
  #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
  #endif

    // TODO:: Make a subfunction for this and the below function (and look for other areas where the same is happening)
    gx_dave2d_anti_aliasing_set(dave, 1);
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    gx_dave2d_outline_width_set(dave, (GX_FIXED_VAL) (((USHORT) brush->gx_brush_width << GX_FIXED_VAL_SHIFT)));
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    CHECK_DAVE_STATUS(d2_rendercircle(dave, (d2_point) (D2_FIX4((USHORT) xcenter)),
                                      (d2_point) (D2_FIX4((USHORT) ycenter)), (d2_width) (D2_FIX4((USHORT) r)), 0))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, aliased circle outline draw function with D/AVE 2D acceleration
 *enabled. This function is called by GUIX to render aliased circle outline.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   xcenter[in]        Center pixel position in the horizontal axis
 *@param   ycenter[in]        Center pixel position in the vertical axis
 *@param   r[in]              Radius in pixel unit
 **********************************************************************************************************************/
VOID _gx_dave2d_circle_draw (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r)
{
    GX_BRUSH *brush = &context->gx_draw_context_brush;

    /* Return to caller if brush width is 0. */
    if (brush->gx_brush_width < 1)
    {
        return;
    }

    if (r < (UINT) ((brush->gx_brush_width + 1) / 2))
    {
        r = 0U;
    }
    else
    {
        r = (UINT) (r - (UINT) ((brush->gx_brush_width + 1) / 2));
    }

    d2_device *dave = gx_dave2d_context_clip_set(context);

  #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
  #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
  #endif

    gx_dave2d_anti_aliasing_set(dave, 0);
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    gx_dave2d_outline_width_set(dave, (GX_FIXED_VAL) (((USHORT) brush->gx_brush_width << GX_FIXED_VAL_SHIFT)));
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    CHECK_DAVE_STATUS(d2_rendercircle(dave, (d2_point) (D2_FIX4((USHORT) xcenter)),
                                      (d2_point) (D2_FIX4((USHORT) ycenter)), (d2_width) (D2_FIX4((USHORT) r)), 0))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, circle fill function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to fill circle.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   xcenter[in]        Center pixel position in the horizontal axis
 *@param   ycenter[in]        Center pixel position in the vertical axis
 *@param   r[in]              Radius in pixel unit
 **********************************************************************************************************************/
VOID _gx_dave2d_circle_fill (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r)
{
    GX_BRUSH *brush       = &context->gx_draw_context_brush;
    GX_COLOR   brush_color = brush->gx_brush_fill_color;

    d2_device *dave = gx_dave2d_context_clip_set(context);

  #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
  #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
  #endif

    /* Enable or Disable anti-aliasing based on the brush style set. */
    if (brush->gx_brush_style & GX_BRUSH_ALIAS)
    {
        gx_dave2d_anti_aliasing_set(dave, 1);
    }
    else
    {
        gx_dave2d_anti_aliasing_set(dave, 0);
    }

    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        gx_dave2d_fill_mode_set(dave, d2_fm_texture);
        gx_dave2d_set_texture(context, dave, xcenter - (INT) r, ycenter - (INT) r, brush->gx_brush_pixelmap);
    }
    else
    {
        gx_dave2d_fill_mode_set(dave, d2_fm_color);
        gx_dave2d_color0_set(dave, brush_color);
    }

    CHECK_DAVE_STATUS(d2_rendercircle(dave, (d2_point) (D2_FIX4((USHORT) xcenter)),
                                      (d2_point) (D2_FIX4((USHORT) ycenter)), (d2_width) (D2_FIX4((USHORT) r)), 0))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, pie fill function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to fill pie.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   xcenter[in]        Center pixel position in the horizontal axis
 *@param   ycenter[in]        Center pixel position in the vertical axis
 *@param   r[in]              Radius in pixel unit
 *@param   start_angle[in]    Start angle in degree
 *@param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
VOID _gx_dave2d_pie_fill (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
    GX_BRUSH  *brush = &context->gx_draw_context_brush;
    INT         sin1;
    INT         cos1;
    INT         sin2;
    INT         cos2;
    d2_u32      flags;
    d2_device *dave = gx_dave2d_context_clip_set(context);

  #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = brush->gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
  #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
  #endif

    INT s_angle = -start_angle;
    INT e_angle = -end_angle;

    sin1 = (INT) _gx_utility_math_sin((GX_FIXED_VAL) ((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));
    cos1 = (INT) _gx_utility_math_cos((GX_FIXED_VAL) ((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));

    sin2 = (INT) _gx_utility_math_sin((GX_FIXED_VAL) ((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));
    cos2 = (INT) _gx_utility_math_cos((GX_FIXED_VAL) ((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));

    /* Set d2_wf_concave flag if the pie object to draw is concave shape. */
    if (((s_angle - e_angle) > 180) || ((s_angle - e_angle) < 0))
    {
        flags = d2_wf_concave;
    }
    else
    {
        flags = 0;
    }

    /* Enable or Disable anti-aliasing based on the brush style set. */
    if (brush->gx_brush_style & GX_BRUSH_ALIAS)
    {
        gx_dave2d_anti_aliasing_set(dave, 1);
    }
    else
    {
        gx_dave2d_anti_aliasing_set(dave, 0);
    }

    gx_dave2d_render_mode_set(dave, d2_rm_solid);

    if (brush->gx_brush_style & GX_BRUSH_PIXELMAP_FILL)
    {
        gx_dave2d_fill_mode_set(dave, d2_fm_texture);
        gx_dave2d_set_texture(context, dave, xcenter - (INT) r, ycenter - (INT) r, brush->gx_brush_pixelmap);
    }
    else
    {
        gx_dave2d_fill_mode_set(dave, d2_fm_color);
        gx_dave2d_color0_set(dave, brush->gx_brush_fill_color);
    }

    CHECK_DAVE_STATUS(d2_renderwedge(dave, (d2_point) (D2_FIX4((USHORT) xcenter)),
                                     (d2_point) (D2_FIX4((USHORT) ycenter)), (d2_width) (D2_FIX4((USHORT) (r + 1))), 0,
                                     (d2_s32) ((UINT) cos1 << 6),
                                     (d2_s32) ((UINT) sin1 << 6), (d2_s32) ((UINT) cos2 << 6),
                                     (d2_s32) ((UINT) sin2 << 6), flags))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, anti-aliased arc draw function with D/AVE 2D acceleration enabled.
 *This function is called by GUIX to draw anti-aliased arc.
 *@param   context[in]        Pointer to a GUIX draw context
 *@param   xcenter[in]        Center pixel position in the horizontal axis
 *@param   ycenter[in]        Center pixel position in the vertical axis
 *@param   r[in]              Radius in pixel unit
 *@param   start_angle[in]    Start angle in degree
 *@param   end_angle[in]      End angle in degree
 **********************************************************************************************************************/
VOID _gx_dave2d_aliased_arc_draw (GX_DRAW_CONTEXT *context,
                                  INT               xcenter,
                                  INT               ycenter,
                                  UINT              r,
                                  INT               start_angle,
                                  INT               end_angle)
{
    GX_BRUSH  *brush;
    INT         sin1;
    INT         cos1;
    INT         sin2;
    INT         cos2;
    d2_u32      flags;
    d2_device *dave;

    brush = &context->gx_draw_context_brush;

    if (brush->gx_brush_width < 1)
    {
        return;
    }

    dave = gx_dave2d_context_clip_set(context);

  #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = 0U;

    brush_alpha = brush->gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
  #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
  #endif

    INT s_angle = -start_angle;
    INT e_angle = -end_angle;

    sin1 = (INT) _gx_utility_math_sin((GX_FIXED_VAL) ((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));
    cos1 = (INT) _gx_utility_math_cos((GX_FIXED_VAL) ((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));

    sin2 = (INT) _gx_utility_math_sin((GX_FIXED_VAL) ((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));
    cos2 = (INT) _gx_utility_math_cos((GX_FIXED_VAL) ((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));

    /* Set d2_wf_concave flag if the pie object to draw is concave shape. */
    if (((s_angle - e_angle) > 180) || ((s_angle - e_angle) < 0))
    {
        flags = d2_wf_concave;
    }
    else
    {
        flags = 0;
    }

    gx_dave2d_anti_aliasing_set(dave, 1);
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    gx_dave2d_outline_width_set(dave,
                                (GX_FIXED_VAL) ((ULONG) ((USHORT) brush->gx_brush_width << GX_FIXED_VAL_SHIFT) >> 1));
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    CHECK_DAVE_STATUS(d2_renderwedge(dave, (d2_point) (D2_FIX4((USHORT) xcenter)),
                                     (d2_point) (D2_FIX4((USHORT) ycenter)), (d2_width) (D2_FIX4((USHORT) r)), 0,
                                     (d2_s32) ((UINT) cos1 << 6),
                                     (d2_s32) ((UINT) sin1 << 6), (d2_s32) ((UINT) cos2 << 6),
                                     (d2_s32) ((UINT) sin2 << 6), flags))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /* Check if line connection style is circle segment. */
    if (brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        UINT     brush_width = (UINT) brush->gx_brush_width;
        GX_POINT startp      = {0};
        GX_POINT endp        = {0};

        /* Set a rendering mode to solid. */
        gx_dave2d_render_mode_set(dave, d2_rm_solid);

        r = (UINT) (r - (brush_width >> 1));

        /* Get the point on circle with specified angle and radius. */
        _gx_utility_circle_point_get(xcenter, ycenter, r, start_angle, &startp);
        _gx_utility_circle_point_get(xcenter, ycenter, r + brush_width, start_angle, &endp);

        /* Render a circle. */
        CHECK_DAVE_STATUS(d2_rendercircle(dave,
                                          (d2_point) (D2_FIX4((USHORT) (startp.gx_point_x + endp.gx_point_x)) >> 1),
                                          (d2_point) (D2_FIX4((USHORT) (startp.gx_point_y + endp.gx_point_y)) >> 1),
                                          (d2_width) (D2_FIX4(brush_width) >> 1), 0))

        /* Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);

        /* Get the point on circle with specified angle and radius. */
        _gx_utility_circle_point_get(xcenter, ycenter, r, end_angle, &startp);
        _gx_utility_circle_point_get(xcenter, ycenter, r + brush_width, end_angle, &endp);

        /* Render a circle. */
        CHECK_DAVE_STATUS(d2_rendercircle(dave,
                                          (d2_point) (D2_FIX4((USHORT) (startp.gx_point_x + endp.gx_point_x)) >> 1),
                                          (d2_point) (D2_FIX4((USHORT) (startp.gx_point_y + endp.gx_point_y)) >> 1),
                                          (d2_width) (D2_FIX4(brush_width) >> 1), 0))

        /* Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);
    }
}

/******************************************************************************************************************/
VOID _gx_dave2d_arc_draw (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
    GX_BRUSH  *brush = &context->gx_draw_context_brush;
    INT         sin1;
    INT         cos1;
    INT         sin2;
    INT         cos2;
    d2_u32      flags;
    d2_device *dave;

    if (brush->gx_brush_width < 1)
    {
        return;
    }

    dave = gx_dave2d_context_clip_set(context);

  #if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE brush_alpha = 0U;

    brush_alpha = brush->gx_brush_alpha;
    if (brush_alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, brush_alpha);
  #else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
  #endif

    INT s_angle = -start_angle;
    INT e_angle = -end_angle;

    sin1 = (INT) _gx_utility_math_sin((GX_FIXED_VAL) ((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));
    cos1 = (INT) _gx_utility_math_cos((GX_FIXED_VAL) ((UINT) (s_angle - 90) << GX_FIXED_VAL_SHIFT));

    sin2 = (INT) _gx_utility_math_sin((GX_FIXED_VAL) ((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));
    cos2 = (INT) _gx_utility_math_cos((GX_FIXED_VAL) ((UINT) (e_angle + 90) << GX_FIXED_VAL_SHIFT));

    /* Set d2_wf_concave flag if the pie object to draw is concave shape. */
    if (((s_angle - e_angle) > 180) || ((s_angle - e_angle) < 0))
    {
        flags = d2_wf_concave;
    }
    else
    {
        flags = 0;
    }

    gx_dave2d_anti_aliasing_set(dave, 0);
    gx_dave2d_render_mode_set(dave, d2_rm_outline);
    gx_dave2d_outline_width_set(dave,
                                (GX_FIXED_VAL) ((ULONG) ((USHORT) brush->gx_brush_width << GX_FIXED_VAL_SHIFT) >> 1));
    gx_dave2d_color0_set(dave, brush->gx_brush_line_color);
    gx_dave2d_fill_mode_set(dave, d2_fm_color);

    CHECK_DAVE_STATUS(d2_renderwedge(dave, (d2_point) (D2_FIX4((USHORT) xcenter)),
                                     (d2_point) (D2_FIX4((USHORT) ycenter)), (d2_width) (D2_FIX4((USHORT) r)), 0,
                                     (d2_s32) ((UINT) cos1 << 6),
                                     (d2_s32) ((UINT) sin1 << 6), (d2_s32) ((UINT) cos2 << 6),
                                     (d2_s32) ((UINT) sin2 << 6), flags))

    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /* Check if line connection style is circle segment. */
    if (brush->gx_brush_style & GX_BRUSH_ROUND)
    {
        UINT     brush_width = (UINT) brush->gx_brush_width;
        GX_POINT startp      = {0};
        GX_POINT endp        = {0};

        /* Set a rendering mode to solid. */
        gx_dave2d_render_mode_set(dave, d2_rm_solid);

        r = (UINT) (r - (brush_width >> 1));

        /* Get the point on circle with specified angle and radius. */
        _gx_utility_circle_point_get(xcenter, ycenter, r, start_angle, &startp);
        _gx_utility_circle_point_get(xcenter, ycenter, r + brush_width, start_angle, &endp);

        /* Render a circle. */
        CHECK_DAVE_STATUS(d2_rendercircle(dave,
                                          (d2_point) (D2_FIX4((USHORT) (startp.gx_point_x + endp.gx_point_x)) >> 1),
                                          (d2_point) (D2_FIX4((USHORT) (startp.gx_point_y + endp.gx_point_y)) >> 1),
                                          (d2_width) (D2_FIX4(brush_width) >> 1), 0))

        /* Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);

        /* Gets the point on circle with specified angle and radius. */
        _gx_utility_circle_point_get(xcenter, ycenter, r, end_angle, &startp);
        _gx_utility_circle_point_get(xcenter, ycenter, r + brush_width, end_angle, &endp);

        /* Render a circle. */
        CHECK_DAVE_STATUS(d2_rendercircle(dave,
                                          (d2_point) (D2_FIX4((USHORT) (startp.gx_point_x + endp.gx_point_x)) >> 1),
                                          (d2_point) (D2_FIX4((USHORT) (startp.gx_point_y + endp.gx_point_y)) >> 1),
                                          (d2_width) (D2_FIX4(brush_width) >> 1), 0))

        /* Count the used display list size. */
        gx_dave2d_display_list_count(context->gx_draw_context_display);
    }
}

/******************************************************************************************************************/
VOID _gx_dave2d_arc_fill (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, UINT r, INT start_angle, INT end_angle)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);

    /* Call the GUIX generic arc fill routine. */
    _gx_display_driver_generic_arc_fill(context, xcenter, ycenter, r, start_angle, end_angle);
}

/******************************************************************************************************************/
VOID _gx_dave2d_aliased_ellipse_draw (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    if (context->gx_draw_context_brush.gx_brush_width > 1)
    {
        /* Call the GUIX generic anti-aliased ellipse draw routine if bruch width is more than 1. */
        _gx_display_driver_generic_aliased_wide_ellipse_draw(context, xcenter, ycenter, a, b);
    }
    else
    {
        /* Call the GUIX generic anti-aliased ellipse draw routine if bruch width is not more than 1. */
        _gx_display_driver_generic_aliased_ellipse_draw(context, xcenter, ycenter, a, b);
    }

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_ellipse_draw (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    if (context->gx_draw_context_brush.gx_brush_width > 1)
    {
        /* Call the GUIX generic aliased ellipse draw routine if bruch width is more than 1. */
        _gx_display_driver_generic_wide_ellipse_draw(context, xcenter, ycenter, a, b);
    }
    else
    {
        /* Call the GUIX generic aliased ellipse draw routine if bruch width is not more than 1. */
        _gx_display_driver_generic_ellipse_draw(context, xcenter, ycenter, a, b);
    }

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);
}

/******************************************************************************************************************/
VOID _gx_dave2d_ellipse_fill (GX_DRAW_CONTEXT *context, INT xcenter, INT ycenter, INT a, INT b)
{
    /* Flush D/AVE 2D display list first to insure order of operation. */
    gx_display_list_flush(context->gx_draw_context_display);

    /* Open next display list before we go. */
    gx_display_list_open(context->gx_draw_context_display);

    /* Call the GUIX generic ellipse fill routine. */
    _gx_display_driver_generic_ellipse_fill(context, xcenter, ycenter, a, b);
}

#endif                                /*is GUIX arc drawing support enabled? */

/******************************************************************************************************************/
VOID _gx_dave2d_buffer_toggle (GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{
    GX_PARAMETER_NOT_USED(dirty);

    GX_RECTANGLE Limit = {0};
    GX_RECTANGLE Copy  = {0};
    GX_DISPLAY *display;
    INT          rotation_angle;

    display = canvas->gx_canvas_display;

    rotation_angle = display->gx_display_rotation_angle;

    _gx_utility_rectangle_define(&Limit, 0, 0, (GX_VALUE) (canvas->gx_canvas_x_resolution - 1),
                                 (GX_VALUE) (canvas->gx_canvas_y_resolution - 1));


    gx_display_list_flush(display);
    gx_display_list_open(display);

    /* Wait till framebuffer writeback is busy. */
    CHECK_DAVE_STATUS(d2_flushframe(display->gx_display_accelerator))

    if (rotation_angle == 180)
     {
         if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
         {
             if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
             {
                 gx_flip_canvas_to_working_16bpp(canvas, &Copy);
             }
             else
             {
                 gx_flip_canvas_to_working_32bpp(canvas, &Copy);
             }
         }
     }

#if defined(RENESAS_RX)
	rx_hardware_frame_toggle(canvas);
	rx_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);
#else
    rm_guix_port_frame_toggle(display->gx_display_handle, &visible_frame);
    rm_guix_port_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);

#endif

    /* If canvas memory is pointing directly to frame buffer, toggle canvas memory. */
    if (canvas->gx_canvas_memory == (GX_COLOR*)visible_frame)
    {
        canvas->gx_canvas_memory = (GX_COLOR*)working_frame;
    }

    if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
    {
        /* Copies canvas memory or visible frame buffer to working frame buffer. */
        if (rotation_angle == 0)
        {
            gx_dave2d_copy_visible_to_working(canvas, &Copy);
        }
        else
        {
            if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
            {
                gx_flip_canvas_to_working_16bpp(canvas, &Copy);
            }
            else
            {
                gx_flip_canvas_to_working_32bpp(canvas, &Copy);
            }
        }
    }
}

#endif

#ifdef RENESAS_SYNERGY
VOID _gx_synergy_buffer_toggle(GX_CANVAS* canvas, GX_RECTANGLE* dirty)
{
    /*LDRA_INSPECTED 57 Statement with no side effect. */
    GX_PARAMETER_NOT_USED(dirty);

    GX_RECTANGLE Limit;
    GX_RECTANGLE Copy;
    GX_DISPLAY* display;
    INT rotation_angle;

    display = canvas->gx_canvas_display;

    _gx_utility_rectangle_define(&Limit, 0, 0,
        (GX_VALUE)(canvas->gx_canvas_x_resolution - 1),
        (GX_VALUE)(canvas->gx_canvas_y_resolution - 1));

    rotation_angle = display->gx_display_rotation_angle;

    sf_el_gx_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);

    if (rotation_angle == 180)
    {
        if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
        {
            if ((INT)display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
            {
                gx_flip_canvas_to_working_16bpp(canvas, &Copy);
            }
            else
            {
                gx_flip_canvas_to_working_32bpp(canvas, &Copy);
            }
        }
    }

    sf_el_gx_frame_toggle(canvas->gx_canvas_display->gx_display_handle, &visible_frame);
    sf_el_gx_frame_pointers_get(canvas->gx_canvas_display->gx_display_handle, &visible_frame, &working_frame);

    if (canvas->gx_canvas_memory == (GX_COLOR*)visible_frame)
    {
        canvas->gx_canvas_memory = (GX_COLOR*)working_frame;
    }

    if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
    {
        if (rotation_angle == 0)
        {
            gx_copy_visible_to_working(canvas, &Copy);
        }
        else
        {
            if ((INT)display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
            {
                gx_flip_canvas_to_working_16bpp(canvas, &Copy);
            }
            else
            {
                gx_flip_canvas_to_working_32bpp(canvas, &Copy);
            }
        }
    }
}
#endif

#if defined(RENESAS_RA)
VOID _gx_ra_buffer_toggle (GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{
    GX_PARAMETER_NOT_USED(dirty);

    GX_RECTANGLE Limit;
    GX_RECTANGLE Copy;
    GX_DISPLAY *display;
    INT          rotation_angle;

    display = canvas->gx_canvas_display;

    _gx_utility_rectangle_define(&Limit, 0, 0, (GX_VALUE) (canvas->gx_canvas_x_resolution - 1),
                                 (GX_VALUE) (canvas->gx_canvas_y_resolution - 1));

    rotation_angle = rm_guix_port_display_rotation_get(display->gx_display_handle);

    rm_guix_port_frame_pointers_get(display->gx_display_handle, &visible_frame, &working_frame);

    if (canvas->gx_canvas_memory != (GX_COLOR *) working_frame)
    {
        if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
        {
            if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
            {
                gx_rotate_canvas_to_working_16bpp(canvas, &Copy, rotation_angle);
            }
            else
            {
                gx_rotate_canvas_to_working_32bpp(canvas, &Copy, rotation_angle);
            }
        }
    }

    rm_guix_port_frame_toggle(canvas->gx_canvas_display->gx_display_handle, &visible_frame);
    rm_guix_port_frame_pointers_get(canvas->gx_canvas_display->gx_display_handle, &visible_frame, &working_frame);

    if (canvas->gx_canvas_memory == (GX_COLOR *) visible_frame)
    {
        canvas->gx_canvas_memory = (GX_COLOR *) working_frame;
    }

    if (_gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
    {
        if (canvas->gx_canvas_memory == (GX_COLOR *) working_frame)
        {
            gx_copy_visible_to_working(canvas, &Copy);
        }
        else
        {
            if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
            {
                gx_rotate_canvas_to_working_16bpp(canvas, &Copy, rotation_angle);
            }
            else
            {
                gx_rotate_canvas_to_working_32bpp(canvas, &Copy, rotation_angle);
            }
        }
    }
}
#endif

#if (GX_USE_HARDWARE_JPEG == 1)
/******************************************************************************************************************//
static INT gx_renesas_jpeg_wait (jpeg_status_t *p_status, uint32_t timeout)
{
    /*Pend on JPEG semaphore to wait for next event */
    INT ret = (INT) tx_semaphore_get(&gx_renesas_jpeg_semaphore, timeout);

    /*Get JPEG status */
    *p_status = g_jpeg_status;

    return ret;
}

/******************************************************************************************************************/
void _gx_renesas_jpeg_callback (jpeg_callback_args_t *p_args)
{
    g_jpeg_status = p_args->status;

    tx_semaphore_ceiling_put(&gx_renesas_jpeg_semaphore, 1U);
}

/******************************************************************************************************************/
VOID _gx_synergy_jpeg_draw (GX_DRAW_CONTEXT *p_context, INT x, INT y, GX_PIXELMAP *p_pixelmap)
{
    INT                           ret;
    jpeg_instance_t             *p_jpeg;
    jpeg_color_space_t            pixel_format    = JPEG_COLOR_SPACE_YCBCR422;
    jpeg_status_t                 jpeg_status     = JPEG_STATUS_NONE;
    jpeg_output_streaming_param_t param           = {0};
    UINT                          minimum_height  = 0;
    UINT                          memory_required = 0;

    /* Gets JPEG Framework driver instance.  */
    p_jpeg = (jpeg_instance_t *) rm_guix_port_jpeg_instance_get(p_context->gx_draw_context_display->gx_display_handle);

    /* Opens JPEG decode driver.  */
    ret = gx_renesas_jpeg_draw_open(p_context, p_jpeg, &param.bytes_per_pixel);

    /* Sets the input buffer address.  */
    ret += (INT) p_jpeg->p_api->inputBufferSet(p_jpeg->p_ctrl,
                                               (UCHAR *) p_pixelmap->gx_pixelmap_data,
                                               p_pixelmap->gx_pixelmap_data_size);

    /* Gets the JPEG hardware status.  */
    ret += gx_renesas_jpeg_wait(&jpeg_status, 1000);

    if ((ret) || (!((UINT) (JPEG_STATUS_IMAGE_SIZE_READY) &(UINT) jpeg_status)))
    {
        /*Nothing to draw. Close the device and return */
        p_jpeg->p_api->close(p_jpeg->p_ctrl);

        return;
    }

    /* Gets the size of JPEG image.  */
    ret +=
        (INT) p_jpeg->p_api->imageSizeGet(p_jpeg->p_ctrl,
                                          (uint16_t *) &param.image_width,
                                          (uint16_t *) &param.image_height);

    /* Gets the pixel format of JPEG image.  */
    ret += (INT) p_jpeg->p_api->pixelFormatGet(p_jpeg->p_ctrl, &pixel_format);

    if (ret)
    {
        /*Nothing to draw. Close the device and return */
        p_jpeg->p_api->close(p_jpeg->p_ctrl);

        return;
    }

    minimum_height = gx_renesas_jpeg_draw_minimum_height_get(pixel_format, param.image_width, param.image_height);
    if (0 == minimum_height)
    {
        p_jpeg->p_api->close(p_jpeg->p_ctrl);

        return;
    }

    memory_required = (UINT) (minimum_height *((UINT) param.image_width *param.bytes_per_pixel));

    param.output_buffer = rm_guix_port_jpeg_buffer_get(p_context->gx_draw_context_display->gx_display_handle,
                                                       &param.jpeg_buffer_size);

    /*Verify JPEG output buffer size meets minimum memory requirement. */
    if ((UINT) param.jpeg_buffer_size < memory_required)
    {
        p_jpeg->p_api->close(p_jpeg->p_ctrl);

        return;
    }

    /* Detects memory allocation errors. */
    if (param.output_buffer == GX_NULL)
    {
        /*If the output buffer is not allocated, nothing to be done but close the JPEG device and return. */
        p_jpeg->p_api->close(p_jpeg->p_ctrl);

        return;
    }

    /* Reject the buffer if it is not 8-byte aligned.*/
    if ((ULONG) param.output_buffer & 0x7)
    {
        /*Close the JPEG device and return. */
        p_jpeg->p_api->close(p_jpeg->p_ctrl);

        return;
    }

    /* Sets the horizontal stride. */
    p_jpeg->p_api->horizontalStrideSet(p_jpeg->p_ctrl, (uint32_t) param.image_width);

    /* Sets JPEG output streaming mode parameters.  */
    param.p_context  = p_context;
    param.p_pixelmap = p_pixelmap;
    param.p_jpeg     = p_jpeg;
    param.x          = x;
    param.y          = y;

    /* Decode JPEG encoded data in the JPEG decode output streaming mode.  */
    gx_renesas_jpeg_draw_output_streaming(&param);

    p_jpeg->p_api->close(p_jpeg->p_ctrl);
}                                      /*End of function _gx_driver_jpeg_draw() */

#endif /*(GX_USE_HARDWARE_JPEG)  */

/******************************************************************************************************************/

#if (GX_USE_DAVE2D_DRAW == 1)
#if defined(LOG_DAVE_ERRORS)

/******************************************************************************************************************/
VOID gx_log_dave_error (d2_s32 status)
{
    if (status)
    {
        dave_error_list[g_dave2d.error_list_index] = status;
        if (g_dave2d.error_count < DAVE_ERROR_LIST_SIZE)
        {
            g_dave2d.error_count++;
        }

        g_dave2d.error_list_index++;
        if (g_dave2d.error_list_index >= DAVE_ERROR_LIST_SIZE)
        {
            g_dave2d.error_list_index = 0;
        }
    }
}

/******************************************************************************************************************/
INT gx_get_dave_error (INT get_index)
{
    if (get_index > g_dave2d.error_count)
    {
        return 0;
    }

    INT list_index = g_dave2d.error_list_index;
    while (get_index > 0)
    {
        list_index--;
        if (list_index < 0)
        {
            list_index = DAVE_ERROR_LIST_SIZE;
        }

        get_index--;
    }

    return dave_error_list[list_index];
}

#endif                                 /*(LOG_DAVE_ERRORS) */


/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, close and execute current D/AVE 2D display list, block until completed.
 *This is a common function for GUIX D/AVE 2D draw routines.
 *@param   display         Pointer to a GUIX display context
 **********************************************************************************************************************/
VOID gx_display_list_flush (GX_DISPLAY *display)
{
    if ((GX_FALSE == g_dave2d.display_list_flushed) && (GX_TRUE == d2_commandspending(display->gx_display_accelerator)))
    {
        CHECK_DAVE_STATUS(d2_endframe(display->gx_display_accelerator))
        CHECK_DAVE_STATUS(d2_startframe(display->gx_display_accelerator))

        /*Wait till framebuffer writeback is busy. */
        CHECK_DAVE_STATUS(d2_flushframe(display->gx_display_accelerator))

        g_dave2d.display_list_flushed = GX_TRUE;
        g_dave2d.display_list_count   = (GX_UBYTE) 0;
    }
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, open D/AVE 2D display list for drawing commands.
 *This is a common function for GUIX D/AVE 2D draw routines.
 *@param   display         Pointer to a GUIX display context
 **********************************************************************************************************************/
VOID gx_display_list_open (GX_DISPLAY *display)
{
    GX_PARAMETER_NOT_USED(display);

    if (g_dave2d.display_list_flushed)
    {
        g_dave2d.display_list_flushed = GX_FALSE;
    }
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, count the used display list size.
 *This function is called by _gx_dave2d_horizontal_pixelmap_line_draw and _gx_dave2d_horizontal_line.
 *@param   display         Pointer to a GUIX display context
 **********************************************************************************************************************/
VOID gx_dave2d_display_list_count (GX_DISPLAY *display)
{
    g_dave2d.display_list_count++;

    if ((INT) g_dave2d.display_list_count > GX_RENESAS_DRW_DL_COMMAND_COUNT_TO_REFRESH)
    {
        /* Flush D/AVE 2D display list first to insure order of operation. */
        gx_display_list_flush(display);

        /* Open next display list before we go. */
        gx_display_list_open(display);
    }
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, test if polygon to be rendered is a convex polygon.
 *This function is called by _gx_dave2d_polygon_fill.
 *@param   vertex          Pointer to GUIX point data
 *@param   num             Number of points
 **********************************************************************************************************************/
GX_BOOL gx_dave2d_convex_polygon_test (GX_POINT *vertex, INT num)
{
    if (num <= 3)
    {
        return GX_TRUE;
    }

    GX_POINT a;
    GX_POINT b;
    GX_POINT c;
    GX_POINT b_a;
    GX_POINT c_b;
    GX_BOOL  negative;
    INT      index;

    a = vertex[0];
    b = vertex[1];
    c = vertex[2];

    b_a.gx_point_x = (GX_VALUE) (b.gx_point_x - a.gx_point_x);
    b_a.gx_point_y = (GX_VALUE) (b.gx_point_y - a.gx_point_y);

    c_b.gx_point_x = (GX_VALUE) (c.gx_point_x - b.gx_point_x);
    c_b.gx_point_y = (GX_VALUE) (c.gx_point_y - b.gx_point_y);

    negative = ((b_a.gx_point_x *c_b.gx_point_y) < (b_a.gx_point_y *c_b.gx_point_x));

    for (index = 3; index < num; index++)
    {
        a = b;
        b = c;
        c = vertex[index];

        b_a.gx_point_x = (GX_VALUE) (b.gx_point_x - a.gx_point_x);
        b_a.gx_point_y = (GX_VALUE) (b.gx_point_y - a.gx_point_y);

        c_b.gx_point_x = (GX_VALUE) (c.gx_point_x - b.gx_point_x);
        c_b.gx_point_y = (GX_VALUE) (c.gx_point_y - b.gx_point_y);

        if (((b_a.gx_point_x *c_b.gx_point_y) < (b_a.gx_point_y *c_b.gx_point_x)) != (INT) negative)
        {
            return GX_FALSE;
        }
    }

    return GX_TRUE;
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, assign clipping rectangle based on GUIX drawing clipping rectangle
 *information.
 *This is a common function for GUIX D/AVE 2D draw routines.
 *@param   dave         Pointer to D/AVE handle
 *@param   clip         Pointer to a GUIX clipping drawing rectangle
 **********************************************************************************************************************/
VOID gx_dave2d_cliprect_set (d2_device *dave, GX_RECTANGLE *clip)
{
    CHECK_DAVE_STATUS(d2_cliprect(dave, clip->gx_rectangle_left, clip->gx_rectangle_top, clip->gx_rectangle_right,
                                  clip->gx_rectangle_bottom))
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, assign clipping rectangle based on GUIX drawing context information.
 *This is a common function for GUIX D/AVE 2D draw routines.
 *@param   context         Pointer to a GUIX drawing context
 *@retval  Address             Pointer to a D/AVE 2D device structure
 **********************************************************************************************************************/
d2_device *gx_dave2d_context_clip_set (GX_DRAW_CONTEXT *context)
{
    d2_device *dave = context->gx_draw_context_display->gx_display_accelerator;

    if (context->gx_draw_context_clip)
    {
        CHECK_DAVE_STATUS(d2_cliprect(dave, context->gx_draw_context_clip->gx_rectangle_left,
                                      context->gx_draw_context_clip->gx_rectangle_top,
                                      context->gx_draw_context_clip->gx_rectangle_right,
                                      context->gx_draw_context_clip->gx_rectangle_bottom))
    }
    else
    {
        CHECK_DAVE_STATUS(d2_cliprect(dave, 0, 0,
                                      (d2_border) (context->gx_draw_context_canvas->gx_canvas_x_resolution - 1),
                                      (d2_border) (context->gx_draw_context_canvas->gx_canvas_y_resolution - 1)))
    }

    return dave;
}

/******************************************************************************************************************//* 
 *@brief  Utility function to convert color data from GUIX RGB565 to D/AVE 2D 24-bit RGB.
 *This function is called by _gx_dave2d_drawing_initiate().
 *@param   color           GUIX color data
 **********************************************************************************************************************/
static d2_color gx_rgb565_to_888 (GX_COLOR color)
{
    d2_color out_color;
    out_color = (((color & 0xf800) << 8) | ((color & 0x7e0) << 5) | ((color & 0x1f) << 3));

    return out_color;
}

/******************************************************************************************************************//* 
 *@brief  Utility function to convert color data from GUIX XRGB to D/AVE 2D XRGB.
 *This function is called by _gx_dave2d_drawing_initiate().
 *@param   color           GUIX color data
 **********************************************************************************************************************/
static d2_color gx_xrgb_to_xrgb (GX_COLOR color)
{
    d2_color out_color = (d2_color) color;

    return out_color;
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, D/AVE 2D pixelmap draw/blend sub-routine to select a D/AVE 2D color format
 *corresponding to the GUIX color format.
 *This function is called by _gx_dave2d_pixelmap_draw() or _gx_dave2d_pixelmap_blend.
 *@param   map             Pointer to GUIX pixelmap
 *@retval  format              D/AVE2D color format d2_mode_rgb565, d2_mode_argb4444, d2_mode_argb8888, d2_mode_alpha8
 *                             or (d2_mode_i8|d2_mode_clut)
 **********************************************************************************************************************/
static d2_u32 gx_dave2d_format_set (GX_PIXELMAP *map)
{
    d2_u32 format;

    switch (map->gx_pixelmap_format)
    {
        case (GX_UBYTE) GX_COLOR_FORMAT_565RGB:
        {
            format = d2_mode_rgb565;
            break;
        }

        case (GX_UBYTE) GX_COLOR_FORMAT_4444ARGB:
        {
            format = d2_mode_argb4444;
            break;
        }

        case (GX_UBYTE) GX_COLOR_FORMAT_24XRGB:
        case (GX_UBYTE) GX_COLOR_FORMAT_32ARGB:
        {
            format = d2_mode_argb8888;
            break;
        }

        case (GX_UBYTE) GX_COLOR_FORMAT_8BIT_ALPHAMAP:
        {
            format = d2_mode_alpha8;
            break;
        }

        default:                       /*GX_COLOR_FORMAT_8BIT_PALETTE */
        {
            format = d2_mode_i8 | d2_mode_clut;
            break;
        }
    }

    // TODO:: Put other mode settings (like RLE, CLUT) here

    return format;
}

/******************************************************************************************************************//* 
 *@brief  GUIX display driver for FSP, Support function used to apply texture source for all shape drawing.
 *This function is called by GUIX to draw a polygon.
 *@param   context         Pointer to a GUIX draw context
 *@param   dave            Pointer to D/AVE 2D device context
 *@param   xpos            X position in pixel unit
 *@param   ypos            y position in pixel unit
 *@param   map             Pointer to GUIX pixelmap
 **********************************************************************************************************************/
static VOID gx_dave2d_set_texture (GX_DRAW_CONTEXT *context, d2_device *dave, INT xpos, INT ypos, GX_PIXELMAP *map)
{
    GX_PARAMETER_NOT_USED(context);

    d2_u32 format = gx_dave2d_format_set(map);

    if ((format & (d2_u32) d2_mode_clut) == (d2_u32) d2_mode_clut)
    {
        CHECK_DAVE_STATUS(d2_settexclut(dave, (d2_color *) map->gx_pixelmap_aux_data))
    }

    if (map->gx_pixelmap_flags & GX_PIXELMAP_COMPRESSED)
    {
        format |= d2_mode_rle;
    }

    CHECK_DAVE_STATUS(d2_settexture(dave, (void *) map->gx_pixelmap_data, map->gx_pixelmap_width,
                                    map->gx_pixelmap_width, map->gx_pixelmap_height, format))

    /*Wrap texture over drawn area */
    CHECK_DAVE_STATUS(d2_settexturemode(dave, d2_tm_wrapu | d2_tm_wrapv))

    /*Set texture color processing */
    d2_u8 alpha_mode = d2_to_one;
    if (map->gx_pixelmap_flags & GX_PIXELMAP_ALPHA)
    {
        alpha_mode = d2_to_copy;
    }

    CHECK_DAVE_STATUS(d2_settextureoperation(dave, alpha_mode, d2_to_copy, d2_to_copy, d2_to_copy))

    /*Map texture to location */
    CHECK_DAVE_STATUS(d2_settexelcenter(dave, 0, 0))
    CHECK_DAVE_STATUS(d2_settexturemapping(dave,
                                           (d2_point) ((USHORT) xpos << 4),
                                           (d2_point) ((USHORT) ypos << 4), 0, 0,
                                           (d2_s32) (1U << 16), 0, 0, (d2_s32) (1U << 16)))
}

/******************************************************************************************************************/
static VOID gx_dave2d_glyph_8bit_draw (GX_DRAW_CONTEXT *context,
                                       GX_RECTANGLE    *draw_area,
                                       GX_POINT        *map_offset,
                                       const GX_GLYPH  *glyph,
                                       d2_u32            mode)
{
    d2_device *dave;
    GX_COLOR    text_color;

    /*pickup pointer to current display driver */
    dave = context->gx_draw_context_display->gx_display_accelerator;

    text_color = context->gx_draw_context_brush.gx_brush_line_color;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE alpha = 0U;

    alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if (alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_cliprect_set(dave, draw_area);
    gx_dave2d_color0_set(dave, text_color);
    gx_dave2d_blend_mode_set(dave, d2_bm_one, d2_bm_one_minus_alpha);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) glyph->gx_glyph_map, glyph->gx_glyph_width, glyph->gx_glyph_width,
                                    glyph->gx_glyph_height, mode | d2_mode_alpha8))

    CHECK_DAVE_STATUS(d2_blitcopy(dave, glyph->gx_glyph_width, glyph->gx_glyph_height,
                                  (d2_blitpos) (map_offset->gx_point_x), (d2_blitpos) (map_offset->gx_point_y),
                                  (d2_width) (D2_FIX4((USHORT) (glyph->gx_glyph_width))),
                                  (d2_width) (D2_FIX4((USHORT) (glyph->gx_glyph_height))),
                                  (d2_point) (D2_FIX4((USHORT) (draw_area->gx_rectangle_left))),
                                  (d2_point) (D2_FIX4((USHORT) (draw_area->gx_rectangle_top))),
                                  (d2_u32) d2_bf_usealpha | (d2_u32) d2_bf_colorize))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /*Return back alpha blend mode. */
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
}

/******************************************************************************************************************/
static VOID gx_dave2d_glyph_4bit_draw (GX_DRAW_CONTEXT *context,
                                       GX_RECTANGLE    *draw_area,
                                       GX_POINT        *map_offset,
                                       const GX_GLYPH  *glyph,
                                       d2_u32            mode)
{
    d2_device *dave;
    GX_COLOR    text_color;

    text_color = context->gx_draw_context_brush.gx_brush_line_color;
    dave       = context->gx_draw_context_display->gx_display_accelerator;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE alpha = 0U;

    alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if (alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_cliprect_set(dave, draw_area);

#if 1
    gx_dave2d_color0_set(dave, text_color);
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);

    if (g_dave2d.font_bits != 4)
    {
        /*setup a 16 entry alpha palette for drawing glyph */
        CHECK_DAVE_STATUS(d2_settexclut_part(dave, (d2_color *) g_gray_palette, 0, 16))
        g_dave2d.font_bits = 4;
    }

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) glyph->gx_glyph_map,
                                    (d2_s32) (((USHORT) glyph->gx_glyph_width + 1U) & 0xfffeU),
                                    (d2_s32) glyph->gx_glyph_width,
                                    (d2_s32) glyph->gx_glyph_height, (mode | d2_mode_i4 | d2_mode_clut)))
#else

    /*This version uses d2_mode_alpha4 instead of setting up a 16-entry palette,
     *however currently this blitsrc mode is not supported by D/AVE2D. Keep this code here in case this supported is
     *added in future versions.
     */
    gx_dave2d_color0_set(dave, text_color);
    gx_dave2d_blend_mode_set(dave, d2_bm_one, d2_bm_one_minus_alpha);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) glyph->gx_glyph_map,
                                    (d2_s32) (((USHORT) glyph->gx_glyph_width + 1U) & 0xfffeU), glyph->gx_glyph_width,
                                    glyph->gx_glyph_height,
                                    mode | d2_mode_alpha4))
#endif
    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                                  glyph->gx_glyph_width,
                                  glyph->gx_glyph_height,
                                  0,
                                  0,
                                  (d2_width) (D2_FIX4((USHORT) (glyph->gx_glyph_width))),
                                  (d2_width) (D2_FIX4((USHORT) (glyph->gx_glyph_height))),
                                  (d2_point) (D2_FIX4((USHORT) draw_area->gx_rectangle_left -
                                                      (USHORT) map_offset->gx_point_x)),
                                  (d2_point) (D2_FIX4((USHORT) draw_area->gx_rectangle_top -
                                                      (USHORT) map_offset->gx_point_y)),
                                  (d2_u32) d2_bf_colorize | (d2_u32) d2_bf_usealpha))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /* Set blend mode. */
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
}

/******************************************************************************************************************/
static VOID gx_dave2d_glyph_1bit_draw (GX_DRAW_CONTEXT *context,
                                       GX_RECTANGLE    *draw_area,
                                       GX_POINT        *map_offset,
                                       const GX_GLYPH  *glyph,
                                       d2_u32            mode)
{
    d2_device *dave;
    GX_COLOR    text_color;

    /* Pickup pointer to current display driver */
    dave = context->gx_draw_context_display->gx_display_accelerator;

    text_color = context->gx_draw_context_brush.gx_brush_line_color;

#if defined(GX_BRUSH_ALPHA_SUPPORT)
    GX_UBYTE alpha;

    alpha = context->gx_draw_context_brush.gx_brush_alpha;
    if (alpha == 0U)
    {
        return;
    }

    gx_dave2d_alpha_set(dave, alpha);
#else
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);
#endif

    gx_dave2d_cliprect_set(dave, draw_area);

#if 1
    if (g_dave2d.font_bits != 1)
    {
        /*set up a 2-entry alpha palette for drawing glyph */
        CHECK_DAVE_STATUS(d2_settexclut_part(dave, (d2_color *) g_mono_palette, 0, 2))
        g_dave2d.font_bits = 1;
    }

    gx_dave2d_color0_set(dave, text_color);
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) glyph->gx_glyph_map,
                                    (d2_s32) (((USHORT) glyph->gx_glyph_width + 7U) & 0xfff8U),
                                    (d2_s32) glyph->gx_glyph_width,
                                    (d2_s32) glyph->gx_glyph_height, (mode | d2_mode_i1 | d2_mode_clut)))
#else

    /*This version uses d2_mode_alpha1 instead of setting up a 16-entry palette,
     *however currently this blitsrc mode is not supported by D/AVE2D. Keep this code here in case this supported is
     *added in future versions.
     */
    gx_dave2d_color0_set(dave, text_color);
    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) glyph->gx_glyph_map, glyph->gx_glyph_width, glyph->gx_glyph_width,
                                    glyph->gx_glyph_height, mode | d2_mode_alpha1))
#endif

    CHECK_DAVE_STATUS(d2_blitcopy(dave,
                                  glyph->gx_glyph_width,
                                  glyph->gx_glyph_height,
                                  0,
                                  0,
                                  (d2_width) (D2_FIX4((USHORT) glyph->gx_glyph_width)),
                                  (d2_width) (D2_FIX4((USHORT) glyph->gx_glyph_height)),
                                  (d2_point) (D2_FIX4((USHORT) draw_area->gx_rectangle_left -
                                                      (USHORT) map_offset->gx_point_x)),
                                  (d2_point) (D2_FIX4((USHORT) draw_area->gx_rectangle_top -
                                                      (USHORT) map_offset->gx_point_y)),
                                  (d2_u32) d2_bf_usealpha | (d2_u32) d2_bf_colorize))

    /* Count the used display list size. */
    gx_dave2d_display_list_count(context->gx_draw_context_display);

    /* Set blend mode. */
    gx_dave2d_blend_mode_set(dave, d2_bm_alpha, d2_bm_one_minus_alpha);
}

/*******************************************************************************************************************/


/*******************************************************************************************************************/
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_simple_rotate(GX_DRAW_CONTEXT* context,
    INT xpos, INT ypos, GX_PIXELMAP* pixelmap, INT angle, INT cx, INT cy)
{
    GX_COLOR* putrow;
    GX_COLOR* get;
    INT           width;
    INT           height;
    USHORT        pixel;
    INT           x;
    INT           y;
    GX_RECTANGLE* clip;
    INT           newxpos;
    INT           newypos;

    GX_DISPLAY* display;
    VOID(*blend_func)(GX_DRAW_CONTEXT * context, INT x, INT y, GX_COLOR color, GX_UBYTE alpha);

    display = context->gx_draw_context_display;
    blend_func = display->gx_display_driver_pixel_blend;

    clip = context->gx_draw_context_clip;

    if (angle == 90)
    {
        width = pixelmap->gx_pixelmap_height;
        height = pixelmap->gx_pixelmap_width;

        newxpos = (xpos + cx) - (width - 1 - cy);
        newypos = (ypos + cy) - cx;

        for (y = ((INT)clip->gx_rectangle_top - newypos); y <= ((INT)clip->gx_rectangle_bottom - newypos); y++)
        {
            for (x = ((INT)clip->gx_rectangle_left - newxpos); x <= ((INT)clip->gx_rectangle_right - newxpos); x++)
            {
                get = (GX_COLOR*)pixelmap->gx_pixelmap_data;
                get += (width - 1 - x) * height;
                get += y;
                pixel = (USHORT)(((*get & 0xf80000) >> 8) | ((*get & 0xfc00) >> 5) | ((*get & 0xf8) >> 3));
                blend_func(context,
                    ((INT)clip->gx_rectangle_left + x),
                    ((INT)clip->gx_rectangle_top + y),
                    (GX_COLOR)pixel,
                    (GX_UBYTE)((*get) >> 24));
            }
        }
    }
    else if (angle == 180)
    {

        width = pixelmap->gx_pixelmap_width;
        height = pixelmap->gx_pixelmap_height;

        newxpos = (xpos + cx) - (width - 1 - cx);
        newypos = (ypos + cy) - (height - 1 - cy);

        putrow = context->gx_draw_context_memory;
        putrow += clip->gx_rectangle_top * context->gx_draw_context_pitch;
        putrow += clip->gx_rectangle_left;

        for (y = ((INT)clip->gx_rectangle_top - newypos); y <= ((INT)clip->gx_rectangle_bottom - newypos); y++)
        {
            for (x = ((INT)clip->gx_rectangle_left - newxpos); x <= ((INT)clip->gx_rectangle_right - newxpos); x++)
            {
                get = (GX_COLOR*)pixelmap->gx_pixelmap_data;
                get += (height - 1 - y) * width;
                get += (width - 1) - x;
                pixel = (USHORT)(((*get & 0xf80000) >> 8) | ((*get & 0xfc00) >> 5) | ((*get & 0xf8) >> 3));
                blend_func(context,
                    ((INT)clip->gx_rectangle_left + x),
                    ((INT)clip->gx_rectangle_top + y),
                    (GX_COLOR)pixel,
                    (GX_UBYTE)((*get) >> 24));
            }
        }
    }
    else if (angle == 270)
    {

        width = pixelmap->gx_pixelmap_height;
        height = pixelmap->gx_pixelmap_width;

        newxpos = (xpos + cx) - cy;
        newypos = ((ypos + cx) - (height - 1 - cy));

        putrow = context->gx_draw_context_memory;
        putrow += clip->gx_rectangle_top * context->gx_draw_context_pitch;
        putrow += clip->gx_rectangle_left;

        for (y = ((INT)clip->gx_rectangle_top - newypos); y <= ((INT)clip->gx_rectangle_bottom - newypos); y++)
        {
            for (x = ((INT)clip->gx_rectangle_left - newxpos); x <= ((INT)clip->gx_rectangle_right - newxpos); x++)
            {
                get = (GX_COLOR*)pixelmap->gx_pixelmap_data;
                get += x * height;
                get += (height - 1) - y;
                pixel = (USHORT)(((*get & 0xf80000) >> 8) | ((*get & 0xfc00) >> 5) | ((*get & 0xf8) >> 3));
                blend_func(context,
                    ((INT)clip->gx_rectangle_left + x),
                    ((INT)clip->gx_rectangle_top + y),
                    (GX_COLOR)pixel,
                    (GX_UBYTE)((*get) >> 24));
            }
        }
    }
}

/*******************************************************************************************************************/
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_left_edge
(pixelmap_adjacent_pixels_t* pixels, GX_PIXELMAP* pixelmap, INT y)
{
    GX_COLOR* get = NULL;

    get = (GX_COLOR*)pixelmap->gx_pixelmap_data;

    /* handle left edge case. */
    if (y >= 0)
    {
        pixels->b = *(get + (y * pixelmap->gx_pixelmap_width));
    }

    if (y < (pixelmap->gx_pixelmap_height - 1))
    {
        pixels->d = *(get + ((y + 1) * pixelmap->gx_pixelmap_width));
    }
}

/*******************************************************************************************************************/
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_top_edge
(pixelmap_adjacent_pixels_t* pixels, GX_PIXELMAP* pixelmap, INT x)
{
    GX_COLOR* get = NULL;

    get = (GX_COLOR*)pixelmap->gx_pixelmap_data;

    /* handle top edge.  */
    if (x >= 0)
    {
        pixels->c = *(get + x);
    }

    if (x < (pixelmap->gx_pixelmap_width - 1))
    {
        pixels->d = *(get + x + 1);
    }
}

/*******************************************************************************************************************/
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_right_edge
(pixelmap_adjacent_pixels_t* pixels, GX_PIXELMAP* pixelmap, INT x, INT y)
{
    GX_COLOR* get = NULL;

    get = (GX_COLOR*)pixelmap->gx_pixelmap_data;

    /* handle right edge. */
    if (y >= 0)
    {
        pixels->a = *(get + (y * (INT)pixelmap->gx_pixelmap_width) + x);
    }

    if (y < (pixelmap->gx_pixelmap_height - 1))
    {
        pixels->c = *(get + ((y + 1) * (INT)pixelmap->gx_pixelmap_width) + x);
    }
}

/*******************************************************************************************************************/
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_bottom_edge
(pixelmap_adjacent_pixels_t* pixels, GX_PIXELMAP* pixelmap, INT x, INT y)
{
    GX_COLOR* get = NULL;

    get = (GX_COLOR*)pixelmap->gx_pixelmap_data;

    /* handle bottom edge. */
    if (x >= 0)
    {
        pixels->a = *(get + (y * (INT)pixelmap->gx_pixelmap_width) + x);
    }

    if (x < ((INT)pixelmap->gx_pixelmap_width - 1))
    {
        pixels->b = *(get + ((y * (INT)pixelmap->gx_pixelmap_width) + x) + 1);
    }
}

/*******************************************************************************************************************/
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get(
    pixelmap_adjacent_pixels_t* pixels, GX_PIXELMAP* pixelmap, INT x, INT y)
{
    GX_COLOR* get;

    if ((x >= 0) && (x < ((INT)pixelmap->gx_pixelmap_width - 1)) &&
        (y >= 0) && (y < ((INT)pixelmap->gx_pixelmap_height - 1)))
    {
        get = (GX_COLOR*)pixelmap->gx_pixelmap_data;
        get += y * (INT)pixelmap->gx_pixelmap_width;
        get += x;

        pixels->a = *get;
        pixels->b = *(get + 1);
        pixels->c = *(get + (INT)pixelmap->gx_pixelmap_width);
        pixels->d = *(get + (INT)pixelmap->gx_pixelmap_width + 1);
    }
    else
    {
        pixels->a = 0U;
        pixels->b = pixels->a;
        pixels->c = pixels->a;
        pixels->d = pixels->a;

        if (x == -1)
        {
            gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_left_edge
            (pixels, pixelmap, y);
        }
        else if (y == -1)
        {
            gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_top_edge
            (pixels, pixelmap, x);
        }
        else if (x == (pixelmap->gx_pixelmap_width - 1))
        {
            gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_right_edge
            (pixels, pixelmap, x, y);
        }
        else if (y == (pixelmap->gx_pixelmap_height - 1))
        {
            gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get_bottom_edge
            (pixels, pixelmap, x, y);
        }
        else
        {
            /* DO NOTHING */
        }
    }
}

/*******************************************************************************************************************/
static INT gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_value_clip(INT value)
{
    INT rtn_value = value;

    if (value > 255)
    {
        rtn_value = (INT)255;
    }

    return rtn_value;
}

/*******************************************************************************************************************/
static VOID gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate(GX_DRAW_CONTEXT* context, INT xpos, INT ypos,
    GX_PIXELMAP* pixelmap, INT angle, INT cx, INT cy)
{
    INT             srcxres;
    INT             srcyres;
    INT             newxpos;
    INT             newypos;
    INT             cosv;
    INT             sinv;
    INT             xres;
    INT             yres;
    INT             alpha;
    GX_COLOR        red;
    GX_COLOR        green;
    GX_COLOR        blue;
    INT             idxminx;
    INT             idxmaxx;
    INT             idxmaxy;
    INT             mx[4];
    INT             my[4];
    INT             x;
    INT             y;
    INT             xx;
    INT             yy;
    INT             xdiff;
    INT             ydiff;
    GX_RECTANGLE* clip;
    pixelmap_adjacent_pixels_t  pixels;

    if (!(context->gx_draw_context_display->gx_display_driver_pixel_blend))
    {
        return;
    }

    mx[0] = -1;
    mx[1] = 1;
    mx[2] = 1;
    mx[3] = -1;

    my[0] = 1;
    my[1] = 1;
    my[2] = -1;
    my[3] = -1;

    idxminx = (INT)((UINT)(angle / 90) & 0x3U);
    idxmaxx = (INT)((UINT)(idxminx + 2) & 0x3U);
    idxmaxy = (INT)((UINT)(idxminx + 1) & 0x3U);

    /* Calculate the x and y center of pixelmap source. */
    srcxres = (INT)((UINT)pixelmap->gx_pixelmap_width >> 1);
    srcyres = (INT)((UINT)pixelmap->gx_pixelmap_height >> 1);

    cosv = _gx_utility_math_cos(GX_FIXED_VAL_MAKE(angle));
    sinv = _gx_utility_math_sin(GX_FIXED_VAL_MAKE(angle));

    xres = GX_FIXED_VAL_TO_INT((mx[idxmaxx] * srcxres * cosv) - (INT)(my[idxmaxx] * srcyres * sinv));
    yres = GX_FIXED_VAL_TO_INT((my[idxmaxy] * srcyres * cosv) + (INT)(mx[idxmaxy] * srcxres * sinv));

    x = GX_FIXED_VAL_TO_INT(((cx - srcxres) * cosv) - ((cy - srcyres) * sinv));
    y = GX_FIXED_VAL_TO_INT(((cy - srcyres) * cosv) + ((cx - srcxres) * sinv));

    x += xres;
    y += yres;

    newxpos = (xpos + cx) - x;
    newypos = (ypos + cy) - y;

    clip = context->gx_draw_context_clip;

    /* Loop through the source's pixels.  */
    for (y = ((INT)clip->gx_rectangle_top - newypos); y < ((INT)clip->gx_rectangle_bottom - newypos); y++)
    {
        for (x = ((INT)clip->gx_rectangle_left - newxpos); x < ((INT)clip->gx_rectangle_right - newxpos); x++)
        {
            xx = ((x - xres) * cosv) + ((y - yres) * sinv);
            yy = ((y - yres) * cosv) - ((x - xres) * sinv);

            xdiff = (INT)((UINT)GX_FIXED_VAL_TO_INT(xx << 8) & 0xffU);
            ydiff = (INT)((UINT)GX_FIXED_VAL_TO_INT(yy << 8) & 0xffU);

            xx = GX_FIXED_VAL_TO_INT(xx) + srcxres;
            yy = GX_FIXED_VAL_TO_INT(yy) + srcyres;

            if ((xx >= -1) && (xx < (INT)pixelmap->gx_pixelmap_width) &&
                (yy >= -1) && (yy < (INT)pixelmap->gx_pixelmap_height))
            {
                gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_adjacent_pixeldata_get(&pixels,
                    pixelmap,
                    xx,
                    yy);

                red = (GX_COLOR)(((REDVAL_32BPP(pixels.a) * (GX_COLOR)(pixels.a >> 24))
                    * (256 - (GX_COLOR)xdiff)) * (256 - (GX_COLOR)ydiff));
                red += (GX_COLOR)(((REDVAL_32BPP(pixels.b) * (GX_COLOR)(pixels.b >> 24))
                    * (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff));
                red += (GX_COLOR)(((REDVAL_32BPP(pixels.c) * (GX_COLOR)(pixels.c >> 24))
                    * (GX_COLOR)ydiff) * (256 - (GX_COLOR)xdiff));
                red += (GX_COLOR)(((REDVAL_32BPP(pixels.d) * (GX_COLOR)(pixels.d >> 24))
                    * (GX_COLOR)xdiff) * (GX_COLOR)ydiff);
                red >>= 16;

                green = (GX_COLOR)(((GREENVAL_32BPP(pixels.a) * (GX_COLOR)(pixels.a >> 24))
                    * (256 - (GX_COLOR)xdiff)) * (256 - (GX_COLOR)ydiff));
                green += (GX_COLOR)(((GREENVAL_32BPP(pixels.b) * (GX_COLOR)(pixels.b >> 24))
                    * (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff));
                green += (GX_COLOR)(((GREENVAL_32BPP(pixels.c) * (GX_COLOR)(pixels.c >> 24))
                    * (GX_COLOR)ydiff) * (256 - (GX_COLOR)xdiff));
                green += (GX_COLOR)(((GREENVAL_32BPP(pixels.d) * (GX_COLOR)(pixels.d >> 24))
                    * (GX_COLOR)xdiff) * (GX_COLOR)ydiff);
                green >>= 16;

                blue = (GX_COLOR)(((BLUEVAL_32BPP(pixels.a) * (GX_COLOR)(pixels.a >> 24))
                    * (256 - (GX_COLOR)xdiff)) * (256 - (GX_COLOR)ydiff));
                blue += (GX_COLOR)(((BLUEVAL_32BPP(pixels.b) * (GX_COLOR)(pixels.b >> 24))
                    * (GX_COLOR)xdiff) * (256 - (GX_COLOR)ydiff));
                blue += (GX_COLOR)(((BLUEVAL_32BPP(pixels.c) * (GX_COLOR)(pixels.c >> 24))
                    * (GX_COLOR)ydiff) * (256 - (GX_COLOR)xdiff));
                blue += (GX_COLOR)(((BLUEVAL_32BPP(pixels.d) * (GX_COLOR)(pixels.d >> 24))
                    * (GX_COLOR)xdiff) * (GX_COLOR)ydiff);
                blue >>= 16;

                alpha = (INT)(pixels.a >> 24);
                alpha = alpha * ((INT)256 - xdiff);
                alpha = alpha * ((INT)256 - ydiff);
                alpha += ((INT)(pixels.b >> 24) * (INT)xdiff) * ((INT)256 - ydiff);
                alpha += ((INT)(pixels.c >> 24) * (INT)ydiff) * ((INT)256 - xdiff);
                alpha += ((INT)(pixels.d >> 24) * (INT)xdiff) * (INT)ydiff;
                alpha = (INT)((UINT)alpha >> 16);

                alpha = gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_value_clip(alpha);
                if (alpha)
                {
                    red = (GX_COLOR)(red / (GX_COLOR)alpha);
                    green = (GX_COLOR)(green / (GX_COLOR)alpha);
                    blue = (GX_COLOR)(blue / (GX_COLOR)alpha);
                }

                red = (GX_COLOR)gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_value_clip((INT)red);
                green = (GX_COLOR)gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_value_clip((INT)green);
                blue = (GX_COLOR)gx_dave2d_display_driver_16bpp_32argb_pixelmap_rotate_value_clip((INT)blue);

                context->gx_draw_context_display->gx_display_driver_pixel_blend(context,
                    (x + newxpos),
                    (y + newypos),
                    (GX_COLOR)(((red & 0xf8) << 8) | ((green & 0xfc) << 3) | ((blue & 0xf8) >> 3)),
                    (GX_UBYTE)alpha);
            }
        }
    }
}

/******************************************************************************************************************/
static VOID gx_dave2d_copy_visible_to_working (GX_CANVAS *canvas, GX_RECTANGLE *copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;
    d2_u32       mode;

    ULONG *pGetRow;
    ULONG *pPutRow;

    INT copy_width;
    INT copy_height;

    GX_DISPLAY *display = canvas->gx_canvas_display;
    d2_device  *dave    = (d2_device *) display->gx_display_accelerator;

    _gx_utility_rectangle_define(&display_size, 0, 0, (GX_VALUE) (display->gx_display_width - 1),
                                 (GX_VALUE) (display->gx_display_height - 1));
    copy_clip = *copy;

    /* Align copy region on 32-bit memory boundary in case not aligned. */
    if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
    {
        /* If yes, align copy region on 32-bit boundary. */
        copy_clip.gx_rectangle_left  = (GX_VALUE) ((USHORT) (copy_clip.gx_rectangle_left) & 0xfffeU);
        copy_clip.gx_rectangle_right = (GX_VALUE) ((USHORT) copy_clip.gx_rectangle_right | 1U);
        mode = d2_mode_rgb565;
    }
    else
    {
        mode = d2_mode_argb8888;
    }

    /* Offset canvas within frame buffer */
    _gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    _gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    if ((copy_width <= 0) ||
        (copy_height <= 0))
    {
        return;
    }

    pGetRow = (ULONG *) visible_frame;
    pPutRow = (ULONG *) working_frame;

    CHECK_DAVE_STATUS(d2_framebuffer(dave, pPutRow, (d2_s32) (canvas->gx_canvas_x_resolution),
                                     (d2_u32) (canvas->gx_canvas_x_resolution),
                                     (d2_u32) (canvas->gx_canvas_y_resolution), (d2_s32) mode))

    gx_dave2d_cliprect_set(dave, &copy_clip);
    gx_dave2d_alpha_set(dave, (UCHAR) GX_ALPHA_VALUE_OPAQUE);

    CHECK_DAVE_STATUS(d2_setblitsrc(dave, (void *) pGetRow, canvas->gx_canvas_x_resolution,
                                    canvas->gx_canvas_x_resolution, canvas->gx_canvas_y_resolution, mode))

    CHECK_DAVE_STATUS(d2_blitcopy(dave, copy_width, copy_height, (d2_blitpos) (copy_clip.gx_rectangle_left),
                                  (d2_blitpos) (copy_clip.gx_rectangle_top), (d2_width) (D2_FIX4((UINT) copy_width)),
                                  (d2_width) (D2_FIX4((UINT) copy_height)),
                                  (d2_point) (D2_FIX4((USHORT) copy_clip.gx_rectangle_left)),
                                  (d2_point) (D2_FIX4((USHORT) copy_clip.gx_rectangle_top)), d2_bf_no_blitctxbackup))

    CHECK_DAVE_STATUS(d2_endframe(display->gx_display_accelerator))
    CHECK_DAVE_STATUS(d2_startframe(display->gx_display_accelerator))
}
#endif

#if (GX_USE_HARDWARE_JPEG == 1)
/******************************************************************************************************************/
static INT gx_renesas_jpeg_draw_open (GX_DRAW_CONTEXT *p_context, jpeg_instance_t *p_jpeg, UINT *p_bytes_per_pixel)
{
    /*Get the GUIX output color format */
    GX_VALUE color_format = p_context->gx_draw_context_display->gx_display_color_format;

    if (GX_COLOR_FORMAT_32ARGB == color_format)
    {
        *p_bytes_per_pixel = 4U;
    }
    else if (GX_COLOR_FORMAT_565RGB == color_format)
    {
        *p_bytes_per_pixel = 2U;
    }
    else
    {

        /* Simply return to the caller if invalid color format is specified */
        return FSP_ERR_JPEG_ERR;
    }

    /* Opens the JPEG driver.  */
    return (INT) p_jpeg->p_api->open(p_jpeg->p_ctrl, p_jpeg->p_cfg);
}

/******************************************************************************************************************/
static UINT gx_renesas_jpeg_draw_minimum_height_ycbcr444 (GX_VALUE width, GX_VALUE height)
{
    UINT minimum_height;

    /*8 lines by 8 pixels. */
    if (((USHORT) width & JPEG_ALIGNMENT_8) || ((USHORT) height & JPEG_ALIGNMENT_8))
    {
        minimum_height = 0U;
    }
    else
    {
        minimum_height = 8U;
    }

    return minimum_height;
}

/******************************************************************************************************************/
static UINT gx_renesas_jpeg_draw_minimum_height_ycbcr422 (GX_VALUE width, GX_VALUE height)
{
    UINT minimum_height;

    /*8 lines by 16 pixels. */
    if (((USHORT) width & JPEG_ALIGNMENT_16) || ((USHORT) height & JPEG_ALIGNMENT_8))
    {
        minimum_height = 0U;
    }
    else
    {
        minimum_height = 8U;
    }

    return minimum_height;
}

/******************************************************************************************************************/
static UINT gx_renesas_jpeg_draw_minimum_height_ycbcr411 (GX_VALUE width, GX_VALUE height)
{
    UINT minimum_height;

    /*8 lines by 32 pixels. */
    if (((USHORT) width & JPEG_ALIGNMENT_32) || ((USHORT) height & JPEG_ALIGNMENT_8))
    {
        minimum_height = 0U;
    }
    else
    {
        minimum_height = 8U;
    }

    return minimum_height;
}

/******************************************************************************************************************/
static UINT gx_renesas_jpeg_draw_minimum_height_ycbcr420 (GX_VALUE width, GX_VALUE height)
{
    UINT minimum_height;

    /*16 lines by 16 pixels. */
    if (((USHORT) width & JPEG_ALIGNMENT_16) || ((USHORT) height & JPEG_ALIGNMENT_16))
    {
        minimum_height = 0U;
    }
    else
    {
        minimum_height = 16U;
    }

    return minimum_height;
}

/******************************************************************************************************************/
static UINT gx_renesas_jpeg_draw_minimum_height_get (jpeg_color_space_t format, GX_VALUE width, GX_VALUE height)
{
    UINT minimum_height;

    /* Compute the decoding width and height based on pixel format. Return an error if the input JPEG size is not valid. */
    switch (format)
    {
        case JPEG_COLOR_SPACE_YCBCR444:
        {
            /*8 lines by 8 pixels. */
            minimum_height = gx_renesas_jpeg_draw_minimum_height_ycbcr444(width, height);
            break;
        }

        case JPEG_COLOR_SPACE_YCBCR422:
        {
            /*8 lines by 16 pixels. */
            minimum_height = gx_renesas_jpeg_draw_minimum_height_ycbcr422(width, height);
            break;
        }

        case JPEG_COLOR_SPACE_YCBCR411:
        {
            /*8 lines by 32 pixels. */
            minimum_height = gx_renesas_jpeg_draw_minimum_height_ycbcr411(width, height);
            break;
        }

        case JPEG_COLOR_SPACE_YCBCR420:
        {
            /*16 lines by 16 pixels. */
            minimum_height = gx_renesas_jpeg_draw_minimum_height_ycbcr420(width, height);
            break;
        }

        default:
            minimum_height = 0U;
    }

    return minimum_height;
}

/******************************************************************************************************************//* 
 *@brief  Subroutine for Hardware accelerated JPEG draw to perform JPEG decoding in output streaming mode.
 * This function is called by _gx_synergy_jpeg_draw_output_streaming().
 *@param   p_jpeg[in]                 Pointer to a parameter set for the JPEG decode framework instance
 *@retval  FSP_SUCCESS       Display device was opened successfully.
 *@retval  Others            See @ref Common_Error_Codes for other possible return codes. This function calls
 *                           jpeg_api_t::wait.
 **********************************************************************************************************************/
static INT gx_renesas_jpeg_draw_output_streaming_wait ()
{
    INT ret;

    jpeg_status_t jpeg_status = JPEG_STATUS_NONE;

    while (((UINT) (JPEG_STATUS_OUTPUT_PAUSE) &(UINT) jpeg_status) == 0U)
    {
        ret = gx_renesas_jpeg_wait(&jpeg_status, 1000);
        if (ret != FSP_SUCCESS)
        {

            /*Nothing to draw. Just return. */
            return ret;
        }

        if ((UINT) (JPEG_STATUS_OPERATION_COMPLETE) &(UINT) jpeg_status)
        {
            /*Finished JPEG decoding. */
            break;
        }
    }

    return FSP_SUCCESS;
}

/******************************************************************************************************************/
static VOID gx_renesas_jpeg_draw_output_streaming (jpeg_output_streaming_param_t *p_param)
{
    INT               ret;
    jpeg_instance_t *p_jpeg              = p_param->p_jpeg;
    UINT              lines_decoded       = 0;
    UINT              remaining_lines     = 0;
    UINT              total_lines_decoded = 0;
    GX_VIEW         *view;
    GX_RECTANGLE      clip_rect;
    GX_PIXELMAP       out_pixelmap;
    GX_DRAW_CONTEXT *p_context    = p_param->p_context;
    GX_VALUE          color_format = p_context->gx_draw_context_display->gx_display_color_format;
    GX_RECTANGLE      bound;

    remaining_lines = (UINT) p_param->image_height;

    /* Sets up the out_pixelmap structure. */
    out_pixelmap.gx_pixelmap_data          = (GX_UBYTE *) (p_param->output_buffer);
    out_pixelmap.gx_pixelmap_data_size     = (ULONG) p_param->jpeg_buffer_size;
    out_pixelmap.gx_pixelmap_format        = (GX_UBYTE) color_format;
    out_pixelmap.gx_pixelmap_height        = p_param->image_height;
    out_pixelmap.gx_pixelmap_width         = p_param->image_width;
    out_pixelmap.gx_pixelmap_version_major = p_param->p_pixelmap->gx_pixelmap_version_major;
    out_pixelmap.gx_pixelmap_version_minor = p_param->p_pixelmap->gx_pixelmap_version_minor;
    out_pixelmap.gx_pixelmap_flags         = (GX_UBYTE) 0;

    /* Calculate rectangle that bounds the JPEG */
    gx_utility_rectangle_define(&bound, (GX_VALUE) p_param->x, (GX_VALUE) p_param->y,
                                (GX_VALUE) ((p_param->x + p_param->image_width) - 1),
                                (GX_VALUE) ((p_param->y + p_param->image_height) - 1));

    /* Clip the line bounding box to the dirty rectangle */
    if (!gx_utility_rectangle_overlap_detect(&bound, &p_context->gx_draw_context_dirty, &bound))
    {

        /*Nothing to draw. Just return. */
        return;
    }

    while (remaining_lines > 0U)
    {
        /*If running with Dave2D, make sure previous block drawing is completed before we attempt to decode
         *new jpeg block.
         */
 #if (GX_USE_DAVE2D_DRAW == 1)
        CHECK_DAVE_STATUS(d2_endframe(p_context->gx_draw_context_display->gx_display_accelerator))

        /*trigger execution of previous display list, switch to new display list */
        CHECK_DAVE_STATUS(d2_startframe(p_context->gx_draw_context_display->gx_display_accelerator))
 #endif

        /* Assigns the output buffer to start the decoding process. */
        ret =
            (INT) p_jpeg->p_api->outputBufferSet(p_jpeg->p_ctrl,
                                                 (VOID *) (INT) (out_pixelmap.gx_pixelmap_data),
                                                 (UINT) p_param->jpeg_buffer_size);
        if (ret != FSP_SUCCESS)
        {

            /*Nothing to draw. Just return. */
            return;
        }

        /* Waits for the device to finish. */
        ret = gx_renesas_jpeg_draw_output_streaming_wait();
        if (ret != FSP_SUCCESS)
        {
            return;
        }

        ret = (INT) p_jpeg->p_api->linesDecodedGet(p_jpeg->p_ctrl, (uint32_t *) &lines_decoded);

        if ((ret != FSP_SUCCESS) || (lines_decoded == 0U))
        {

            /*Nothing to draw. Just return. */
            return;
        }

        remaining_lines -= lines_decoded;

        out_pixelmap.gx_pixelmap_height    = (GX_VALUE) lines_decoded;
        out_pixelmap.gx_pixelmap_data_size =
            (ULONG) (lines_decoded *((UINT) p_param->image_width *p_param->bytes_per_pixel));

        view = p_context->gx_draw_context_view_head;

        while (view)
        {
            if (!gx_utility_rectangle_overlap_detect(&view->gx_view_rectangle, &bound, &clip_rect))
            {
                view = view->gx_view_next;
                continue;
            }

            p_context->gx_draw_context_clip = &clip_rect;

            /* Draws map.  */
            p_context->gx_draw_context_display->gx_display_driver_pixelmap_draw(p_context,
                                                                                p_param->x,
                                                                                p_param->y +
                                                                                (INT) total_lines_decoded,
                                                                                &out_pixelmap);

            /* Goes to the next view. */
            view = view->gx_view_next;
        }

        total_lines_decoded += lines_decoded;

 #if (GX_USE_DAVE2D_DRAW == 1)
        CHECK_DAVE_STATUS(d2_endframe(p_context->gx_draw_context_display->gx_display_accelerator))

        /*trigger execution of previous display list, switch to new display list */
        CHECK_DAVE_STATUS(d2_startframe(p_context->gx_draw_context_display->gx_display_accelerator))
 #endif
    }
}

#endif                                 /*GX_USE_HARDWARE_JPEG */

/******************************************************************************************************************/
#if 0
static VOID gx_copy_visible_to_working (GX_CANVAS *canvas, GX_RECTANGLE *copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;

    ULONG *pGetRow;
    ULONG *pPutRow;

    INT copy_width;
    INT copy_height;
    INT canvas_stride;
    INT display_stride;

    ULONG *pGet;
    ULONG *pPut;
    INT     row;
    INT     col;

    GX_DISPLAY *display = canvas->gx_canvas_display;
    gx_utility_rectangle_define(&display_size, 0, 0, (GX_VALUE) (display->gx_display_width - 1),
                                (GX_VALUE) (display->gx_display_height - 1));
    copy_clip = *copy;

    /* Is color format RGB565? */
    if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
    {
        /* If yes, align copy region on 32-bit boundary. */
        copy_clip.gx_rectangle_left  = (GX_VALUE) ((USHORT) copy_clip.gx_rectangle_left & 0xfffeU);
        copy_clip.gx_rectangle_right = (GX_VALUE) ((USHORT) copy_clip.gx_rectangle_right | 1U);
    }

    /* Offset canvas within frame buffer. */
    gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);

    gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width  = (copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left) + 1;
    copy_height = (copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top) + 1;

    /* Return if copy width or height was invalid. */
    if ((copy_width <= 0) || (copy_height <= 0))
    {
        return;
    }

    pGetRow = (ULONG *) visible_frame;
    pPutRow = (ULONG *) working_frame;

    /* Calculate copy width, canvas stride, source and destination pointer for data copy. */
    if ((INT) display->gx_display_color_format == GX_COLOR_FORMAT_565RGB)
    {
        copy_width   /= 2;
        canvas_stride = canvas->gx_canvas_x_resolution / 2;
        pPutRow       = pPutRow + ((INT) copy_clip.gx_rectangle_top *canvas_stride);
        pPutRow       = pPutRow + ((INT) copy_clip.gx_rectangle_left / 2);

        display_stride = display->gx_display_width / 2;
        pGetRow        = pGetRow +
                         ((INT) (canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) *display_stride);
        pGetRow = pGetRow + ((INT) (canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left) / 2);
    }
    else
    {
        canvas_stride = canvas->gx_canvas_x_resolution;
        pPutRow       = pPutRow + ((INT) copy_clip.gx_rectangle_top *canvas_stride);
        pPutRow       = pPutRow + (INT) copy_clip.gx_rectangle_left;

        display_stride = display->gx_display_width;
        pGetRow        = pGetRow +
                         ((INT) (canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) *display_stride);
        pGetRow = pGetRow + (INT) (canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);
    }

    /* Copy data by software. */
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
#endif

/******************************************************************************************************************/
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



/******************************************************************************************************************/
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

    /* Offset canvas within frame buffer. */
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

    pPutRow = pPutRow + (INT)((display->gx_display_height - copy_clip.gx_rectangle_top) *canvas_stride);
    pPutRow = pPutRow - (INT)(copy_clip.gx_rectangle_left + 1);

    pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) *canvas_stride);
    pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

    gx_flip_canvas_to_working_16bpp_draw(pGetRow, pPutRow, copy_width, copy_height, canvas_stride);
}



/******************************************************************************************************************/
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

/******************************************************************************************************************/
static VOID gx_flip_canvas_to_working_32bpp(GX_CANVAS  *canvas, GX_RECTANGLE *copy)
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

    /* Offset canvas within frame buffer. */
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

    pPutRow = pPutRow + ((INT)(display->gx_display_height - copy_clip.gx_rectangle_top) *canvas_stride);
    pPutRow = pPutRow - (INT)(copy_clip.gx_rectangle_left + 1);

    pGetRow = pGetRow + ((INT)(canvas->gx_canvas_display_offset_y + copy_clip.gx_rectangle_top) *canvas_stride);
    pGetRow = pGetRow + (INT)(canvas->gx_canvas_display_offset_x + copy_clip.gx_rectangle_left);

    gx_flip_canvas_to_working_32bpp_draw(pGetRow, pPutRow, copy_width, copy_height, canvas_stride);
}
