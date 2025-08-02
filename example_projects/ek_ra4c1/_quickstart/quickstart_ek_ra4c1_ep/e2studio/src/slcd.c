/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : slcd.c
 * Version      : 1.0
 * Description  : SLCD Configuration and controls.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "slcd.h"
#include "hal_data.h"
#include "jlink_console.h"
#include "r_typedefs.h"
#include "common_init.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/**
 * Clear segment command
 */
#define CLEAR_WRITE_DATA    (0x00)

/**
 * Number of the first segment.
 */
#define START_SEGMENT (0u)

/**
 * Number of the last segment.
 */
#define END_SEGMENT (43u)

/**
 * Number of segments. Note: not all must be used, it is a difference between last and first segment.
 */
#define SEGMENT_COUNT (END_SEGMENT - START_SEGMENT)

/**
 * Segment LCD frame buffer.
 */
static uint8_t s_slcd_frame_buffer[SEGMENT_COUNT] =
{ };

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const st_icon_segment_t g_day_of_week_segment[7] =
{
    { SUNDAY_SEGMENT,    SUNDAY_DATA    },
    { MONDAY_SEGMENT,    MONDAY_DATA    },
    { TUESDAY_SEGMENT,   TUESDAY_DATA   },
    { WEDNESDAY_SEGMENT, WEDNESDAY_DATA },
    { THURSDAY_SEGMENT,  THURSDAY_DATA  },
    { FRIDAY_SEGMENT,    FRIDAY_DATA    },
    { SATURDAY_SEGMENT,  SATURDAY_DATA  }
};

const st_icon_segment_t g_battery_level_segment[5] =
{
    { BATTERY_SHELL_SEGMENT, BATTERY_SHELL_DATA },                              /* BATTERY_EMPTY OR BATTERY_0 */
    { BATTERY_LEVEL1_SEGMENT, BATTERY_LEVEL1_DATA },                            /* BATTERY_1 */
    { BATTERY_LEVEL2_SEGMENT, (BATTERY_LEVEL1_DATA | BATTERY_LEVEL2_DATA) },    /* BATTERY_2 */
                                                                                /* BATTERY_3 */
    { BATTERY_LEVEL3_SEGMENT, (BATTERY_LEVEL1_DATA | BATTERY_LEVEL2_DATA | BATTERY_LEVEL3_DATA) },
                                                                                /* BATTERY_FULL OR BATTERY_4 */
    { BATTERY_LEVEL4_SEGMENT, (BATTERY_LEVEL1_DATA | BATTERY_LEVEL2_DATA | BATTERY_LEVEL3_DATA | BATTERY_LEVEL4_DATA) }
};

const st_icon_segment_t g_am_or_pm[2] =
{
    { AM_SEGMENT, AM_DATA },
    { PM_SEGMENT, PM_DATA }
};

const st_icon_segment_t g_degc_or_degf[2] =
{
    { DEG_C_SEGMENT, DEG_C_DATA },
    { DEG_F_SEGMENT, DEG_F_DATA }
};

const st_icon_segment_t g_medium_digit_dp_or_colon[2] =
{
    { MEDIUM_DIGIT_DEC_POINT_SEGMENT, MEDIUM_DIGIT_DEC_POINT_DATA },
    { MEDIUM_DIGIT_COLON_SEGMENT,     MEDIUM_DIGIT_COLON_DATA     }
};

const uint8_t g_big_digit_start_addresses[3][2] =
{
    { 15,16 },  // units digit is controlled by SLCD Data register 19 & 20 pin 45, 43, segment "9"
    { 17,25 },  // tens digit is controlled by SLCD Data register 21 & 29 pin 41, 39, segment "8"
    { 27,28 }   // hundreds digit is controlled by SLCD Data register 31 & 32 pin 37, 35, segment "7"
};

const uint8_t g_medium_digit_start_addresses[5][2] =
{
    { 6,12 },  // units digit is controlled by SLCD Data register 29 & 30 pin 48, 46, segment "10"
    { 11,24 },  // tens digit is controlled by SLCD Data register 31 & 32 pin 44, 42, segment "11"
    { 20,19 },  // hundreds digit is controlled by SLCD Data register 34 & 35 pin 38,36, segment "12"
    { 29,32 },  // thousands digit is controlled by SLCD Data register 36 & 37 pin 34,32, segment "13"
    { 18,14 }   // ten-thousands digit is controlled by SLCD Data register 38 & 39 pin 30,28, segment "14"
};

const uint8_t g_big_digit_data[10][2] =
{
    { 0x05, 0x0f }, // 0 digit is display by writing 0x05 and 0x0f to the appropriate big_digit_start_addresses
    { 0x00, 0x06 }, // 1 digit is display by writing 0x00 and 0x06 to the appropriate big_digit_start_addresses
    { 0x06, 0x0b }, // 2 digit is display by writing 0x06 and 0x0b to the appropriate big_digit_start_addresses
    { 0x02, 0x0f }, // 3 digit is display by writing 0x02 and 0x0f to the appropriate big_digit_start_addresses
    { 0x03, 0x06 }, // 4 digit is display by writing 0x03 and 0x06 to the appropriate big_digit_start_addresses
    { 0x03, 0x0d }, // 5 digit is display by writing 0x03 and 0x0d to the appropriate big_digit_start_addresses
    { 0x07, 0x0d }, // 6 digit is display by writing 0x07 and 0x0d to the appropriate big_digit_start_addresses
    { 0x00, 0x07 }, // 7 digit is display by writing 0x00 and 0x07 to the appropriate big_digit_start_addresses
    { 0x07, 0x0f }, // 8 digit is display by writing 0x07 and 0x0f to the appropriate big_digit_start_addresses
    { 0x03, 0x0f }  // 9 digit is display by writing 0x03 and 0x0f to the appropriate big_digit_start_addresses
};

const uint8_t g_medium_digit_data[10][2] =
{
    { 0x0f, 0x0a }, // 0 digit is display by writing 0x0f and 0x0a to the appropriate medium_digit_start_addresses
    { 0x06, 0x00 }, // 1 digit is display by writing 0x06 and 0x00 to the appropriate medium_digit_start_addresses
    { 0x0d, 0x06 }, // 2 digit is display by writing 0x0d and 0x06 to the appropriate medium_digit_start_addresses
    { 0x0f, 0x04 }, // 3 digit is display by writing 0x0f and 0x04 to the appropriate medium_digit_start_addresses
    { 0x06, 0x0c }, // 4 digit is display by writing 0x06 and 0x0c to the appropriate medium_digit_start_addresses
    { 0x0b, 0x0c }, // 5 digit is display by writing 0x0b and 0x0c to the appropriate medium_digit_start_addresses
    { 0x0b, 0x0e }, // 6 digit is display by writing 0x0b and 0x0e to the appropriate medium_digit_start_addresses
    { 0x0e, 0x00 }, // 7 digit is display by writing 0x0e and 0x00 to the appropriate medium_digit_start_addresses
    { 0x0f, 0x0e }, // 8 digit is display by writing 0x0f and 0x0e to the appropriate medium_digit_start_addresses
    { 0x0f, 0x0c }  // 9 digit is display by writing 0x0f and 0x0c to the appropriate medium_digit_start_addresses
};

const uint8_t g_big_digit_error_data[2][2] =
{
    { 0x07, 0x09 }, // E character is display by writing 0x07 and 0x09 to the appropriate medium_digit_start_addresses
    { 0x06, 0x00 }  // r character is display by writing 0x06 and 0x00 to the appropriate medium_digit_start_addresses
};

const uint8_t g_medium_digit_error_data[3][2] =
{
    { 0x09, 0x0e }, // E character is display by writing 0x09 and 0x0e to the appropriate medium_digit_start_addresses
    { 0x00, 0x06 }, // r character is display by writing 0x00 and 0x06 to the appropriate medium_digit_start_addresses
    { 0x03, 0x06 }  // o character is display by writing 0x03 and 0x06 to the appropriate medium_digit_start_addresses
};


const uint8_t g_small_digit_start_addresses[2][4]=
{
    /* The below digit is refer to the small digit segment sector.
     * Please check the schematic on segment lcd for more information
     * 1: AJML
     * 2: HIND
     * 3: XFGE
     * 4: XBKC
     * X = not used */
    { 9, 7, 21, 22},   // units digit is controlled by SLCD pin 33, 31, 24, 26, segment "6"
    { 40, 41, 37, 35 },  // tens digit is controlled by SLCD pin 29, 27, 20, 22, segment "5"
};

const uint8_t g_small_round_data[14][4]=
{
    {0x01, 0x00 , 0x00, 0x00}, /*Segment A*/
    {0x00, 0x00 , 0x00, 0x02}, /*Segment B*/
    {0x00, 0x00 , 0x00, 0x08}, /*Segment C*/
    {0x00, 0x08 , 0x00, 0x00}, /*Segment D*/
    {0x00, 0x00 , 0x08, 0x00}, /*Segment E*/
    {0x00, 0x00 , 0x02, 0x00}, /*Segment F*/
    {0x00, 0x00 , 0x04, 0x00}, /*Segment G*/
    {0x00, 0x01 , 0x00, 0x00}, /*Segment H*/
    {0x00, 0x02 , 0x00, 0x00}, /*Segment I*/
    {0x02, 0x00 , 0x00, 0x00}, /*Segment J*/
    {0x00, 0x00 , 0x00, 0x04}, /*Segment K*/
    {0x08, 0x00 , 0x00, 0x00}, /*Segment L*/
    {0x04, 0x00 , 0x00, 0x00}, /*Segment M*/
    {0x00, 0x04 , 0x00, 0x00}, /*Segment N*/
};

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static fsp_err_t print_err_big_digit (void);
static fsp_err_t print_error_medium_digit (void);
static fsp_err_t display_medium_digit (uint32_t value);

/**********************************************************************************************************************
 * Function Name: slcdc_init
 * Description  : Open, configure and enable the Static LCD module.
 * Return Value : True = Success.
 *********************************************************************************************************************/
bool_t slcdc_init(void)
{
    bool_t    opened = false;

    fsp_err_t err    = FSP_SUCCESS;

    err = g_slcdc.p_api->open (g_slcdc.p_ctrl, g_slcdc.p_cfg);

    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = g_slcdc.p_api->start (g_slcdc.p_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = g_slcdc.p_api->setDisplayArea (g_slcdc.p_ctrl, SLCDC_DISP_A);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (FSP_SUCCESS == err)
    {
        opened = true;
    }

    g_slcdc.p_api->write (g_slcdc.p_ctrl, START_SEGMENT, s_slcd_frame_buffer, SEGMENT_COUNT);

    return (opened);
}
/**********************************************************************************************************************
 End of function slcdc_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_segments_medium_digits
 * Description  : Turn on the segments for a value on the medium size digits, including colon or dot as specified.
 * Arguments    : value
 *              : point_or_colon
 * Return Value : FSP_SUCCESS or relevant error code
 *********************************************************************************************************************/
fsp_err_t set_segments_medium_digits(uint32_t value, e_medium_dp_or_colon_t point_or_colon)
{
    fsp_err_t err = FSP_SUCCESS;

    /* set / clear the decimal point */
    switch (point_or_colon)
    {
        case SET_DP:
        {
            err = set_segment_icon_named (BIG_DIGIT_DEC_POINT_ICON, ICON_ON);
            break;
        }
        case SET_COLON:
        {
            err = set_segment_icon_named (MEDIUM_DIGIT_COLON_ICON, ICON_ON);
            break;
        }
        case DP_COLON_OFF:
        default:
        {
            err = set_segment_icon_named (MEDIUM_DIGIT_COLON_ICON, ICON_OFF);
            break;
        }
    }
    if (value > 99999)
    {
        /* Write an error message "E r r o r" */
        err = print_error_medium_digit();
    }
    else
    {
        err = display_medium_digit(value);
    }
    return err;
}
/**********************************************************************************************************************
 End of function set_segments_medium_digits
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_segments_big_extras
 * Description  : Turn on the segments for the big digits, including minus or dot as specified.
 * Arguments    : use_minus
 *              : dp_setting
 * Return Value : FSP_SUCCESS or relevant error code
 *********************************************************************************************************************/
fsp_err_t set_segments_big_extras(bool use_minus, e_big_digit_dp_t dp_setting)
{
    fsp_err_t err = FSP_SUCCESS;

    /* set / clear the minus sign and make the negative number positive for ease of processing */
    if (true == use_minus)
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, BIG_DIGIT_MINUS_SEGMENT, BIG_DIGIT_MINUS_DATA,
                                        (uint8_t) BIG_DIGIT_MINUS_DATA); /* Cast to uint8_t */
    }
    else
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, BIG_DIGIT_MINUS_SEGMENT, CLEAR_WRITE_DATA,
                                        (uint8_t) BIG_DIGIT_MINUS_DATA); /* Cast to uint8_t */
    }

    if (WITH_DP == dp_setting)
    {
        err = R_SLCDC_Modify (&g_slcdc_ctrl, BIG_DIGIT_DEC_POINT_SEGMENT, BIG_DIGIT_DEC_POINT_DATA,
                                (uint8_t) BIG_DIGIT_DEC_POINT_DATA); /* Cast to uint8_t */
    }
    else
    {
        err = R_SLCDC_Modify (&g_slcdc_ctrl, BIG_DIGIT_DEC_POINT_SEGMENT, CLEAR_WRITE_DATA,
                                (uint8_t) BIG_DIGIT_DEC_POINT_DATA); /* Cast to uint8_t */
    }

    return err;
}
/**********************************************************************************************************************
 End of function set_segments_big_extras
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_segments_big_digits
 * Description  : Turn on the segments for a value on the big size digits, including dot as specified.
 *                Write Err on the LCD if invalid.
 * Arguments    : value     : value to set
 *              : dp_setting: point setting from enumeration
 * Return Value : FSP_SUCCESS or relevant error code
 *********************************************************************************************************************/
fsp_err_t set_segments_big_digits(int16_t value, e_big_digit_dp_t dp_setting)
{
    fsp_err_t err = FSP_SUCCESS;

    int8_t temp_hundreds;
    int8_t temp_tens;
    int8_t temp_units;

    /* set / clear the minus sign and make the negative number positive for ease of processing */
    if (value < 0)
    {
        value = -value;
        err   = g_slcdc.p_api->modify (g_slcdc.p_ctrl, BIG_DIGIT_MINUS_SEGMENT, BIG_DIGIT_MINUS_DATA,
                                        (uint8_t) BIG_DIGIT_MINUS_DATA); /* Cast to uint8_t */
    }
    else
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, BIG_DIGIT_MINUS_SEGMENT, CLEAR_WRITE_DATA,
                                        (uint8_t) BIG_DIGIT_MINUS_DATA); /* Cast to uint8_t */
    }
    temp_hundreds = (int8_t) ((value / 100)); /* Cast int from calculation to int8_t */
    temp_tens     = (int8_t) ((value % 100) / 10); /* Cast int from calculation to int8_t */
    temp_units    = (int8_t) ((value % 10) / 1); /* Cast int from calculation to int8_t */
    if (value > 999)
    {
        /* Write an error message "E r r" */
        err = print_err_big_digit();
    }
    else
    {
        /* set / clear the decimal point */
        if (WITH_DP == dp_setting)
        {
            err = R_SLCDC_Modify (&g_slcdc_ctrl, BIG_DIGIT_DEC_POINT_SEGMENT, BIG_DIGIT_DEC_POINT_DATA,
                                    (uint8_t) BIG_DIGIT_DEC_POINT_DATA); /* Cast to uint8_t */
        }
        else
        {
            err = R_SLCDC_Modify (&g_slcdc_ctrl, BIG_DIGIT_DEC_POINT_SEGMENT, CLEAR_WRITE_DATA,
                                    (uint8_t) BIG_DIGIT_DEC_POINT_DATA); /* Cast to uint8_t */
        }
        if (value > 99)
        {
            /* set the HUNDREDS digit */
            err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_HUNDREDS][0],
                                    g_big_digit_data[temp_hundreds][0], BIG_DIGIT_MASK_4_2_1);
            err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_HUNDREDS][1],
                                    g_big_digit_data[temp_hundreds][1], BIG_DIGIT_MASK_8_4_2_1);
        }
        else
        {
            /* Clear the HUNDREDS digit of any old and redundant segments */
            err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_HUNDREDS][0], CLEAR_WRITE_DATA,
                                    BIG_DIGIT_MASK_4_2_1);
            err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_HUNDREDS][1], CLEAR_WRITE_DATA,
                                    BIG_DIGIT_MASK_8_4_2_1);
        }
        if (value > 9)
        {
            /* set the TENS digit */
            err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_TENS][0],
                                    g_big_digit_data[temp_tens][0], BIG_DIGIT_MASK_4_2_1);
            err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_TENS][1],
                                    g_big_digit_data[temp_tens][1], BIG_DIGIT_MASK_8_4_2_1);
        }
        else
        {
            /* Clear the TENS digit of any old and redundant segments */
            err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_TENS][0], CLEAR_WRITE_DATA,
                                    BIG_DIGIT_MASK_4_2_1);
            err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_TENS][1], CLEAR_WRITE_DATA,
                                    BIG_DIGIT_MASK_8_4_2_1);
        }

        /* set the UNITS digit */
        err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_UNITS][0], g_big_digit_data[temp_units][0],
                                BIG_DIGIT_MASK_4_2_1);
        err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_UNITS][1], g_big_digit_data[temp_units][1],
                                BIG_DIGIT_MASK_8_4_2_1);
        if (value < 10)
        {
            if (WITH_DP == dp_setting)      // write a 0 so we display 0.X
            {
                err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_TENS][0], g_big_digit_data[0][0],
                                        BIG_DIGIT_MASK_4_2_1);
                err = R_SLCDC_Modify (&g_slcdc_ctrl, g_big_digit_start_addresses[BN_TENS][1], g_big_digit_data[0][1],
                                        BIG_DIGIT_MASK_8_4_2_1);
            }
        }
    }
    return err;
}
/**********************************************************************************************************************
 End of function set_segments_big_digits
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: set_segment_icon_named
 * Description  : Turn on or off a specific enumerated segment.
 * Arguments    : segment_name  : Segment to set / clear
 *              : setting       : set or clear
 * Return Value : FSP_SUCCESS or relevant error code
 *********************************************************************************************************************/
fsp_err_t set_segment_icon_named(uint16_t segment_name, e_on_or_off_t setting)
{
    fsp_err_t err = FSP_SUCCESS;

    uint16_t segment_address = (segment_name & 0xff00) >> 8;
    uint16_t segment_data    = (segment_name & 0x00ff);

    if (ICON_ON == setting)
    {
        /* Write the specified segment */
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, (uint8_t) segment_address, (uint8_t) segment_data,
                                        (uint8_t) segment_data);
    }
    else
    {
        /* Clear the specified segment */
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, (uint8_t) segment_address, (uint8_t) CLEAR_WRITE_DATA,
                                        (uint8_t) segment_data);
    }
    return err;
}
/**********************************************************************************************************************
 End of function set_segment_icon_named
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_err_big_digit
 * Description  : Write Err on the big digits to indicate a problem
 * Return Value : FSP_SUCCESS or relevant error code
 *********************************************************************************************************************/
static fsp_err_t print_err_big_digit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_big_digit_start_addresses[BN_HUNDREDS][0],
                                &g_big_digit_error_data[0][0], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_big_digit_start_addresses[BN_HUNDREDS][1],
                                &g_big_digit_error_data[0][1], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_big_digit_start_addresses[BN_TENS][0],
                                &g_big_digit_error_data[1][0], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_big_digit_start_addresses[BN_TENS][1],
                                &g_big_digit_error_data[1][1], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_big_digit_start_addresses[BN_UNITS][0],
                                &g_big_digit_error_data[1][0], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_big_digit_start_addresses[BN_UNITS][1],
                                &g_big_digit_error_data[1][1], 1);
    return (err);
}
/**********************************************************************************************************************
 End of function print_err_big_digit
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_error_medium_digit
 * Description  : Write Err on the medium digits to indicate a problem
 * Return Value : FSP_SUCCESS or relevant error code
 *********************************************************************************************************************/
static fsp_err_t print_error_medium_digit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Write an error message "E r r o r" */
    err = set_segment_icon_named (MEDIUM_DIGIT_DEC_POINT_ICON, ICON_OFF);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TEN_THOUSANDS][0],
                                &g_medium_digit_error_data[0][0], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TEN_THOUSANDS][1],
                                &g_medium_digit_error_data[0][1], 1);

    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_THOUSANDS][0],
                                &g_medium_digit_error_data[1][0], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_THOUSANDS][1],
                                &g_medium_digit_error_data[1][1], 1);

    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_HUNDREDS][0],
                                &g_medium_digit_error_data[1][0], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_HUNDREDS][1],
                                &g_medium_digit_error_data[1][1], 1);

    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TENS][0],
                                &g_medium_digit_error_data[2][0], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TENS][1],
                                &g_medium_digit_error_data[2][1], 1);

    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_UNITS][0],
                                &g_medium_digit_error_data[1][0], 1);
    err = g_slcdc.p_api->write (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_UNITS][1],
                                &g_medium_digit_error_data[1][1], 1);
    return (err);
}
/**********************************************************************************************************************
 End of function print_error_medium_digit
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: display_medium_digit
 * Description  : Turn on the segments for a value on the medium size digits.
 * Argument     : value : value to set
 * Return Value : FSP_SUCCESS or relevant error code
 *********************************************************************************************************************/
static fsp_err_t display_medium_digit(uint32_t value)
{
    fsp_err_t err = FSP_SUCCESS;

    uint8_t temp_ten_thousands = (uint8_t) ((value / 10000)); /* Cast to uint8_t */
    uint8_t temp_thousands     = (uint8_t) ((value % 10000) / 1000); /* Cast to uint8_t */
    uint8_t temp_hundreds      = (uint8_t) ((value % 1000) / 100); /* Cast to uint8_t */
    uint8_t temp_tens          = (uint8_t) ((value % 100) / 10); /* Cast to uint8_t */
    uint8_t temp_units         = (uint8_t) ((value % 10) / 1); /* Cast to uint8_t */

    if (value > 9999)
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TEN_THOUSANDS][0],
                                        g_medium_digit_data[temp_ten_thousands][0], MED_DIGIT_MASK_8_4_2_1);
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TEN_THOUSANDS][1],
                                        g_medium_digit_data[temp_ten_thousands][1], MED_DIGIT_MASK_8_4_2);
    }
    else
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TEN_THOUSANDS][0],
                                        CLEAR_WRITE_DATA, MED_DIGIT_MASK_8_4_2_1);
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TEN_THOUSANDS][1],
                                        CLEAR_WRITE_DATA, MED_DIGIT_MASK_8_4_2);
    }
    if (value > 999)
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_THOUSANDS][0],
                                        g_medium_digit_data[temp_thousands][0], MED_DIGIT_MASK_8_4_2_1);
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_THOUSANDS][1],
                                        g_medium_digit_data[temp_thousands][1], MED_DIGIT_MASK_8_4_2);
    }
    else
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_THOUSANDS][0],
                                        CLEAR_WRITE_DATA, MED_DIGIT_MASK_8_4_2_1);
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_THOUSANDS][1],
                                        CLEAR_WRITE_DATA, MED_DIGIT_MASK_8_4_2);
    }
    if (value > 99)
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_HUNDREDS][0],
                                        g_medium_digit_data[temp_hundreds][0], MED_DIGIT_MASK_8_4_2_1);
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_HUNDREDS][1],
                                        g_medium_digit_data[temp_hundreds][1], MED_DIGIT_MASK_8_4_2);
    }
    else
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_HUNDREDS][0],
                                        CLEAR_WRITE_DATA, MED_DIGIT_MASK_8_4_2_1);
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_HUNDREDS][1],
                                        CLEAR_WRITE_DATA, MED_DIGIT_MASK_8_4_2);
    }
    if (value > 9)
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TENS][0],
                                        g_medium_digit_data[temp_tens][0], MED_DIGIT_MASK_8_4_2_1);
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TENS][1],
                                        g_medium_digit_data[temp_tens][1], MED_DIGIT_MASK_8_4_2);
    }
    else
    {
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TENS][0], CLEAR_WRITE_DATA,
                                        MED_DIGIT_MASK_8_4_2_1);
        err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_TENS][1], CLEAR_WRITE_DATA,
                                        MED_DIGIT_MASK_8_4_2);
    }

    /* Write the Units */
    err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_UNITS][0],
                                    g_medium_digit_data[temp_units][0], MED_DIGIT_MASK_8_4_2_1);
    err = g_slcdc.p_api->modify (g_slcdc.p_ctrl, g_medium_digit_start_addresses[MN_UNITS][1],
                                    g_medium_digit_data[temp_units][1], MED_DIGIT_MASK_8_4_2);
    return (err);
}
/**********************************************************************************************************************
 End of function display_medium_digit
 *********************************************************************************************************************/
