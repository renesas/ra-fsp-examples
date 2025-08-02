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

#include "tp_thread_entry.h"

#include "common_init.h"
#include "common_utils.h"

#include "jlink_console.h"
#include "menu_led.h"

#include "images/user_font_title/user_font_title_if.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define MODULE_NAME             "\r\n%d. User LED Control\r\n\r\n"
#define PRINT_COORDINATES       "TP %d: %4d, %4d     \r\n"
#define PRINT_NO_TOUCH          "TP %d:     ,        \r\n"
#define LED_LINE_THICKNESS      (4)


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

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
st_led_page_t g_led_screen_details =
{
    /* Red control block */
    {
        000, 000,                   /* not used yet */
        {{100, 40}, {300, 260}},    /* blinking_box */
        {{310, 65}, {405, 255}},    /* brightness_box */
        50.0,                       /* angle 0% */
        50.0,                       /* brightness 0% */
        {148,  200},                /* Blinking origin */
        {238,  290},                /* Blinking touch point */
        {55,   300},                /* brightness_origin_tp */ // 110 - 380
    },

    /* Green control block */
    {
        000, 000,                   /* not used yet */
        {{100, 300}, {300, 500}},   /* blinking_box */
        {{300, 320}, {500, 500}},   /* brightness_box */
        50.0,                       /* angle 0% */
        50.0,                       /* brightness 0% */
        {398, 200},                 /* Blinking origin */
        {638, 290},                 /* Blinking touch point */
        {310, 300},                 /* brightness_origin_tp */ // 110 - 380
    },

    /* Blue control block */
    {
        000, 000,                   /* not used yet */
        {{100, 560}, {300, 760}},   /* blinking_box */
        {{300, 560}, {500, 755}},   /* brightness_box */
        50.0,                       /* angle 0% */
        50.0,                       /* brightness 0% */
        {652, 200},                 /* Blinking origin */
        { 232,  290},               /* Blinking touchpoint */
        {560, 300},                 /* brightness_origin_tp */
    }
};

uint32_t g_led_brightness_red = 0;
uint32_t g_led_brightness_grn = 0;
uint32_t g_led_brightness_blu = 0;
uint32_t g_led_pulse_red = 0;
uint32_t g_led_pulse_grn = 0;
uint32_t g_led_pulse_blu = 0;

/* 32 bit is used as this is the native size and may be faster */
uint32_t g_led_red_on = 1;
uint32_t g_led_grn_on = 1;
uint32_t g_led_blu_on = 1;


double brightness_scale_factor = 0.49;

uint32_t reset_counter = 100;

uint32_t counter             = 0;
uint32_t red_brightness_sc   = 0;
uint32_t green_brightness_sc = 0;
uint32_t blue_brightness_sc  = 0;
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

static double angle(double a1, double a2, double b1, double b2);
static double get_brightness(double dest, st_led_control_block_t * block);
void led_brightness_processor(void);
void led_blink_processor(void);
void configure_led(e_led_configuration_t cfg);
void close_led_timers(void);
double angle_to_frequency(double angle);



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

    result = (double) (dest - block->brightness_box.top_left.y);
    origin = (double) (block->brightness_box.bottom_right.y - block->brightness_box.top_left.y);

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
    static const double TWOPI = 6.2831853071795865;
    static const double RAD2DEG = 57.2957795130823209;

    volatile double a      = (b2 - a2);
    volatile double b      = (b1 - a1);
    double offset          = 270.0;    // make 0' point due south

    volatile double theta  = atan2(a, b);
    volatile double result = 0.0;

    if (theta < 0.0)
    {
        theta += TWOPI;
    }
    result = offset + (RAD2DEG * theta);

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
    if (result > 307.0)
    {
        result = 308.0;
    }

    return result;
}

int8_t map_tp_to_hitbox(uint32_t x_hp, uint32_t y_hp);
void menu_led_update_screen(d2_device *d2_handle);

/**********************************************************************************************************************
 * Function Name: map_tp_to_hitbox
 * Description  : map transposed touch point from screen to pre-defined hit locations
 * Arguments    : x_hp - contact point x co-ordinate
 *              : y_hp - contact point y co-ordinate
 * Return Value : select the co-responding hitbox that matches the location provided.
 *                -1 no matching hitbox detected.
 *********************************************************************************************************************/
int8_t map_tp_to_hitbox(uint32_t x_hp, uint32_t y_hp)
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
    tmp /= 32.25;
    tmp += 2;

    return (tmp);
}

/**********************************************************************************************************************
 * Function Name: led_display_menu
 * Description  : main function for controlling LCD demo (console screen and lcd display)
 * Return Value : Default 0, used for possible expansion.
 *********************************************************************************************************************/
test_fn led_display_menu(void)
{
    int8_t           c         = -1;
    volatile bool_t  mode      = true;
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
    dsp_set_background(LCD_FULL_BG_USER_LED);

    new_mode = jlink_set_mode(mode);

    start_key_check();

    configure_led(LED_CONFIGURED_DEMO);

    while (CONNECTION_ABORT_CRTL != c)
    {
        touchpad_get_copy(&data);
        on_screen_selection = process_menu_detection(&data, false, '2');

        if (key_pressed() || ((on_screen_selection != LCD_FULL_BG_POPUP_MENU) && (on_screen_selection != -1)))
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

                if (0 == c)
                {
                    c = -1;
                }
            }

            if ((MENU_EXIT_CRTL == c) || (CONNECTION_ABORT_CRTL == c))
            {
                vTaskDelay(100);
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

            sprintf (s_print_buffer,"a) Red LED blinking frequency (Hz):   %d   \r\n", (size_t) angle_to_frequency(g_led_screen_details.red.blinking_tp_angle));
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer,"b) Red LED blinking intensity (%%):    %2d   \r\n", (size_t) g_led_screen_details.red.brightness);
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer,"c) Green LED blinking frequency (Hz): %d   \r\n", (size_t) angle_to_frequency(g_led_screen_details.green.blinking_tp_angle));
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer,"d) Green LED blinking intensity (%%):  %2d   \r\n", (size_t) g_led_screen_details.green.brightness);
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer,"e) Blue LED blinking frequency (Hz):  %d   \r\n", (size_t) angle_to_frequency(g_led_screen_details.blue.blinking_tp_angle));
            print_to_console(s_print_buffer);

            sprintf (s_print_buffer,"f) Blue LED blinking intensity (%%):   %2d   \r\n", (size_t) g_led_screen_details.blue.brightness);
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
            zone = map_tp_to_hitbox (data.point[0].y, data.point[0].x);

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

    return (0);
}
/**********************************************************************************************************************
 End of function led_display_menu
 *********************************************************************************************************************/

static bool_t s_red_control = false;
static bool_t s_grn_control = false;
static bool_t s_blu_control = false;

static double s_blu_led_backup_angle = 50.0;
static double s_blu_led_backup_intensity = 50.0;

/**********************************************************************************************************************
 * Function Name: update_frequency
 * Description  : convert the three settings for led frequency (used outside of led demo) into inpus to the blue flash
 *              : required as the sw pwm is used for blue led throughout this project.
 * Argument     : setting - desired setting values 1,5,10.
 * Return Value : NONE
 *********************************************************************************************************************/
static void update_frequency(uint32_t setting)
{
    switch (setting)
    {
        case 1:
            default:
            g_led_screen_details.blue.blinking_tp_angle = 60.0;
            break;

        case 5:
            g_led_screen_details.blue.blinking_tp_angle = 145.0;
            break;

        case 10:
            g_led_screen_details.blue.blinking_tp_angle = 290.0;
            break;
    }
}
/**********************************************************************************************************************
 End of function update_frequency
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: close_led_timers
 * Description  : Helper function to close pwm timer
 * Return Value : NONE
 *********************************************************************************************************************/
 void close_led_timers()
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
 * Description  : MAnages the different modes for the software pwm controlling 3 led's
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

            /* Update structure directly in function as we are modifying a global */
            update_frequency(g_pwm_rates_data[g_board_status.led_frequency]);
            g_led_screen_details.blue.brightness = g_pwm_dcs_data[g_board_status.led_intensity];

            /* red default (off) */
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_05, BSP_IO_LEVEL_LOW);

            /* grn default (on) */
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_06, BSP_IO_LEVEL_HIGH);

            close_led_timers();

            /* configure the blue LED for PWM operation */
            R_IOPORT_PinCfg(g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, (uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PERIPHERAL_PIN | (uint32_t) IOPORT_PERIPHERAL_GPT1);

            vTaskResume(blinky_thread);
            break;

        case LED_CONFIGURED_DEMO:
            vTaskSuspend(blinky_thread);
            s_red_control = true;
            s_grn_control = true;
            s_blu_control = true;

            g_led_screen_details.blue.blinking_tp_angle = s_blu_led_backup_angle;
            g_led_screen_details.blue.brightness        = s_blu_led_backup_intensity;

            /* Reset LED setting to synchronise */
            counter = 0;
            g_led_red_on = 0;
            g_led_grn_on = 0;
            g_led_blu_on = 0;

            g_led_pulse_red = 0;
            g_led_pulse_grn = 0;
            g_led_pulse_blu = 0;

            /* configure the blue LED for GTPIO operation */
            R_IOPORT_PinCfg(g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, (uint32_t) IOPORT_CFG_DRIVE_HIGH | (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t) IOPORT_CFG_PORT_OUTPUT_LOW);

            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_05, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_06, BSP_IO_LEVEL_LOW);
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, BSP_IO_LEVEL_LOW);

            R_GPT_Open(led_pulse_timer.p_ctrl, led_pulse_timer.p_cfg);
            R_GPT_Start(led_pulse_timer.p_ctrl);

            R_GPT_Open(led_brightness_timer.p_ctrl, led_brightness_timer.p_cfg);
            R_GPT_Start(led_brightness_timer.p_ctrl);
            break;

        case LED_NORMAL_UPDATE:
            if (!s_red_control) /* only apply in NORMAL node not DEMO */
            {
                /* Update structure directly in funcrion as we are modifying a global */
                update_frequency(g_pwm_rates_data[g_board_status.led_frequency]);
                g_led_screen_details.blue.brightness = g_pwm_dcs_data[g_board_status.led_intensity];
                close_led_timers();
            }
            break;

        case LED_UNCONFIGURED:
        default:
            close_led_timers();
        break;
    }
}
/**********************************************************************************************************************
 End of function configure_led
 *********************************************************************************************************************/
 
uint32_t blink_i = 0;           // angle rounded down to uint32 to represent position of blinking indicator
double scalex = 1.78;           // scale factor to convert % to screen position on brightness slider

static const double RAD2DEG = 57.2957795130823209;  // I hope this is accurate enough
double ext_radius = 90.0;

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
static void map_angle_to_screen(d2_device *d2_handle, uint32_t angle_i, lv_point_t *origin_tp)
{
    double destination_x = 0.0;
    double destination_y = 0.0;
    double radius = ext_radius;
    double ta = 1.0;

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

    ta = ta / RAD2DEG;

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

    d2_renderline(d2_handle, (d2_point) ((origin_tp->x) << 4), (d2_point) ((origin_tp->y) << 4),
                 (d2_point) ((destination_tp.x) << 4), (d2_point) ((destination_tp.y) << 4), (d2_point) (LED_LINE_THICKNESS << 4), 0);

}
/**********************************************************************************************************************
 End of function map_angle_to_screen
 *********************************************************************************************************************/
 
/**********************************************************************************************************************
 * Function Name: menu_led_update_screen
 * Description  : Function called by call_user_draw in display_thread_display.c whenever the screen for
 *                LCD_FULL_BG_USER_LED is required.
 * Argument     : d2_handle - device pointer (see: <d2_opendevice>)
 * Return Value : NONE
 *********************************************************************************************************************/
void menu_led_update_screen(d2_device *d2_handle)
{
    d2_color col = 0x00FFFFFF;

    lv_point_t cursor_pos = {TITLE_LEFT_EDGE, 6};
    user_font_title_draw_line(&cursor_pos, " USER LED CONTROL");
    graphics_reset_blit_buffer();

    lv_point_t origin_tp = {g_led_screen_details.red.blinking_origin_tp.x,
                                     g_led_screen_details.red.blinking_origin_tp.y};
    lv_point_t destination_tp;

    lv_indev_data_t data = {};
    touchpad_get_copy(&data);

    d2_setcolor(d2_handle, 0, col);

    // Draw RED Blinking Rate
    blink_i = (uint32_t) g_led_screen_details.red.blinking_tp_angle;
    origin_tp.x = g_led_screen_details.red.blinking_origin_tp.x;
    origin_tp.y = g_led_screen_details.red.blinking_origin_tp.y;

    map_angle_to_screen(d2_handle, blink_i, &origin_tp);

    // Draw GREEN Blinking Rate
    blink_i = (uint32_t) g_led_screen_details.green.blinking_tp_angle;
    origin_tp.x = g_led_screen_details.green.blinking_origin_tp.x;
    origin_tp.y = g_led_screen_details.green.blinking_origin_tp.y;

    map_angle_to_screen(d2_handle, blink_i, &origin_tp);

    // Draw BLUE Blinking Rate
    blink_i = (uint32_t) g_led_screen_details.blue.blinking_tp_angle;
    origin_tp.x = g_led_screen_details.blue.blinking_origin_tp.x;
    origin_tp.y = g_led_screen_details.blue.blinking_origin_tp.y;

    map_angle_to_screen(d2_handle, blink_i, &origin_tp);

    // Draw RED Brightness
    origin_tp.x  = g_led_screen_details.red.brightness_origin_tp.x;
    origin_tp.x += (uint32_t) (scalex * g_led_screen_details.red.brightness);
    origin_tp.y = 50 + g_led_screen_details.red.brightness_origin_tp.y;

    destination_tp.x = origin_tp.x;
    destination_tp.y = 10 + g_led_screen_details.red.brightness_origin_tp.y;

    d2_renderline(d2_handle, (d2_point) ((origin_tp.x) << 4), (d2_point) ((origin_tp.y) << 4),
                 (d2_point) ((destination_tp.x) << 4), (d2_point) ((destination_tp.y) << 4), (d2_point) (LED_LINE_THICKNESS << 4), 0);

    // Draw GREEN Brightness
    origin_tp.x  = g_led_screen_details.green.brightness_origin_tp.x;
    origin_tp.x += (uint32_t) (scalex * g_led_screen_details.green.brightness);
    origin_tp.y = 50 + g_led_screen_details.green.brightness_origin_tp.y;

    destination_tp.x = origin_tp.x;
    destination_tp.y = 10 + g_led_screen_details.green.brightness_origin_tp.y;

    d2_renderline(d2_handle, (d2_point) ((origin_tp.x) << 4), (d2_point) ((origin_tp.y) << 4),
                 (d2_point) ((destination_tp.x) << 4), (d2_point) ((destination_tp.y) << 4), (d2_point) (LED_LINE_THICKNESS << 4), 0);

    // Draw BLUE Brightness
    origin_tp.x  = g_led_screen_details.blue.brightness_origin_tp.x;
    origin_tp.x += (uint32_t) (scalex * g_led_screen_details.blue.brightness);
    origin_tp.y = 50 + g_led_screen_details.blue.brightness_origin_tp.y;

    destination_tp.x = origin_tp.x;
    destination_tp.y = 10 + g_led_screen_details.blue.brightness_origin_tp.y;

    d2_renderline(d2_handle, (d2_point) ((origin_tp.x) << 4), (d2_point) ((origin_tp.y) << 4),
                 (d2_point) ((destination_tp.x) << 4), (d2_point) ((destination_tp.y) << 4), (d2_point) (LED_LINE_THICKNESS << 4), 0);
}
/**********************************************************************************************************************
 End of function menu_led_update_screen
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
 
static uint32_t roff = 40;

/**********************************************************************************************************************
 * Function Name: led_blink_processor
 * Description  : Management for software pwm to determine led on/off toggle time.
 *                All 3 user leds are driven by 1 hardware timer.
 * Return Value : NONE
 *********************************************************************************************************************/
void led_blink_processor()
{
    if (g_led_pulse_red-- == 0)
    {
        g_led_pulse_red = 325 - (uint32_t) g_led_screen_details.red.blinking_tp_angle;
        g_led_pulse_red += roff;

        /* Toggle red LED */
        g_led_red_on = !g_led_red_on;
        R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_05, (bsp_io_level_t) g_led_red_on);
    }

    if (g_led_pulse_grn-- == 0)
    {
        g_led_pulse_grn = 325 - (uint32_t) g_led_screen_details.green.blinking_tp_angle;
        g_led_pulse_grn += roff;

        /* Toggle green LED */
        g_led_grn_on = !g_led_grn_on;
        R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_06, (bsp_io_level_t) g_led_grn_on);
    }

    if (g_led_pulse_blu-- == 0)
    {
        g_led_pulse_blu = 325 - (uint32_t) g_led_screen_details.blue.blinking_tp_angle;
        g_led_pulse_blu += roff;

        /* Toggle blue LED */
        g_led_blu_on = !g_led_blu_on;
        R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, (bsp_io_level_t) g_led_blu_on);
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
void led_brightness_processor()
{
    if (counter-- == 0)
    {
        counter = reset_counter;
        red_brightness_sc = (uint32_t) (g_led_screen_details.red.brightness * brightness_scale_factor);
        green_brightness_sc = (uint32_t) (g_led_screen_details.green.brightness * brightness_scale_factor);
        blue_brightness_sc = (uint32_t) (g_led_screen_details.blue.brightness * brightness_scale_factor);
    }

    if (s_red_control)
    {
        if (g_led_red_on)
        {
            if (counter > red_brightness_sc)
            {
                R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_05, BSP_IO_LEVEL_LOW);
            }
            else
            {
                R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_05, BSP_IO_LEVEL_HIGH);
            }
        }
        else
        {
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_05, BSP_IO_LEVEL_LOW);
        }
    }

    if (s_grn_control)
    {
        if (g_led_grn_on)
        {
            if (counter > green_brightness_sc)
            {
                R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_06, BSP_IO_LEVEL_LOW);
            }
            else
            {
                R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_06, BSP_IO_LEVEL_HIGH);
            }
        }
        else
        {
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_04_PIN_06, BSP_IO_LEVEL_LOW);
        }
    }

    if (g_led_blu_on)
    {
        if (counter > blue_brightness_sc)
        {
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, BSP_IO_LEVEL_LOW);
        }
        else
        {
            R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, BSP_IO_LEVEL_HIGH);
        }
    }
    else
    {
        R_IOPORT_PinWrite (g_ioport.p_ctrl, BSP_IO_PORT_02_PIN_05, BSP_IO_LEVEL_LOW);
    }
}
/**********************************************************************************************************************
 End of function led_brightness_processor
 *********************************************************************************************************************/
 
