/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : menu_led.c
 * Version      : 1.0
 * Description  : The led demo console and led display file. Manages demo interface to console and lcd.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <math.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_init.h"
#include "common_utils.h"
#include "jlink_console.h"

#include "tp_thread_entry.h"
#include "display_thread_entry.h"

#include "menu_main.h"
#include "menu_led.h"

#include "images/user_font_title/user_font_title_if.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#undef MODULE_NAME
#define MODULE_NAME             "\r\n%d. USER LED CONTROL\r\n\r\n"
#define LED_LINE_THICKNESS      (4)

#define RED_LED_PORT            (BSP_IO_PORT_10_PIN_07)
#define GREEN_LED_PORT          (BSP_IO_PORT_03_PIN_03)
#define BLUE_LED_PORT           (BSP_IO_PORT_06_PIN_00)

// Mathematical constants
#define R_CONST_RAD2DEG          (57.2957795130823209)
#define R_CONST_INDICATOR_RADIUS (120.0)
#define R_CONST_TWOPI            (6.2831853071795865)

// Starting delay used when angle is subtracted forming approximation of the the led blinking rate
#define R_CONST_BLINK_RATE_DELAY_INDICATOR (365)

// Scale factor to convert % to screen position on brightness slider
#define R_CONST_SCALEX                      (2.16)

// Height of the vertical brightness indicator
#define R_CONST_BRIGHTNESS_INDICATOR_LENGTH (64)

#define RED_BLINK_RATE_SELECTED   (0)
#define GREEN_BLINK_RATE_SELECTED (1)
#define BLUE_BLINK_RATE_SELECTED  (2)

#define RED_BRIGHNESS_SELECTED    (3)
#define GREEN_BRIGHNESS_SELECTED  (4)
#define BLUE_BRIGHNESS_SELECTED   (5)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

static int8_t map_tp_to_hitbox(uint32_t x_hp, uint32_t y_hp);

static void led_brightness_processor(void);
static void led_blink_processor(void);
static double angle_to_frequency(double angle);

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
st_led_page_t g_led_screen_details =
{
    /* Red control block */
    {
        000, 000,                   /* not used yet */
        {{ 60, 110}, {320, 340}},   /* blinking_box */
        {{ 60, 380}, {320, 480}},   /* brightness_box */
        50.0,                       /* angle 0% */
        50.0,                       /* brightness 0% */
        {192,  240},                /* Blinking origin */
        {238,  290},                /* Blinking touch point */
        {82,   372},                /* brightness_origin_tp */ // 110 - 380
    },

    /* Green control block */
    {
        000, 000,                   /* not used yet */
        {{380, 110}, {640, 340}},   /* blinking_box */
        {{380, 380}, {640, 480}},   /* brightness_box */
        50.0,                       /* angle 0% */
        50.0,                       /* brightness 0% */
        {516, 240},                 /* Blinking origin */
        {638, 290},                 /* Blinking touch point */
        {404, 372},                 /* brightness_origin_tp */ // 110 - 380
    },

    /* Blue control block */
    {
        000, 000,                   /* not used yet */
        {{700, 110}, {960, 340}},   /* blinking_box */
        {{700, 380}, {960, 480}},   /* brightness_box */
        50.0,                       /* angle 0% */
        50.0,                       /* brightness 0% */
        {839, 240},                 /* Blinking origin */
        {232, 290},                 /* Blinking touch point */
        {729, 372},                 /* brightness_origin_tp */
    }
};

/* 32 bit is used as this is the native size and may be faster */

static bool_t s_red_control = false;
static bool_t s_grn_control = false;
static bool_t s_blu_control = false;

static uint32_t s_led_pulse_red = 0;
static uint32_t s_led_pulse_grn = 0;
static uint32_t s_led_pulse_blu = 0;

static uint32_t s_led_red_on = 1;
static uint32_t s_led_grn_on = 1;
static uint32_t s_led_blu_on = 1;

static uint32_t counter             = 0;
static uint32_t red_brightness_sc   = 0;
static uint32_t green_brightness_sc = 0;
static uint32_t blue_brightness_sc  = 0;
static uint32_t reset_counter = 100;

static double s_brightness_scale_factor = 0.49;
static double s_blu_led_backup_angle = 50.0;
static double s_blu_led_backup_intensity = 50.0;

static double angle(double a1, double a2, double b1, double b2);
static double get_brightness(double dest, st_led_control_block_t * block);
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};
static void close_led_timers(void);


/**********************************************************************************************************************
 * Function Name: get_brightness
 * Description  : Converts horizontal position within brightness box to determine require brightness as a percentage
 * Arguments    : dest  - position on-screen
 *              : block - required colour control (red,green,blue) used to determine which brightness block to use.
 *              : Note the returned range is limited to 5% through to 5 %
 * Return Value : Representation of the current brightness requested.
 *********************************************************************************************************************/
static double get_brightness(double dest, st_led_control_block_t * block)
{
	double result = 0.0;
	double origin = 0.0;

	result = (double) (dest - block->brightness_box.top_left.x);
	origin = (double) (block->brightness_box.bottom_right.x - block->brightness_box.top_left.x);

    result = (result / origin) * 100.0;

    if (result < 5.0)
    {
        result = 5.0;
    }

    if (result > 95.0)
    {
        result = 100.0;
    }
    return (result);
}
/**********************************************************************************************************************
 End of function get_brightness
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: angle
 * Description  : returns angle from due south, always positive range 0--360 (counter clockwise)
 * Arguments    : a1 - origin x co-ordinate
 *              : a2 - origin y co-ordinate
 *              : b1 - contact point x co-ordinate
 *              : b2 - contact point y co-ordinate
 * Return Value : angle in range 50 - 310 degrees obtained between destination and 0 degrees
 *                where 0 is due south (bottom of the screen).
 *********************************************************************************************************************/
static double angle(double a1, double a2, double b1, double b2)
{
    static const double two_pi  = R_CONST_TWOPI;
    static const double rad2deg = R_CONST_RAD2DEG;

	volatile double a      = (b2 - a2);
	volatile double b      = (b1 - a1);
	double offset          = 270.0;    // make 0' point due south

	volatile double theta  = atan2(a, b);
	volatile double result = 0.0;

    if (theta < 0.0)
    {
        theta += two_pi;
    }
    result = offset + (rad2deg * theta);

    if (result > 360.0)
    {
        result = result - 360.0;
    }

    /* LIMIT TO 0 % AS ANGLE */
    if (result < 50.0)
    {
        result = 50.0;
    }

    /* LIMIT TO 100 % AS ANGLE */
    if (result > 310.0)
    {
        result = 310.0;
    }

    return result;
}



/**********************************************************************************************************************
 * Function Name: map_tp_to_hitbox
 * Description  : map transposed touch point from screen to pre-defined hit locations
 * Arguments    : x_hp - contact point x co-ordinate
 *              : y_hp - contact point y co-ordinate
 * Return Value : select the co-responding hit box that matches the location provided.
 *                -1 no matching hit box detected.
 *********************************************************************************************************************/
static int8_t map_tp_to_hitbox(uint32_t x_hp, uint32_t y_hp)
{
    int8_t selected = -1;

    if ((x_hp > g_led_screen_details.red.blinking_box.top_left.x) && (x_hp < g_led_screen_details.red.blinking_box.bottom_right.x) &&
        (y_hp > g_led_screen_details.red.blinking_box.top_left.y) && (y_hp < g_led_screen_details.red.blinking_box.bottom_right.y))
    {
        selected = RED_BLINK_RATE_SELECTED;
    }

    if ((x_hp > g_led_screen_details.green.blinking_box.top_left.x) && (x_hp < g_led_screen_details.green.blinking_box.bottom_right.x) &&
        (y_hp > g_led_screen_details.green.blinking_box.top_left.y) && (y_hp < g_led_screen_details.green.blinking_box.bottom_right.y))
    {
        selected = GREEN_BLINK_RATE_SELECTED;
    }

    if ((x_hp > g_led_screen_details.blue.blinking_box.top_left.x) && (x_hp < g_led_screen_details.blue.blinking_box.bottom_right.x) &&
        (y_hp > g_led_screen_details.blue.blinking_box.top_left.y) && (y_hp < g_led_screen_details.blue.blinking_box.bottom_right.y))
    {
        selected = BLUE_BLINK_RATE_SELECTED;
    }

    if ((x_hp > g_led_screen_details.red.brightness_box.top_left.x) && (x_hp < g_led_screen_details.red.brightness_box.bottom_right.x) &&
        (y_hp > g_led_screen_details.red.brightness_box.top_left.y) && (y_hp < g_led_screen_details.red.brightness_box.bottom_right.y))
    {
        selected = RED_BRIGHNESS_SELECTED;
    }

    if ((x_hp > g_led_screen_details.green.brightness_box.top_left.x) && (x_hp < g_led_screen_details.green.brightness_box.bottom_right.x) &&
        (y_hp > g_led_screen_details.green.brightness_box.top_left.y) && (y_hp < g_led_screen_details.green.brightness_box.bottom_right.y))
    {
        selected = GREEN_BRIGHNESS_SELECTED;
    }

    if ((x_hp > g_led_screen_details.blue.brightness_box.top_left.x) && (x_hp < g_led_screen_details.blue.brightness_box.bottom_right.x) &&
        (y_hp > g_led_screen_details.blue.brightness_box.top_left.y) && (y_hp < g_led_screen_details.blue.brightness_box.bottom_right.y))
    {
        selected = BLUE_BRIGHNESS_SELECTED;
    }
    return (selected);
}
/**********************************************************************************************************************
 End of function map_tp_to_hitbox
 *********************************************************************************************************************/
 
double angle_to_frequency(double angle)
{
    double tmp;

    /* 50 degrees corresponds to ~2Hz, 308 degrees corresponds to ~10Hz */
    tmp = angle - 50;

    // Gives value 0-260 (degrees)
    tmp /= 28.8;

    /*  into range 1-10 */
    tmp += 1;

    return (tmp);
}

/**********************************************************************************************************************
 * Function Name: led_display_menu
 * Description  : main function for controlling LCD demo (console screen and lcd display)
 * Return Value : Default 0, used for possible expansion.
 *********************************************************************************************************************/
test_fn led_display_menu(void)
{
    int8_t c = -1;
    bool_t           mode      = true;
    bool_t           new_mode  = false;
    lv_indev_data_t  data      = {};
    int8_t on_screen_selection = -1;

    volatile double result;
    volatile double origin_x;
    volatile double origin_y;
    volatile double destination_x;
    volatile double destination_y;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(s_print_buffer);

    ts_thread_enable_output(true);
    set_selected_menu_demo(LCD_FULL_BG_USER_LED);
    dsp_set_background(LCD_FULL_BG_USER_LED);

    new_mode = jlink_set_mode(mode);

    start_key_check();

    configure_led(LED_CONFIGURED_DEMO);

    while (CONNECTION_ABORT_CRTL != c)
    {
        touchpad_get_copy(&data);
        on_screen_selection = process_menu_detection(&data, false, '2');

        if (key_pressed() || (g_next_menu_selection != -1) || ((on_screen_selection != LCD_FULL_BG_POPUP_MENU) && (on_screen_selection != -1)))
        {
            if ((!g_overlay_selected) && (on_screen_selection != -1) && (on_screen_selection != '2'))
            {
                if (on_screen_selection == ' ')
                {
                    g_next_menu_selection = -1;
                }
                else
                {
                    g_next_menu_selection = on_screen_selection;
                }

                /* Pop up menu cancel detected */
                if (on_screen_selection == 'c')
                {
                    g_next_menu_selection = -1;
                }
                else
                {
                    /* Force exit */
                    c = MENU_EXIT_CRTL;
                }
            }
            else
            {
                c = (int8_t) get_detected_key();
            }

            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                break;
            }
            start_key_check();
        }

        /* only update console when there is no interaction on the panel */
        if (data.number_of_coordinates == 0)
        {
            sprintf (s_print_buffer, "%s%s", gp_hide_cursor, gp_cursor_home);
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer, MODULE_NAME, g_selected_menu);
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer, "a) Red LED blinking frequency (Hz):   %d   \r\n", (size_t) angle_to_frequency(g_led_screen_details.red.blinking_tp_angle));
            print_to_console(s_print_buffer);
            sprintf (s_print_buffer, "b) Red LED blinking intensity (%%):    %d   \r\n", (size_t) g_led_screen_details.red.brightness);
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer, "c) Green LED blinking frequency (Hz): %d   \r\n", (size_t) angle_to_frequency(g_led_screen_details.green.blinking_tp_angle));
            print_to_console(s_print_buffer);
            sprintf (s_print_buffer, "d) Green LED blinking intensity (%%):  %d   \r\n", (size_t) g_led_screen_details.green.brightness);
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer, "e) Blue LED blinking frequency (Hz):  %d   \r\n", (size_t) angle_to_frequency(g_led_screen_details.blue.blinking_tp_angle));
            print_to_console(s_print_buffer);
            sprintf (s_print_buffer, "f) Blue LED blinking intensity (%%):   %d   \r\n", (size_t) g_led_screen_details.blue.brightness);
            print_to_console(s_print_buffer);

            print_to_console(MENU_RETURN_INFO);
        }

        vTaskDelay(10);

    	/* map touch point to screen */
    	destination_x = ((double) data.point[0].x);
		destination_y = ((double) data.point[0].y);

        int8_t zone = -1; // zone -1 indicates "OUT OF BOUNDS", no zone matched
        if (data.number_of_coordinates > 0)
        {
			zone = map_tp_to_hitbox(data.point[0].x, data.point[0].y);

            if (RED_BLINK_RATE_SELECTED == zone)
            {
				origin_x = (double) g_led_screen_details.red.blinking_origin_tp.x;
				origin_y = (double) g_led_screen_details.red.blinking_origin_tp.y;

                result = angle(origin_x, origin_y, destination_x, destination_y);

                g_led_screen_details.red.blinking_tp_angle = result;
            }

            if (GREEN_BLINK_RATE_SELECTED == zone)
            {
				origin_x = (double) g_led_screen_details.green.blinking_origin_tp.x;
				origin_y = (double) g_led_screen_details.green.blinking_origin_tp.y;

                result = angle(origin_x, origin_y, destination_x, destination_y);

                g_led_screen_details.green.blinking_tp_angle = result;
            }

            if (BLUE_BLINK_RATE_SELECTED == zone)
            {
				origin_x = (double) g_led_screen_details.blue.blinking_origin_tp.x;
				origin_y = (double) g_led_screen_details.blue.blinking_origin_tp.y;

                result = angle(origin_x, origin_y, destination_x, destination_y);

                g_led_screen_details.blue.blinking_tp_angle = result;
            }

            if (RED_BRIGHNESS_SELECTED == zone)
            {
                result = get_brightness(destination_x, &g_led_screen_details.red);

                g_led_screen_details.red.brightness = result;
            }

            if (GREEN_BRIGHNESS_SELECTED == zone)
            {
                result = get_brightness(destination_x, &g_led_screen_details.green);

                g_led_screen_details.green.brightness = result;
            }

            if (BLUE_BRIGHNESS_SELECTED == zone)
            {
                result = get_brightness(destination_x, &g_led_screen_details.blue);

                g_led_screen_details.blue.brightness = result;
            }
        }
    }

    jlink_restore_mode(new_mode);

    /* Restore LED configuration */
    configure_led(LED_CONFIGURED_NORMAL);
    set_selected_menu_demo(LCD_FULL_BG_KIT_INFORMATION);

    return (0);
}
/**********************************************************************************************************************
 End of function led_display_menu
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: close_led_timers
 * Description  : Helper function to close pwm timer
 * Return Value : NONE
 *********************************************************************************************************************/
 static void close_led_timers(void)
{
    if (((gpt_instance_ctrl_t *) led_pulse_timer.p_ctrl)->open != 0)
    {
        R_GPT_Close(led_pulse_timer.p_ctrl);
    }

    if (((gpt_instance_ctrl_t *) led_brightness_timer.p_ctrl)->open != 0)
    {
        R_GPT_Close(led_brightness_timer.p_ctrl);
    }
}
/**********************************************************************************************************************
 End of function close_led_timers
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: configure_led
 * Description  : Manages the different modes for the software pwm controlling 3 led's
 * Argument     : desired mode
 *                LED_UNCONFIGURED           - uninitialised state
 *                LED_CONFIGURED_DEMO        - led demo mode (option 2) all 3 leds are controllable by lcd I/F
 *                LED_CONFIGURED_EXIT_DEMO   - tranisition from led demo back to normal operation
 *                LED_CONFIGURED_NORMAL      - initialise in normal operation blue led is controlled by user switches,
 *                                             green led is on and red led is off.
 *                LED_NORMAL_UPDATE          - process update to blue led state
 * Return Value : NONE
 *********************************************************************************************************************/
void configure_led(e_led_configuration_t cfg)
{
    switch (cfg)
    {
        case LED_CONFIGURED_NORMAL:
            s_red_control = false;
            s_grn_control = false;
            s_blu_control = true;

			s_blu_led_backup_angle = g_led_screen_details.blue.blinking_tp_angle;
			s_blu_led_backup_intensity = g_led_screen_details.blue.brightness;

			/* red default (off) */
			R_IOPORT_PinWrite (g_ioport.p_ctrl, RED_LED_PORT, BSP_IO_LEVEL_LOW);

			/* green default (on) */
			R_IOPORT_PinWrite (g_ioport.p_ctrl, GREEN_LED_PORT, BSP_IO_LEVEL_HIGH);

            close_led_timers();

            /* configure the blue LED for PWM operation */
            R_IOPORT_PinCfg(g_ioport.p_ctrl, BLUE_LED_PORT, (uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_GPT1);
		    break;

        case LED_CONFIGURED_DEMO:
            s_red_control = true;
            s_grn_control = true;
            s_blu_control = true;

            g_led_screen_details.blue.blinking_tp_angle = s_blu_led_backup_angle;
            g_led_screen_details.blue.brightness        = s_blu_led_backup_intensity;

            /* Reset LED setting to synchronise */
            counter = 0;
            s_led_red_on = 0;
            s_led_grn_on = 0;
            s_led_blu_on = 0;

            s_led_pulse_red = 0;
            s_led_pulse_grn = 0;
            s_led_pulse_blu = 0;

            /* configure the blue LED for GTPIO operation */
            R_IOPORT_PinCfg(g_ioport.p_ctrl, BSP_IO_PORT_06_PIN_00, (uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW);

            R_IOPORT_PinWrite (g_ioport.p_ctrl, RED_LED_PORT, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite (g_ioport.p_ctrl, GREEN_LED_PORT, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BLUE_LED_PORT, BSP_IO_LEVEL_LOW);

            R_GPT_Open(led_pulse_timer.p_ctrl, led_pulse_timer.p_cfg);
            R_GPT_Start(led_pulse_timer.p_ctrl);

            R_GPT_Open(led_brightness_timer.p_ctrl, led_brightness_timer.p_cfg);
            R_GPT_Start(led_brightness_timer.p_ctrl);
            break;

        case LED_NORMAL_UPDATE:
			if (!s_red_control) /* only apply in NORMAL node not DEMO */
            {
		        close_led_timers();
			}
            break;

        case LED_UNCONFIGURED:
        default:
            R_GPT_Close(led_pulse_timer.p_ctrl);
            R_GPT_Close(led_brightness_timer.p_ctrl);
        break;
    }
}
/**********************************************************************************************************************
 End of function configure_led
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: map_angle_to_screen
 * Description  : Converts angle into screen co-ordinates and draws line from origin to calculated point.
 *                Note the radius of the indicator has been pre-determined and defined as R_CONST_INDICATOR_RADIUS
 *                The radians to degrees conversion (1 Rad = ~ 57.296 degrees) defined as R_CONST_RAD2DEG
 * Argument       gp_d2_handle - application pointer to address of a device structure without knowing its internal layout.
 *                angle_i   - desired angle as a whole integer
 *                origin_tp - location in screen co-ordinates of the origin of the indicator,
 * Return Value : NONE
 *********************************************************************************************************************/
static void map_angle_to_screen(d2_device *handle, uint32_t angle_i, lv_point_t *origin_tp)
{
    double destination_x = 0.0;
    double destination_y = 0.0;
    double radius        = R_CONST_INDICATOR_RADIUS;
    double rad2deg       = R_CONST_RAD2DEG;
    double ta            = 1.0;

    lv_point_t destination_tp;

    if (angle_i <= 90.0)
    {
        ta = 90.0 - ((double) angle_i * 1.0);
    }

    if ((angle_i > 90) && (angle_i <= 180))
    {
        ta = ((double) (angle_i - 90) * 1.0);
    }

    if ((angle_i > 180) && (angle_i <= 270))
    {
        ta = 90.0 - ((double) (angle_i - 180) * 1.0);
    }

    if (angle_i > 270)
    {
        ta = ((double) (angle_i - 270) * 1.0);
    }

    ta = ta / rad2deg;

    destination_x = cos(ta) * radius;
    destination_y = sin(ta) * radius;

    if (angle_i <= 90)
    {
        destination_tp.x = origin_tp->x - (uint32_t) destination_x;
        destination_tp.y = origin_tp->y + (uint32_t) destination_y;
    }

    if ((angle_i > 90) && (angle_i <= 180))
    {
        destination_tp.x = origin_tp->x - (uint32_t) destination_x;
        destination_tp.y = origin_tp->y - (uint32_t) destination_y;
    }

    if ((angle_i > 180) && (angle_i <= 270))
    {
        destination_tp.x = origin_tp->x + (uint32_t) destination_x;
        destination_tp.y = origin_tp->y - (uint32_t) destination_y;
    }

    if (angle_i > 270)
    {
        destination_tp.x = origin_tp->x + (uint32_t) destination_x;
        destination_tp.y = origin_tp->y + (uint32_t) destination_y;
    }

    d2_renderline(handle, (d2_point) ((origin_tp->x) << 4), (d2_point) ((origin_tp->y) << 4),
                 (d2_point) ((destination_tp.x) << 4), (d2_point) ((destination_tp.y) << 4), (d2_point) (LED_LINE_THICKNESS << 4), 0);

}
/**********************************************************************************************************************
 End of function map_angle_to_screen
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: menu_led_update_screen
 * Description  : Helper function used from the display thread to draw the led indicators.
 * Argument     : gp_d2_handle - application pointer to address of a device structure without knowing its internal layout.
 * Return Value : NONE
 *********************************************************************************************************************/
void menu_led_update_screen(d2_device *handle)
{
    d2_color col     = 0x00FFFFFF;
    double scalex    = R_CONST_SCALEX;
    uint32_t brightness_indicator_length = R_CONST_BRIGHTNESS_INDICATOR_LENGTH;

    uint32_t blink_i = 0;  // angle as uint32 to represent position of blinking indicator

    lv_point_t cursor_pos = {TITLE_LEFT_EDGE, 18};
    user_font_title_draw_line(&cursor_pos, " USER LED CONTROL");

    cursor_pos.x  = TITLE_LEFT_EDGE;
    cursor_pos.y += TITLE_LINE_SPACING;

    DRAW_TITLE_FONT_UNDERLINE;

    lv_point_t origin_tp = {g_led_screen_details.red.blinking_origin_tp.x,
                                     g_led_screen_details.red.blinking_origin_tp.y};
    lv_point_t destination_tp;

    lv_indev_data_t data = {};
    touchpad_get_copy(&data);

    d2_setcolor(gp_d2_handle, 0, col);

    // Draw RED Blinking Rate
    blink_i = (uint32_t) g_led_screen_details.red.blinking_tp_angle;
    origin_tp.x = g_led_screen_details.red.blinking_origin_tp.x;
    origin_tp.y = g_led_screen_details.red.blinking_origin_tp.y;

    map_angle_to_screen(gp_d2_handle, blink_i, &origin_tp);

    // Draw GREEN Blinking Rate
    blink_i = (uint32_t) g_led_screen_details.green.blinking_tp_angle;
    origin_tp.x = g_led_screen_details.green.blinking_origin_tp.x;
    origin_tp.y = g_led_screen_details.green.blinking_origin_tp.y;

    map_angle_to_screen(gp_d2_handle, blink_i, &origin_tp);

    // Draw BLUE Blinking Rate
    blink_i = (uint32_t) g_led_screen_details.blue.blinking_tp_angle;
    origin_tp.x = g_led_screen_details.blue.blinking_origin_tp.x;
    origin_tp.y = g_led_screen_details.blue.blinking_origin_tp.y;

    map_angle_to_screen(gp_d2_handle, blink_i, &origin_tp);

    // Draw RED Brightness
    origin_tp.x  = g_led_screen_details.red.brightness_origin_tp.x;
    origin_tp.x += (uint32_t) (scalex * g_led_screen_details.red.brightness);
    origin_tp.y = brightness_indicator_length + g_led_screen_details.red.brightness_origin_tp.y;

    destination_tp.x = origin_tp.x;
    destination_tp.y = 10 + g_led_screen_details.red.brightness_origin_tp.y;

    d2_renderline(handle, (d2_point) ((origin_tp.x) << 4), (d2_point) ((origin_tp.y) << 4),
                 (d2_point) ((destination_tp.x) << 4), (d2_point) ((destination_tp.y) << 4), (d2_point) (LED_LINE_THICKNESS << 4), 0);

    // Draw GREEN Brightness
    origin_tp.x  = g_led_screen_details.green.brightness_origin_tp.x;
    origin_tp.x += (uint32_t) (scalex * g_led_screen_details.green.brightness);
    origin_tp.y = brightness_indicator_length + g_led_screen_details.green.brightness_origin_tp.y;

    destination_tp.x = origin_tp.x;
    destination_tp.y = 10 + g_led_screen_details.green.brightness_origin_tp.y;

    d2_renderline(handle, (d2_point) ((origin_tp.x) << 4), (d2_point) ((origin_tp.y) << 4),
                 (d2_point) ((destination_tp.x) << 4), (d2_point) ((destination_tp.y) << 4), (d2_point) (LED_LINE_THICKNESS << 4), 0);

    // Draw BLUE Brightness
    origin_tp.x  = g_led_screen_details.blue.brightness_origin_tp.x;
    origin_tp.x += (uint32_t) (scalex * g_led_screen_details.blue.brightness);
    origin_tp.y = brightness_indicator_length + g_led_screen_details.blue.brightness_origin_tp.y;

    destination_tp.x = origin_tp.x;
    destination_tp.y = 10 + g_led_screen_details.blue.brightness_origin_tp.y;

    d2_renderline(handle, (d2_point) ((origin_tp.x) << 4), (d2_point) ((origin_tp.y) << 4),
                 (d2_point) ((destination_tp.x) << 4), (d2_point) ((destination_tp.y) << 4), (d2_point) (LED_LINE_THICKNESS << 4), 0);
}
/**********************************************************************************************************************
 End of function menu_led_update_screen
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Function Name: led_blink_processor
 * Description  : Management for software pwm to determine led on/off toggle time.
 *                All 3 user leds are driven by 1 hardware timer.
 * Return Value : NONE
 *********************************************************************************************************************/
void led_blink_processor(void)
{
    if (s_led_pulse_red-- == 0)
    {
        s_led_pulse_red = R_CONST_BLINK_RATE_DELAY_INDICATOR - (uint32_t) g_led_screen_details.red.blinking_tp_angle;

        /* Toggle red LED */
        s_led_red_on = !s_led_red_on;
        R_IOPORT_PinWrite (g_ioport.p_ctrl, RED_LED_PORT, (bsp_io_level_t) s_led_red_on);
    }

    if (s_led_pulse_grn-- == 0)
    {
        s_led_pulse_grn = R_CONST_BLINK_RATE_DELAY_INDICATOR - (uint32_t) g_led_screen_details.green.blinking_tp_angle;

        /* Toggle green LED */
        s_led_grn_on = !s_led_grn_on;
        R_IOPORT_PinWrite (g_ioport.p_ctrl, GREEN_LED_PORT, (bsp_io_level_t) s_led_grn_on);
    }

    if (s_led_pulse_blu-- == 0)
    {
        s_led_pulse_blu = R_CONST_BLINK_RATE_DELAY_INDICATOR - (uint32_t) g_led_screen_details.blue.blinking_tp_angle;

        /* Toggle blue LED */
        s_led_blu_on = !s_led_blu_on;
        R_IOPORT_PinWrite (g_ioport.p_ctrl, BLUE_LED_PORT, (bsp_io_level_t) s_led_blu_on);
    }
}
/**********************************************************************************************************************
 End of function led_blink_processor
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: led_brightness_processor
 * Description  : Management for software pwm to determine led brightness.
 *                All 3 user leds are driven by 1 hardware timer.
 * Return Value : NONE
 *********************************************************************************************************************/
void led_brightness_processor(void)
{
    if (counter-- == 0)
    {
        counter = reset_counter;
        red_brightness_sc   = (uint32_t) (g_led_screen_details.red.brightness   * s_brightness_scale_factor);
        green_brightness_sc = (uint32_t) (g_led_screen_details.green.brightness * s_brightness_scale_factor);
        blue_brightness_sc  = (uint32_t) (g_led_screen_details.blue.brightness  * s_brightness_scale_factor);
    }

    if (s_red_control)
    {
        if (s_led_red_on)
        {
            if (counter > red_brightness_sc)
            {
                R_IOPORT_PinWrite (g_ioport.p_ctrl, RED_LED_PORT, BSP_IO_LEVEL_LOW);
            }
            else
            {
                R_IOPORT_PinWrite (g_ioport.p_ctrl, RED_LED_PORT, BSP_IO_LEVEL_HIGH);
            }
        }
        else
        {
            R_IOPORT_PinWrite (g_ioport.p_ctrl, RED_LED_PORT, BSP_IO_LEVEL_LOW);
        }
    }

    if (s_grn_control)
    {
        if (s_led_grn_on)
        {
            if (counter > green_brightness_sc)
            {
                R_IOPORT_PinWrite (g_ioport.p_ctrl, GREEN_LED_PORT, BSP_IO_LEVEL_LOW);
            }
            else
            {
                R_IOPORT_PinWrite (g_ioport.p_ctrl, GREEN_LED_PORT, BSP_IO_LEVEL_HIGH);
            }
        }
        else
        {
            R_IOPORT_PinWrite (g_ioport.p_ctrl, GREEN_LED_PORT, BSP_IO_LEVEL_LOW);
        }
    }

    if (s_led_blu_on)
    {
        if (counter > blue_brightness_sc)
        {
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BLUE_LED_PORT, BSP_IO_LEVEL_LOW);
        }
        else
        {
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BLUE_LED_PORT, BSP_IO_LEVEL_HIGH);
        }
    }
    else
    {
        R_IOPORT_PinWrite (g_ioport.p_ctrl, BLUE_LED_PORT, BSP_IO_LEVEL_LOW);
    }
}
/**********************************************************************************************************************
 End of function led_brightness_processor
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: led_brightness_timer_callback
 * Description  : timer interrupt callback
 * Return Value : p_args: type of interrupt
 *********************************************************************************************************************/
void led_brightness_timer_callback(timer_callback_args_t *p_args)
{
    UNUSED_PARAM(p_args);

    led_brightness_processor();
}
/**********************************************************************************************************************
 End of function led_brightness_timer_callback
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: led_pulse_timer_callback
 * Description  : timer interrupt callback
 * Return Value : p_args: type of interrupt
 *********************************************************************************************************************/
void led_pulse_timer_callback(timer_callback_args_t *p_args)
{
    UNUSED_PARAM(p_args);

    led_blink_processor();
}
/**********************************************************************************************************************
 End of function led_pulse_timer_callback
 *********************************************************************************************************************/
