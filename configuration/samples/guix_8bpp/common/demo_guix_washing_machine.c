/* This is a small demo of the high-performance GUIX graphics framework. */

#include "demo_guix_washing_machine.h"

/* Define the ThreadX demo thread control block and stack.  */
TX_BYTE_POOL       memory_pool;

#define POWER_ON_OFF_TIMER  10
#define CLOCK_TIMER         20

// #define SCRATCHPAD_PIXELS (MAIN_DISPLAY_X_RESOLUTION * MAIN_DISPLAY_Y_RESOLUTION)
#define SCRATCHPAD_PIXELS   (16 * 1024)

/* Define memory for memory pool. */
GX_COLOR           scratchpad[SCRATCHPAD_PIXELS];

GX_WINDOW_ROOT    *root;
extern GX_COLOR *working_frame;
GX_BOOL	 		 power_on_flag = GX_TRUE;


/* Define canvas blend alpha. */
INT                blend_alpha = 255;

/* Define canvas blend alpha increment. */
INT                blend_alpha_increment = 0;

/* Define power on/off callback. */
VOID             (*power_on_callback)() = GX_NULL;
VOID             (*power_off_callback)() = GX_NULL;


/* Define prototypes.   */
VOID  guix_setup(GX_UBYTE *first_unused_memory);
extern UINT rx_graphics_driver_setup_8bit_palette(GX_DISPLAY *display);

VOID clock_update();
VOID main_screen_widgets_enable_disable(INT status);

static GX_CONST GX_CHAR day_name_sun[] = "Sunday";
static GX_CONST GX_CHAR day_name_mon[] = "Monday";
static GX_CONST GX_CHAR day_name_tue[] = "Tuesday";
static GX_CONST GX_CHAR day_name_wed[] = "Wednesday";
static GX_CONST GX_CHAR day_name_thu[] = "Thursday";
static GX_CONST GX_CHAR day_name_fri[] = "Friday";
static GX_CONST GX_CHAR day_name_sat[] = "Saturday";

const GX_STRING day_names[7] = {
    {day_name_sun, sizeof(day_name_sun) - 1},
    {day_name_mon, sizeof(day_name_mon) - 1},
    {day_name_tue, sizeof(day_name_tue) - 1},
    {day_name_wed, sizeof(day_name_wed) - 1},
    {day_name_thu, sizeof(day_name_thu) - 1},
    {day_name_fri, sizeof(day_name_fri) - 1},
    {day_name_sat, sizeof(day_name_sat) - 1}
};


const GX_CHAR *month_names[12] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};

GX_WIDGET *main_screen_enable_disable_widgets[] = {
    (GX_WIDGET *)&main_screen.main_screen_pixelmap_slider,
    (GX_WIDGET *)&main_screen.main_screen_page_name,
    (GX_WIDGET *)&main_screen.main_screen_button_washer_on,
    (GX_WIDGET *)&main_screen.main_screen_button_garments,
    (GX_WIDGET *)&main_screen.main_screen_button_water_level,
    (GX_WIDGET *)&main_screen.main_screen_button_temperature,
    GX_NULL
};


/******************************************************************************************/
/* Define memory allocator function.                                                     */
/******************************************************************************************/
VOID *memory_allocate(ULONG size)
{
    VOID *memptr;

    if (tx_byte_allocate(&memory_pool, &memptr, size, TX_NO_WAIT) == TX_SUCCESS)
    {
        return memptr;
    }
    return NULL;
}

/******************************************************************************************/
/* Define memory de-allocator function.                                                   */
/******************************************************************************************/
void memory_free(VOID *mem)
{
    tx_byte_release(mem);
}


/******************************************************************************************/
/* Initiate and run GUIX.                                                                 */
/******************************************************************************************/
VOID  guix_setup(GX_UBYTE *first_unused_memory)
{
    /* create byte pool. */
    tx_byte_pool_create(&memory_pool, "scratchpad", scratchpad,
        SCRATCHPAD_PIXELS * sizeof(GX_COLOR));

    /* Initialize GUIX. */
    gx_system_initialize();

    /* install our memory allocator and de-allocator */
    gx_system_memory_allocator_set(memory_allocate, memory_free);

     /* Configure display. */

    gx_studio_display_configure(MAIN_DISPLAY, rx_graphics_driver_setup_8bit_palette,
        LANGUAGE_ENGLISH, MAIN_DISPLAY_THEME_1, &root);

    gx_canvas_memory_define(root->gx_window_root_canvas, (GX_COLOR *) working_frame, MAIN_DISPLAY_X_RESOLUTION * MAIN_DISPLAY_Y_RESOLUTION);

    /* Create the main screen and attach it to root window. */
    gx_studio_named_widget_create("main_screen", (GX_WIDGET *)root, GX_NULL);

    /* Create garments window. */
    gx_studio_named_widget_create("garments_window", GX_NULL, GX_NULL);

    /* Create water level window. */
    gx_studio_named_widget_create("water_level_window", GX_NULL, GX_NULL);

    /* Create temperature window. */
    gx_studio_named_widget_create("temperature_window", GX_NULL, GX_NULL);

    /* Show the root window to make it and main screen visible.  */
    gx_widget_show(root);

    /* Let GUIX run */
    gx_system_start();
}

/******************************************************************************************/
/* Override the default event processing of "main_screen" to handle signals from my child */
/* widgets.                                                                               */
/******************************************************************************************/
UINT main_screen_event_process(GX_WINDOW *window, GX_EVENT *event_ptr)
{
    switch (event_ptr->gx_event_type)
    {
    case GX_EVENT_SHOW:
        clock_update();

        /* Init washer on page. */
        washer_on_page_init();
        power_on_callback = washer_on_page_init;
        power_off_callback = washer_on_page_power_off;

        /* Start a timer to update current time. */
        gx_system_timer_start((GX_WIDGET *)window, CLOCK_TIMER, GX_TICKS_SECOND, GX_TICKS_SECOND);

        /* Call default event process. */
        gx_window_event_process(window, event_ptr);
        break;

    case GX_SIGNAL(ID_BTN_WASHER_ON, GX_EVENT_RADIO_SELECT):

        /* Init washer on page. */
        washer_on_page_init();
        power_on_callback = washer_on_page_init;
        power_off_callback = washer_on_page_power_off;
        break;

    case GX_SIGNAL(ID_BTN_WASHER_ON, GX_EVENT_RADIO_DESELECT):
        /* Dettach washer on page. */
        gx_widget_detach(&main_screen.main_screen_washer_on_window);

        /* Set washer on button icon id to "ICON_START". */
        gx_icon_pixelmap_set(&main_screen.main_screen_washer_on_icon, GX_PIXELMAP_ID_BUTTON_ICON_START, GX_NULL);
        break;

    case GX_SIGNAL(ID_BTN_GARMENTS, GX_EVENT_RADIO_SELECT):

        /* Init garments page. */
        garments_page_init();
        power_on_callback = garments_page_init;
        power_off_callback = garments_page_power_off;
        break;

    case GX_SIGNAL(ID_BTN_GARMENTS, GX_EVENT_RADIO_DESELECT):
        /* Detach garments page. */
        gx_widget_detach(&garments_window);
        break;

    case GX_SIGNAL(ID_BTN_WATER_LEVEL, GX_EVENT_RADIO_SELECT):

        /* Init water level page. */
        water_level_page_init();
        power_on_callback = water_level_page_init;
        power_off_callback = water_level_page_power_off;
        break;

    case GX_SIGNAL(ID_BTN_WATER_LEVEL, GX_EVENT_RADIO_DESELECT):
        /* Detach water level page. */
        gx_widget_detach(&water_level_window);
        break;

    case GX_SIGNAL(ID_BTN_TEMPERATURE, GX_EVENT_RADIO_SELECT):
        /* Init temperature page. */
        temperature_page_init();
        power_on_callback = temperature_page_init;
        power_off_callback = temperature_page_power_off;
        break;

    case GX_SIGNAL(ID_BTN_TEMPERATURE, GX_EVENT_RADIO_DESELECT):
        /* Dettach temperature page. */
        gx_widget_detach(&temperature_window);
        break;

    case GX_SIGNAL(ID_BTN_POWER_ON_OFF, GX_EVENT_CLICKED):
        if (is_power_on())
        {
        	power_on_flag = GX_FALSE;
            if (power_off_callback)
            {
                power_off_callback();
                main_screen_widgets_enable_disable(POWER_OFF);
            }
        }
        else
        {
        	power_on_flag = GX_TRUE;
            if (power_on_callback)
            {
                power_on_callback();
                main_screen_widgets_enable_disable(POWER_ON);
            }
        }
        break;


    default:
        return gx_window_event_process(window, event_ptr);
    }

    return 0;
}

/******************************************************************************************/
/* A custom prompt draw function that draws the widget with specified blend alpha.        */
/******************************************************************************************/
VOID prompt_alpha_draw(GX_PROMPT *prompt)
{
    GX_BRUSH *brush;

    /* Get context brush. */
    gx_context_brush_get(&brush);

    /* Set brush alpha. */
    brush->gx_brush_alpha = blend_alpha;

    gx_prompt_draw(prompt);
}

/******************************************************************************************/
/* A custom pixelmap button draw function that draws the widget with specified blend      */
/* alpha.                                                                                 */
/******************************************************************************************/
VOID pixelmap_button_alpha_draw(GX_PIXELMAP_BUTTON *button)
{
    GX_BRUSH *brush;

    /* Get context brush. */
    gx_context_brush_get(&brush);

    brush->gx_brush_alpha = blend_alpha;

    gx_pixelmap_button_draw(button);
}

/******************************************************************************************/
/* A custom pixlemap slider draw function that draws the widget with specified blend      */
/* alpha.                                                                                 */
/******************************************************************************************/
VOID pixelmap_slider_alpha_draw(GX_PIXELMAP_SLIDER *slider)
{
    GX_BRUSH *brush;

    /* Get context brush. */
    gx_context_brush_get(&brush);

    brush->gx_brush_alpha = blend_alpha;

    gx_pixelmap_slider_draw(slider);
}

/******************************************************************************************/
/* A custom wubdiw draw function that draws the widget with specified blend alpha.        */
/******************************************************************************************/
VOID window_alpha_draw(GX_WINDOW *window)
{
    GX_BRUSH *brush;

    /* Get context brush. */
    gx_context_brush_get(&brush);

    brush->gx_brush_alpha = blend_alpha;

    gx_window_draw(window);
}

/******************************************************************************************/
/* Update clock of main screen.                                                           */
/******************************************************************************************/
VOID clock_update()
{
#ifdef WIN32
    GX_CHAR time_string_buffer[6];
    GX_CHAR am_pm_buffer[3];
    GX_CHAR date_string_buffer[20];
    GX_STRING string;

    SYSTEMTIME local_time;
    GetLocalTime(&local_time);
    if (local_time.wHour < 12)
    {
        sprintf(time_string_buffer, "%d:%02d", local_time.wHour, local_time.wMinute);
        GX_STRCPY(am_pm_buffer, "AM");
    }
    else
    {
        sprintf(time_string_buffer, "%d:%02d", local_time.wHour - 12, local_time.wMinute);
        GX_STRCPY(am_pm_buffer, "PM");
    }

    sprintf(date_string_buffer, "%s %02d, %d", month_names[local_time.wMonth - 1], local_time.wDay, local_time.wYear);

    string.gx_string_ptr = time_string_buffer;
    string.gx_string_length = string_length_get(time_string_buffer, sizeof(time_string_buffer) - 1);
    gx_prompt_text_set_ext(&main_screen.main_screen_time, &string);

    string.gx_string_ptr = am_pm_buffer;
    string.gx_string_length = string_length_get(am_pm_buffer, sizeof(am_pm_buffer) - 1);
    gx_prompt_text_set_ext(&main_screen.main_screen_am_pm, &string);
    gx_prompt_text_set_ext(&main_screen.main_screen_day_of_week, &day_names[local_time.wDayOfWeek]);

    string.gx_string_ptr = date_string_buffer;
    string.gx_string_length = string_length_get(date_string_buffer, sizeof(date_string_buffer) - 1);
    gx_prompt_text_set_ext(&main_screen.main_screen_date, &string);

#else
#endif
}

/******************************************************************************************/
/* Enable/Disable main screen.                                                            */
/******************************************************************************************/
VOID main_screen_widgets_enable_disable(INT status)
{
    GX_WIDGET *widget;
    INT        index = 0;

    widget = main_screen_enable_disable_widgets[index];

    while (widget)
    {
    	if (status == POWER_ON)
    	{
            //gx_widget_style_add(widget, GX_STYLE_ENABLED);
    		gx_widget_show(widget);
    	}
    	else
    	{
            // gx_widget_style_remove(widget, GX_STYLE_ENABLED);
            gx_widget_hide(widget);
    	}
    	index++;
    	widget = main_screen_enable_disable_widgets[index];
    }
}

/******************************************************************************************/
/* Calculate string length.                                                               */
/******************************************************************************************/
UINT string_length_get(GX_CONST GX_CHAR* input_string, UINT max_string_length)
{
    UINT length = 0;

    if (input_string)
    {
        /* Traverse the string.  */
        for (length = 0; input_string[length]; length++)
        {
            /* Check if the string length is bigger than the max string length.  */
            if (length >= max_string_length)
            {
                break;
            }
        }
    }

    return length;
}

/******************************************************************************************/
/* Retrieve power status.                                                                 */
/******************************************************************************************/
GX_BOOL is_power_on()
{
	return power_on_flag;
}
