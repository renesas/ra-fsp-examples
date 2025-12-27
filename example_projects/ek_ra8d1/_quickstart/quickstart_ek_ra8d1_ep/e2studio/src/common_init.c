/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : common_init.c
 * Description  : Common init function.
 **********************************************************************************************************************/

#include "common_init.h"
#include "r_ioport.h"
#include "bsp_api.h"
#include "bg_font_18_full.h"
#include "fg_font_22_full.h"

#define NUM_RATES            (sizeof(pwm_rates) / sizeof(pwm_rates[0]))   /*  */
#define NUM_DCS              (sizeof(pwm_dcs) / sizeof(pwm_dcs[0]))       /*  */
#define NUM_SWITCH           (sizeof(s_irq_pins) / sizeof(s_irq_pins[0])) /*  */

#define SW_1_5_DETECT_PIN    BSP_IO_PORT_07_PIN_01                        // ETH_B_REFCLK_CAM_D5

/* Enable SW toggling of the LED. ie. PWM via interrupt handler */

// #define USE_SW_TIMER          (1)

// uint8_t ucHeap[ configTOTAL_HEAP_SIZE ]  BSP_ALIGN_VARIABLE(8);

const char_t * const gp_cursor_store     = "\x1b[s";
const char_t * const gp_cursor_restore   = "\x1b[u";
const char_t * const gp_cursor_temp      = "\x1b[8;41H\x1b[K";
const char_t * const gp_cursor_frequency = "\x1b[9;41H\x1b[K";
const char_t * const gp_cursor_intensity = "\x1b[10;41H\x1b[K";

const char_t * const gp_red_fg    = "\x1B[91m";
const char_t * const gp_orange_fg = "\x1B[38;5;208m";
const char_t * const gp_green_fg  = "\x1b[32m";
const char_t * const gp_white_fg  = "\x1b[37m";

const char_t * const gp_clear_screen = "\x1b[2J";
const char_t * const gp_cursor_home  = "\x1b[H";

bool_t  do_detection;
bool_t  do_classification;
int32_t s_mode_selected;

extern bool_t initialise_data_flash(void);

bool_t is_sdram_in_error(void);
bool_t is_startup_mode_in_error(void);

#if BSP_FEATURE_SRAM_SRAMWTSC_WAIT_CYCLE_ENABLE == 0
 #error BSP_FEATURE_SRAM_SRAMWTSC_WAIT_CYCLE_ENABLE is not set in /ra/fsp/src/bsp/mcu/ra8d1/bsp_feature.h
#endif

static uint32_t s_duty = 1;

int32_t g_curr_led_freq = BLINK_FREQ_1HZ;

static st_irq_pins_t s_irq_pins[] =
{
    {&g_external_irq10},
    {&g_external_irq9 }
};

st_pwm_pins_t g_pwm_pins[] =
{
    {&g_gpt_blue,                 GPT_IO_PIN_GTIOCB                                }, // TMR_DEF_QSEP_BLINKY_BLUE
    {&g_led_scr_red_brightness,   GPT_IO_PIN_GTIOCB                                }, // TMR_DEF_LED_UX_BRIGHTNESS_RED
    {&g_led_scr_green_brightness, GPT_IO_PIN_GTIOCB                                }, // TMR_DEF_LED_UX_BRIGHTNESS_RED
    {&g_led_scr_blue_brightness,  GPT_IO_PIN_GTIOCB                                }, // TMR_DEF_LED_UX_BRIGHTNESS_RED
};

#define YEARS_SINCE_1900    2023 - 1900

/* rtc_time_t is an alias for the C Standard time.h struct 'tm' */
rtc_time_t set_time =
{
    .tm_sec  = 00,
    .tm_min  = 00,
    .tm_hour = 12,

    // Not Used (day.date)
    .tm_mday = 0,
    .tm_wday = 0,
    .tm_mon  = 01,
    .tm_year = 0,
};

extern char_t g_current_time_raw_str[];

/**********************************************************************************************************************
 * Function Name: rtc_startup
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
static void rtc_startup ()
{
    fsp_err_t err = FSP_SUCCESS;

    UNUSED_VARIABLE(err);

    /* Open the RTC module */
    err = R_RTC_Open(&g_rtc_ctrl, &g_rtc_cfg);

    /* Handle any errors. This function should be defined by the user. */

// assert(FSP_SUCCESS == err);

    char_t index_minuite[8] = {};
    char_t index_hours[8]   = {};

    char_t * p_index_localtime = strstr(g_current_time_raw_str, "localtime\"");
    p_index_localtime = strstr(p_index_localtime, " ");

    memcpy(index_hours, strstr(p_index_localtime, " ") + 1, 2);

    p_index_localtime++;

    memcpy(index_minuite, strstr(p_index_localtime, ":") + 1, 2);

    set_time.tm_hour = atoi(index_hours);
    set_time.tm_min  = atoi(index_minuite);

    /* Set the RTC clock source. Can be skipped if "Set Source Clock in Open" property is enabled. */
    R_RTC_ClockSourceSet(&g_rtc_ctrl);

    /* Set the calendar time */
    R_RTC_CalendarTimeSet(&g_rtc_ctrl, &set_time);
}

/**********************************************************************************************************************
 * End of function rtc_startup
 *********************************************************************************************************************/

adc_info_t g_adc_info_rtn BSP_PLACE_IN_SECTION(".sdram");

/**********************************************************************************************************************
 * Function Name: adc_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t adc_initialize (void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* When selecting the temperature sensor output, the internal reference voltage,
     * do not use continuous scan mode or group scan mode. */

    fsp_err = R_ADC_Open(&g_adc_ctrl, &g_adc_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = R_ADC_ScanCfg(&g_adc_ctrl, &g_adc_channel_cfg);
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    /* Read TSN cal data (value written at manufacture, does not change at runtime) */
    fsp_err = R_ADC_InfoGet(&g_adc_ctrl, &g_adc_info_rtn);

    /* Start the ADC */
    R_ADC_ScanStart(&g_adc_ctrl);

    return fsp_err;
}

/**********************************************************************************************************************
 * End of function adc_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: icu_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t icu_initialize (void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    for (uint32_t i = 0; i < NUM_SWITCH; i++)
    {
        fsp_err = R_ICU_ExternalIrqOpen(s_irq_pins[i].p_irq->p_ctrl, s_irq_pins[i].p_irq->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }

        fsp_err = R_ICU_ExternalIrqEnable(s_irq_pins[i].p_irq->p_ctrl);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    return fsp_err;
}

/**********************************************************************************************************************
 * End of function icu_initialize
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: gpt_initialize
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t gpt_initialize(void);

fsp_err_t gpt_initialize (void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;
    uint32_t  limit   = (sizeof(g_pwm_pins)) / sizeof(st_pwm_pins_t);

    for (uint32_t i = 0; i < limit; i++)
    {
        fsp_err = R_GPT_Open(g_pwm_pins[i].p_timer->p_ctrl, g_pwm_pins[i].p_timer->p_cfg);
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Open(g_blinker.p_ctrl, g_blinker.p_cfg);
    {
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_PeriodSet(g_blinker.p_ctrl, g_pwm_rates[g_board_status.led_frequency]);
    {
        if (FSP_SUCCESS != fsp_err)
        {
            return fsp_err;
        }
    }

    fsp_err = R_GPT_Start(g_blinker.p_ctrl);

    if (FSP_SUCCESS != fsp_err)
    {
        /* Fatal error */
        SYSTEM_ERROR
        /* Close the GPT timer */
        R_GPT_Close(g_blinker.p_ctrl);

        return fsp_err;
    }

    return fsp_err;
}

/**********************************************************************************************************************
 * End of function gpt_initialize
 *********************************************************************************************************************/

extern void jlink_console_init(void);
extern void init_ospi(void);
extern void init_sdram(void);

#define REG_PIDH                     0x300a
#define REG_PIDL                     0x300b

#define CAM_PWDN_PIN                 BSP_IO_PORT_07_PIN_04
#define BSP_I2C_SLAVE_ADDR_CAMERA    (0x3C) // Slave address for OV3640 Camera Module

bool_t is_startup_mode_in_error(void);
bool_t is_camera_mode(void);
bool_t is_ethernet_mode(void);

/**********************************************************************************************************************
 * Function Name: is_camera_connected
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static bool_t is_camera_connected (void)
{
    fsp_err_t status            = FSP_ERR_NOT_OPEN;
    bool_t    initialised_state = false;
    byte      reg_val1          = 0;
    byte      reg_val2          = 0;

    R_GPT_Open(&g_cam_clk_ctrl, &g_cam_clk_cfg);
    R_GPT_Start(&g_cam_clk_ctrl);

    /* Hardware reset module */
    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(&g_ioport_ctrl, CAM_PWDN_PIN, (bsp_io_level_t) POWER_DOWN);
    R_BSP_SoftwareDelay(40, BSP_DELAY_UNITS_MILLISECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, CAM_PWDN_PIN, (bsp_io_level_t) POWER_UP);
    R_BSP_PinAccessDisable();

    i2c_master_status_t i2c_status;
    R_IIC_MASTER_StatusGet(&g_i2c_master1_ctrl, &i2c_status);

    /* only open if not opened */
    if (i2c_status.open != true)       // I2c not open
    {
        status = R_IIC_MASTER_Open(&g_i2c_master1_ctrl, &g_i2c_master1_cfg);
    }

    if (FSP_SUCCESS == status)
    {
        R_IIC_MASTER_SlaveAddressSet(&g_i2c_master1_ctrl, BSP_I2C_SLAVE_ADDR_CAMERA, I2C_MASTER_ADDR_MODE_7BIT);
        rdSensorReg16_8(REG_PIDH, &reg_val1); // PIDH  PID MSB
        rdSensorReg16_8(REG_PIDL, &reg_val2); // PIDH  PID LSB REV2c - 0x4C, REV2a = 0x41, REV1a=0x40 otherwise error

        if ((reg_val2 == 0x40) || (reg_val2 == 0x41) || (reg_val2 == 0x4C))
        {
            initialised_state = true;

            /* Tel Camera to enter into Sleep(software power down)mode */
            wrSensorReg8_8(0x361e, 0x00);
            wrSensorReg8_8(0x308d, 0x06);
            wrSensorReg8_8(0x30ad, 0x82);
            wrSensorReg8_8(0x308d, 0x0f);
        }

        R_GPT_Stop(&g_cam_clk_ctrl);
    }

    return initialised_state;
}

/**********************************************************************************************************************
 * End of function is_camera_connected
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: is_ethernet_connected
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static bool_t is_ethernet_connected (void)
{
    volatile bool_t initialised_state = false;

// if (false == is_camera_mode())
    {
        /* Load Ethernet io_port_cfg */
        R_IOPORT_Close(&g_ioport_ctrl);
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_qsep_cfg);

        /* User has pressed signal Ethernet to start */
        xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_ETHERNET);

        vTaskDelay(1000);

        uint32_t reg = 0;
        R_ETHER_PHY_Read(g_ether0.p_cfg->p_ether_phy_instance->p_ctrl, 0x00, &reg);

        if (reg != 0)
        {
            initialised_state = true;
        }
    }

    return initialised_state;
}

/**********************************************************************************************************************
 * End of function is_ethernet_connected
 *********************************************************************************************************************/

/*
 * 1: Check for Camera IIC
 *
 * Response Check
 * No response = Assume SW3 off as it's don't care GOTO 2:
 * With response: Tell camera to go to sleep!!
 *
 * 2: Set P701 to  INPUT - PullUP
 *
 * Read P701 if it goes low even once => SW5 ON - Only allow QSEP apps
 * Read P701 stay high for 5 ms =>  SW5-OFF - GOTO 3
 */

/**********************************************************************************************************************
 * Function Name: decide_which_demo
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void decide_which_demo (void)
{
    // Can we talk to CAMERA ?
    if (is_camera_connected() == true)
    {
        s_mode_selected = 1;           // Camera Mode
    }

    // Is Ethernet (sw1_5) ON ?
    if (is_ethernet_connected() == true)
    {
        s_mode_selected += 2;          // Ethernet Mode
    }
}

/**********************************************************************************************************************
 * End of function decide_which_demo
 *********************************************************************************************************************/

#define TEST_SDRAM_DATA_0    (0x05A5A5A5)
static volatile int32_t s_sd_data BSP_ALIGN_VARIABLE(64) BSP_PLACE_IN_SECTION(".sdram") = TEST_SDRAM_DATA_0;

/* Check SDRAM operation */

/**********************************************************************************************************************
 * Function Name: is_sdram_in_error
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t is_sdram_in_error (void)
{
    volatile bool_t ret = false;

    /* Write test byte TEST_SDRAM_DATA_0 */
    s_sd_data = TEST_SDRAM_DATA_0;
    vTaskDelay(100);

    /* Verify test byte TEST_SDRAM_DATA_0 */
    if (TEST_SDRAM_DATA_0 != s_sd_data)
    {
        ret = true;
    }

    /* Verify test byte 0 */
    s_sd_data = 0;
    vTaskDelay(100);

    /* Verify test byte 0 */
    if (0 != s_sd_data)
    {
        ret = true;
    }

    return ret;
}

/**********************************************************************************************************************
 * End of function is_sdram_in_error
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: is_startup_mode_in_error
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t is_startup_mode_in_error (void)
{
    volatile bool_t ret = false;

    if ((s_mode_selected >= 3) || (s_mode_selected == 0))
    {
        ret = true;
    }

    return ret;
}

/**********************************************************************************************************************
 * End of function is_startup_mode_in_error
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: is_camera_mode
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t is_camera_mode (void)
{
    bool_t ret = false;
    if (s_mode_selected == 1)
    {
        ret = true;
    }

    return ret;
}

/**********************************************************************************************************************
 * End of function is_camera_mode
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: is_ethernet_mode
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t is_ethernet_mode (void)
{
    bool_t ret = false;
    if (s_mode_selected >= 2)
    {
        ret = true;
    }

    return ret;
}

/**********************************************************************************************************************
 * End of function is_ethernet_mode
 *********************************************************************************************************************/
#define KEY_TIMEOUT    (60000000)

/**********************************************************************************************************************
 * Function Name: common_init
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t common_init (void)
{
    fsp_err_t fsp_err = FSP_SUCCESS;

    /* Set baseline LED status */
    g_board_status.led_intensity = 0;
    g_board_status.led_frequency = 0;

    /* Reset sw1 detection result */
    s_mode_selected = 0;

    do_detection      = false;
    do_classification = false;

    jlink_console_init();

    decide_which_demo();

    /* g_ s_mode_selected state */
    /* 0  No Mode selected, assume Ethernet Mode */
    /* 1  Camera selected */
    /* 2  Ethernet Mode Selected */
    /* 3+ Camera & Ethernet Mode Selected ERROR STATE */
    if (s_mode_selected == 1)
    {
        /* Load AI io_port_cfg */
        R_IOPORT_Close(&g_ioport_ctrl);
        R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_ai_cfg);
        SYSTEM_OK vTaskDelay(100);
    }
    else
    {
        SYSTEM_OK vTaskDelay(100);

        /* User has pressed signal Ethernet to start */

// xEventGroupSetBits(g_update_console_event, STATUS_ENABLE_ETHERNET);
// xEventGroupSetBits(g_update_console_event, STATUS_IOT_REQUEST_TIME);
    }

    /* Initialse SD_RAM uses bespoke PinCfg */
    init_sdram();

    jlink_console_init();

    if (is_sdram_in_error() == true)
    {
        uint32_t count = KEY_TIMEOUT;

        R_GPT_Stop(g_blinker.p_ctrl);

        print_to_console((void *) "\n\rNote: SDRAM state cannot be determined, please check SW1 on target board.");

        while (1)
        {
            if (KEY_TIMEOUT == count)
            {
                R_BSP_PinAccessEnable();
                TURN_RED_OFF;
                R_BSP_PinAccessDisable();
            }

            if ((count--) == 0)
            {
                count = KEY_TIMEOUT * 2;

                R_BSP_PinAccessEnable();
                SYSTEM_ERROR;
                R_BSP_PinAccessDisable();

                print_to_console((void *) "\rNote: SDRAM state cannot be determined, please check SW1 on target board.");
            }
        }
    }

    init_ospi();

    /* Initailse the simulated https responses for weather / currency & time */
    if (false == initialise_simulated_data_in_sdram())
    {
        print_to_console("Graphics data is corrupted !\r\n");
        TURN_GREEN_OFF;
        TURN_BLUE_OFF;
        TURN_RED_ON;

        while (1)
        {
            vTaskDelay(1000);
        }
    }

    /* initalise fonts in SDRAM  */
    initialise_bg_font_18_full();
    initialise_fg_font_22_full();

    rtc_startup();

    fsp_err = adc_initialize();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = icu_initialize();
    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = gpt_initialize();

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    fsp_err = initialise_data_flash();

    if (FSP_SUCCESS != fsp_err)
    {
        return fsp_err;
    }

    led_duty_cycle_update();

// R_GPT_Start(g_pwm_pins[0].p_timer->p_ctrl);

    xEventGroupSetBits(g_update_console_event, STATUS_CONSOLE_AVAILABLE);

    return fsp_err;
}

/**********************************************************************************************************************
 * End of function common_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: led_duty_cycle_update
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void led_duty_cycle_update (void)
{
    R_GPT_DutyCycleSet(g_pwm_pins[0].p_timer->p_ctrl, g_pwm_dcs[g_board_status.led_intensity], g_pwm_pins[0].pin);

    s_duty = g_pwm_dcs[g_board_status.led_intensity];
}

/**********************************************************************************************************************
 * End of function led_duty_cycle_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: search_key_second
 * Description  : .
 * Arguments    : s_source
 *              : pattern1
 *              : pattern2
 *              : index
 * Return Value : .
 *********************************************************************************************************************/
static char_t * search_key_second (char_t * s_source, char_t * pattern1, char_t * pattern2, uint32_t * index)
{
    char_t * p_target = NULL;
    char_t * p_start;
    char_t * p_end;
    if (strstr(s_source, pattern1) && strstr(s_source, pattern2)) // check failure occur
    {
        p_start  = strstr(s_source, pattern1);
        p_start += strlen(pattern1);
        p_end    = strstr(p_start, pattern2);
        if (p_end > p_start)
        {
            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            p_target = (char_t *) malloc((size_t) ((p_end - p_start) + 2));

            /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
            memcpy(p_target, p_start, (size_t) (p_end - p_start));
            p_target[p_end - p_start]       = '}';
            p_target[(p_end - p_start) + 1] = '\0';

            /* ignoring -Wpointer-sign index must be uint32_t to save the address */
            *index = (uint32_t) p_end;
        }
        else
        {
            ;
        }
    }
    else
    {
        ;
    }

    return p_target;
}

/**********************************************************************************************************************
 * End of function search_key_second
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: currency_get_rate
 * Description  : .
 * Arguments    : table
 *              : request
 * Return Value : .
 *********************************************************************************************************************/
float64_t currency_get_rate (char_t * table, char_t request[3], char_t * _pres)
{
    char_t * p_char_front;
    char_t * p_char_back;
    char_t * p_target_currency_string;
    uint32_t index_from_last_search;
    char_t   tmp[64];

    float64_t currency_number;
    p_char_back = "}";

    p_target_currency_string = search_key_second((char_t *) table, request, p_char_back, &index_from_last_search);

    p_char_front = "\"value\":";
    p_char_back  = "}";

    p_target_currency_string = search_key_second((char_t *) p_target_currency_string,
                                                 p_char_front,
                                                 p_char_back,
                                                 &index_from_last_search);

    memcpy(tmp, p_target_currency_string, strlen(p_target_currency_string));
    tmp[strlen(tmp)] = '\0';

    currency_number = atof(tmp);

    sprintf(tmp, "%s", p_target_currency_string);
    tmp[strlen(p_target_currency_string) - 1] = '\0';

    p_char_back = ".";

    char * fnd = strstr(tmp, p_char_back);

    if (fnd != 0)
    {
        sprintf(_pres, "%s0000", tmp);
    }
    else
    {
        sprintf(_pres, "%s.0000", tmp);
    }

    _pres[7] = '\0';

    free(p_target_currency_string);

    return currency_number;
}

/**********************************************************************************************************************
 * End of function currency_get_rate
 *********************************************************************************************************************/

char_t  dbg_time[512] = {0};
char_t  dbg_val[512]  = {0};
uint8_t dbg_len       = 0;

/**********************************************************************************************************************
 * Function Name: json_get_data_from_key
 * Description  : .
 * Arguments    : table
 *              : request
 * Return Value : .
 *********************************************************************************************************************/
bool_t json_get_data_from_key (char_t * table, char_t * timestring, char_t * _key, char_t * _pres)
{
    bool_t result = true;

    char_t * p_char_front;
    char_t * p_char_back;
    char_t * p_target_time_string;
    char_t * p_target_str_string;
    char_t * p_target_val_string;
    uint32_t index_from_last_search;
    char_t   tmp[64];
    uint16_t x = 0U;

    float64_t currency_number;
    p_char_back = "\"uv\"";

    /* Firstly, find the passed time (string) parameter in the response. */
    p_target_time_string = search_key_second((char_t *) table, timestring, p_char_back, &index_from_last_search);

    if (NULL == p_target_time_string)
    {
        return false;
    }

    for (x = 0; x < strlen(p_target_time_string); x++)
    {
        dbg_time[x] = p_target_time_string[x];
    }

    p_char_front = "\":";
    p_char_back  = ",";

    /* Now search for the key that has been passed as a string and get its value. */
    p_target_str_string =
        search_key_second((char_t *) p_target_time_string, _key, p_char_back, &index_from_last_search);

    if (NULL == p_target_str_string)
    {
        return false;
    }

    for (x = 0; x < strlen(p_target_str_string); x++)
    {
        dbg_val[x] = p_target_str_string[x];
    }

    p_char_back = "}";

    p_target_val_string = search_key_second((char_t *) p_target_str_string,
                                            p_char_front,
                                            p_char_back,
                                            &index_from_last_search);

    for (x = 0; x < strlen(p_target_val_string); x++)
    {
        dbg_val[x] = p_target_val_string[x];
    }

/*
 *  print_to_console("Value found: [");
 *  print_to_console(p_target_val_string);
 *  print_to_console("]\r\n");
 */
    sprintf(_pres, "%s", &p_target_str_string[2]); /* Ignore first 2 characters ": */
    dbg_len = strlen(p_target_str_string - 1);

    for (x = 0; x < strlen(p_target_str_string); x++)
    {
        dbg_val[x] = 0;
    }

    for (x = 0; x < strlen(p_target_str_string); x++)
    {
        dbg_val[x] = _pres[x];
    }

    /* Remove the closing brace that was added by search_key_second() */
    for (x = 0; x < strlen(p_target_str_string); x++)
    {
        if (_pres[x] == '}')
        {
            _pres[x] = '\0';
            break;
        }
    }

    free(p_target_time_string);
    free(p_target_str_string);

    return result;
}

/**********************************************************************************************************************
 * End of function json_get_data_from_key
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: gpt_blinker_callback
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void gpt_blinker_callback (timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        timer_status_t status = {0};

        R_GPT_StatusGet(g_pwm_pins[0].p_timer->p_ctrl, &status);

        if (TIMER_STATE_COUNTING == status.state)
        {
            R_GPT_Stop(g_pwm_pins[0].p_timer->p_ctrl);
        }
        else
        {
            R_GPT_Start(g_pwm_pins[0].p_timer->p_ctrl);
        }
    }
}

/**********************************************************************************************************************
 * End of function gpt_blinker_callback
 *********************************************************************************************************************/
