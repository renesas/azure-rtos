
/*******************************************************************************/
#include <string.h>
#include "gx_touch_driver_rx72n_ek.h"
#include "gx_api.h"

#define TOUCH_STATE_TOUCHED  1
#define TOUCH_STATE_RELEASED 2
#define MIN_DRAG_DELTA       10

static int last_pos_x;
static int last_pos_y;
static int curpos_x;
static int curpos_y;
static int touch_state;

#define TOUCH_READ_INFO_SIZE     (5)
static GX_UBYTE touch_data[TOUCH_READ_INFO_SIZE];

TX_THREAD touch_thread;
ULONG touch_thread_stack[1024];

static volatile sci_iic_info_t siic_info;

typedef struct st_touch_info {
    struct {
        uint8_t :5;
        uint8_t touch_points:3;
    } td_status;
    struct {
        uint8_t event_flag:2;
        uint8_t :2;
        uint8_t x_pos_msb:4;
    } touch1_xh;
    uint8_t touch1_xl;
    struct {
        uint8_t :4;
        uint8_t y_pos_msb:4;
    } touch1_yh;
    uint8_t touch1_yl;
} touch_info_t;

/*******************************************************************************/
void gx_touch_initialize (void)
{
    sci_iic_return_t err_ret;

    siic_info.ch_no = 6;
    siic_info.dev_sts = SCI_IIC_NO_INIT;

    err_ret = R_SCI_IIC_Open((sci_iic_info_t *) &siic_info);

    if (SCI_IIC_SUCCESS != err_ret)
    {
        while (1)
        {
        }
    }
}


/*******************************************************************************/
int gx_touch_scan()
{
    static uint8_t touch_device;
    static uint8_t read_addr;
    //touch_info_t * p_touch_info;
    //touch_event_t touch_event = NO_TOUCH;
    volatile sci_iic_return_t ret;
    volatile uint16_t x_pos;
    volatile uint16_t y_pos;
    volatile uint8_t i;

    touch_device = 0x38;
    read_addr = 0x02;

    /* ---- Set arguments ---- */
    siic_info.p_slv_adr = &touch_device;
    siic_info.cnt1st = 1;
    siic_info.cnt2nd = TOUCH_READ_INFO_SIZE;
    siic_info.p_data1st = &read_addr;
    siic_info.p_data2nd = touch_data;
    siic_info.callbackfunc = &touch_callback;

    /* ---- Master receive start ---- */
    ret = R_SCI_IIC_MasterReceive((sci_iic_info_t *) &siic_info);
    if (SCI_IIC_SUCCESS == ret)
    {
        while (SCI_IIC_FINISH != siic_info.dev_sts)
        {

        }
    }
    else
    {
        /* error */
        return TOUCH_STATE_RELEASED;
    }

    /* ---- Get data analysis ---- */
    //p_touch_info = (touch_info_t *) touch_data;

    if (touch_data[0] && ((touch_data[1] & 0xc0) == 0x80))
    {
    	curpos_x = (touch_data[1] & 0x0f) << 8;
    	curpos_x |= touch_data[2];
    	curpos_y = (touch_data[3] & 0x0f) << 8;
    	curpos_y |= touch_data[4];
     	return TOUCH_STATE_TOUCHED;
    }
    return TOUCH_STATE_RELEASED;

}



/*******************************************************************************/
void touch_callback (void)
{
    volatile sci_iic_return_t ret;
    sci_iic_mcu_status_t iic_status;

    ret = R_SCI_IIC_GetStatus((sci_iic_info_t *)&siic_info, &iic_status);

    if (SCI_IIC_SUCCESS != ret)
    {
        /* Call error processing for the R_SCI_IIC_GetStatus()function*/
    }
    else
    {
        if (1 == iic_status.BIT.NACK)
        {
            /* Processing when a NACK is detected by verifying the iic_status flag. */
        }
    }

}

/**************************************************************************/
VOID gx_send_pen_down_event(VOID)
{
    GX_EVENT event;
    event.gx_event_type = GX_EVENT_PEN_DOWN;
    event.gx_event_payload.gx_event_pointdata.gx_point_x = curpos_x;
    event.gx_event_payload.gx_event_pointdata.gx_point_y = curpos_y;
    event.gx_event_sender = 0;
    event.gx_event_target = 0;
    event.gx_event_display_handle = 0;
    gx_system_event_send(&event);
}

/**************************************************************************/
VOID gx_send_pen_drag_event(VOID)
{
    GX_EVENT event;
    int x_delta = GX_ABS(curpos_x - last_pos_x);
    int y_delta = GX_ABS(curpos_y - last_pos_y);

    if (x_delta > MIN_DRAG_DELTA || y_delta > MIN_DRAG_DELTA)
    {
        event.gx_event_type = GX_EVENT_PEN_DRAG;
        event.gx_event_payload.gx_event_pointdata.gx_point_x = curpos_x;
        event.gx_event_payload.gx_event_pointdata.gx_point_y = curpos_y;
        event.gx_event_sender = 0;
        event.gx_event_target = 0;
        event.gx_event_display_handle = 0;
        last_pos_x = curpos_x;
        last_pos_y = curpos_y;

        gx_system_event_fold(&event);
    }
}

/**************************************************************************/
VOID gx_send_pen_up_event(VOID)
{
    GX_EVENT event;
    event.gx_event_type = GX_EVENT_PEN_UP;
    event.gx_event_payload.gx_event_pointdata.gx_point_x = curpos_x;
    event.gx_event_payload.gx_event_pointdata.gx_point_y = curpos_y;
    event.gx_event_sender = 0;
    event.gx_event_target = 0;
    event.gx_event_display_handle = 0;
    last_pos_x = curpos_x;
    last_pos_y = curpos_y;
    gx_system_event_send(&event);
}

/**************************************************************************/
VOID  touch_thread_entry(ULONG thread_input)
{
	int status;
	gx_touch_initialize();

    /* Initialize the touch handle. */
    touch_state = TOUCH_STATE_RELEASED;

    tx_thread_sleep(30);

    while(1)
    {
        //tx_event_flags_get(&touch_events, 1, TX_AND_CLEAR, &actual_flags, TX_WAIT_FOREVER);
        tx_thread_sleep(2);
        status = gx_touch_scan();

        if (status == TOUCH_STATE_TOUCHED)
        {
        	// screen is touched, update coords:
            if (touch_state == TOUCH_STATE_RELEASED)
            {
                touch_state = TOUCH_STATE_TOUCHED;
                gx_send_pen_down_event();
            }
            else
            {
                // test and send pen drag
                gx_send_pen_drag_event();
            }
        }
        else
        {
            // no touch, check so see if last was touched
            if (touch_state == TOUCH_STATE_TOUCHED)
            {
                touch_state = TOUCH_STATE_RELEASED;
                gx_send_pen_up_event();
            }
        }
    }
}

/**************************************************************************/
/* called by application to fire off the touch screen driver thread       */
VOID start_touch_thread(void)
{
    /* Create the touch driver thread.  */
    tx_thread_create(&touch_thread, "GUIX Touch Thread", touch_thread_entry,
                     0,  touch_thread_stack, sizeof(touch_thread_stack),
                     GX_SYSTEM_THREAD_PRIORITY - 1, GX_SYSTEM_THREAD_PRIORITY - 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}
