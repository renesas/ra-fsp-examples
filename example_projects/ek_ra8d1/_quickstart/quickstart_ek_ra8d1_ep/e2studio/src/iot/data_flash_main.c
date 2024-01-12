/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : data_flash_main.c
 * Version      : .
 * OS           : .
 * H/W Platform : .
 * Description  : This file includes the main tutorial code.
 * Operation    : See doc\readme.txt
 * Limitations  : .
 * xxx          : Make your own other sections as needed. (e.g. describe endian)
 *********************************************************************************************************************/

/*
 * data_flash_main.c
 *
 *  Created on: 22 Oct 2023
 *      Author: b3800117
 */

#include "board_cfg.h"
#include "common_init.h"
#include "jlink_console.h"

#define FLASH_DF_BLOCK_0         0x27000000U /*   64 B:  0x40100000 - 0x4010003F */
#define FLASH_DATA_BLOCK_SIZE    (1024)
#define KEY_SIZE_IN_BYTES        64
#define BLOCK_ERASE_SIZE         32

#define KEY_DATA_IDENTIFIER      "QSEP iot key group v 1.00\0"
#define KEY_DATA_BLANK           "                                               "

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)
#define BACKSPACE                (0x08)
#define CARRIAGE_RETURN          (0x0D)
#define ESCAPE                   (0x1B)

typedef struct hp_flash_data_hdr
{
    char_t data[KEY_SIZE_IN_BYTES];
    bool_t key_valid;
    size_t length_of_record_bytes;
} st_hp_flash_data_hdr_t;

typedef struct hp_flash_data_block
{
    char_t                 id_block[64];
    st_hp_flash_data_hdr_t table[4];
} st_hp_flash_data_block_t;

static uint8_t * const          s_hp_stored_flash_data = (uint8_t *) (void *) 0x27000000U;
static st_hp_flash_data_block_t s_hp_new_flash_data    = {};
static bool_t initialised = false;
static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};

bool_t initialise_data_flash(void);
bool_t is_key_data_available(e_hp_data_flash_iot_keys_t id);
bool_t get_key_data(e_hp_data_flash_iot_keys_t id, char_t * active_key);
bool_t update_key_data(e_hp_data_flash_iot_keys_t id);
bool_t update_stored_keys(void);
bool_t erase_stored_keys(void);

static uint8_t fifo_chars[KEY_SIZE_IN_BYTES + 8];
static int8_t  g_new_api_key_str[KEY_SIZE_IN_BYTES + 8];
static st_hp_flash_data_block_t * stored_iot = (st_hp_flash_data_block_t *) s_hp_stored_flash_data;

static uint8_t idx           = 0U;
static uint8_t num_new_chars = 0U;

extern void start_key_check_fifo(void);

/**********************************************************************************************************************
 * Function Name: input_new_key
 * Description  : .
 * Arguments    : prompt
 *              : use_hex_mode
 * Return Value : .
 *********************************************************************************************************************/
bool_t input_new_key (char_t * prompt, bool_t use_hex_mode)
{
    bool_t res = true;
    int8_t c   = -1;
    idx = 0U;
    uint16_t x         = 0U;
    bool     exit_loop = FALSE;

    memset(g_new_api_key_str, '\0', sizeof(g_new_api_key_str));

    print_to_console(prompt);

    idx = 0U;
    start_key_check_fifo();
    num_new_chars = get_new_chars(fifo_chars);

    start_key_check_fifo();

    while (exit_loop == false)
    {
        num_new_chars = get_new_chars(fifo_chars);

        if (num_new_chars > 0)
        {
            for (x = 0; x < num_new_chars; x++)
            {
                c = (char_t) fifo_chars[x];

                if (c == CARRIAGE_RETURN)
                {
                    exit_loop = TRUE;
                }
                // else if ( (c == CONNECTION_ABORT_CRTL) || (c == MENU_EXIT_CRTL) )
                else if ((c == MENU_EXIT_CRTL))
                {
                    res       = FALSE;
                    exit_loop = TRUE;
                }
                else if (c == BACKSPACE)
                {
                    if (idx > 0)
                    {
                        g_new_api_key_str[--idx] = 0;
                        sprintf(s_print_buffer, "%c %c", c, c);
                        print_to_console(s_print_buffer);
                    }
                }
                else if (idx < KEY_SIZE_IN_BYTES)
                {
                    g_new_api_key_str[idx++] = (char_t) c;
                    sprintf(s_print_buffer, "%c", c);
                    print_to_console(s_print_buffer);
                }
                else
                {
                    /* Length limit reached, do not transfer any more characters to buffer */

// print_to_console("");
                }
            }

            num_new_chars = 0U;
        }
        else
        {
            vTaskDelay(1);
        }
    }

// sprintf(s_print_buffer, "\r\nState of exit_loop [%d], res [%d]\r\n",exit_loop, res);
// print_to_console(s_print_buffer);

    if (res == TRUE)
    {
        sprintf(s_print_buffer, "\r\nStoring key %s \r\n", g_new_api_key_str);
        print_to_console(s_print_buffer);
    }

    return res;
}

/**********************************************************************************************************************
 * End of function input_new_key
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: initialise_data_flash
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t initialise_data_flash (void)
{
    fsp_err_t err = FSP_SUCCESS;
    bool_t    res = false;

    err = R_FLASH_HP_Open(&g_flash1_ctrl, &g_flash1_cfg);

    if (err == FSP_SUCCESS)
    {
        flash_status_t status;
        err = R_FLASH_HP_StatusGet(&g_flash1_ctrl, &status);
        if (FLASH_STATUS_IDLE == status)
        {
            res = true;
        }

        R_FLASH_HP_Close(&g_flash1_ctrl);
    }

    memcpy(&s_hp_new_flash_data.id_block, KEY_DATA_IDENTIFIER, strlen(KEY_DATA_IDENTIFIER));

    for (uint32_t i = 0; i < 4; i++)
    {
        sprintf(s_hp_new_flash_data.table[i].data, (char_t *) KEY_DATA_BLANK);
        s_hp_new_flash_data.table[i].length_of_record_bytes = sizeof(KEY_DATA_BLANK);
        s_hp_new_flash_data.table[i].key_valid              = false;
    }

    R_FLASH_HP_Close(&g_flash1_ctrl);

    memcpy(&s_hp_new_flash_data, FLASH_DF_BLOCK_0, sizeof(s_hp_new_flash_data));

    return err;
}

/**********************************************************************************************************************
 * End of function initialise_data_flash
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_key_data
 * Description  : .
 * Arguments    : id
 *              : active_key
 * Return Value : .
 *********************************************************************************************************************/
bool_t get_key_data (e_hp_data_flash_iot_keys_t id, char_t * active_key)
{
    bool_t res = true;
    size_t i   = 0;

    switch (id)
    {
        case IOT_KEY_CURRENCY:
        {
            i = 0;
            sprintf(active_key, s_hp_new_flash_data.table[i].data, s_hp_new_flash_data.table[i].length_of_record_bytes);
            break;
        }

        case IOT_KEY_WEATHER:
        {
            i = 1;
            sprintf(active_key, s_hp_new_flash_data.table[i].data, s_hp_new_flash_data.table[i].length_of_record_bytes);
            break;
        }

        default:
            res = false;
    }

    return res;
}

/**********************************************************************************************************************
 * End of function get_key_data
 *********************************************************************************************************************/
#define KEY_TIMEOUT    60000000

bool_t disable_temprature = true;

bool_t disable_kis_update(void);
void   enable_temprature_update(void);
void   disable_temprature_update(void);

/**********************************************************************************************************************
 * Function Name: enable_kis_update
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void enable_temprature_update (void)
{
    disable_temprature = false;
}

/**********************************************************************************************************************
 * End of function enable_kis_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: disable_temprature_update
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void disable_temprature_update (void)
{
    disable_temprature = true;
}

/**********************************************************************************************************************
 * End of function disable_temprature_update
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: disable_kis_update
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t disable_kis_update (void)
{
    bool_t disable = false;

    if (disable_temprature == true)
    {
        disable = true;
    }

    return disable;
}

/**********************************************************************************************************************
 * End of function disable_kis_update
 *********************************************************************************************************************/

void   eth_abort(void);
bool_t did_data_abort_from_eth();

static bool_t b_eth_abort = false;

void   eth_escape(void);
bool_t did_escape_from_eth();

static bool_t b_eth_escape = false;

/**********************************************************************************************************************
 * Function Name: eth_escape
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void eth_escape (void)
{
    b_eth_escape = true;
}

/**********************************************************************************************************************
 * End of function eth_escape
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: did_escape_from_eth
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
bool_t did_escape_from_eth ()
{
    bool_t res = false;
    if (b_eth_escape == true)
    {
        res          = true;
        b_eth_escape = false;
    }

    return res;
}

/**********************************************************************************************************************
 * End of function did_escape_from_eth
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: eth_abort
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void eth_abort (void)
{
    b_eth_abort = true;
}

/**********************************************************************************************************************
 * End of function eth_abort
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: did_data_abort_from_eth
 * Description  : .
 * Argument     :
 * Return Value : .
 *********************************************************************************************************************/
bool_t did_data_abort_from_eth ()
{
    bool_t res = false;
    if (b_eth_abort == true)
    {
        res         = true;
        b_eth_abort = false;
    }

    return res;
}

/**********************************************************************************************************************
 * End of function did_data_abort_from_eth
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: update_key_data
 * Description  : .
 * Argument     : id
 * Return Value : .
 *********************************************************************************************************************/
bool_t update_key_data (e_hp_data_flash_iot_keys_t id)
{
    bool_t   res   = true;
    size_t   i     = 0;
    uint32_t count = 0;
    uint8_t  c     = (uint8_t) -1;

    UNUSED_VARIABLE(c);
    start_key_check();

    while (1)
    {
        if (count == 600000)
        {
            R_BSP_PinAccessEnable();
            TURN_RED_ON;
            R_BSP_PinAccessDisable();
        }

        if (count == 6000)
        {
            R_BSP_PinAccessEnable();
            TURN_RED_OFF;
            R_BSP_PinAccessDisable();
        }

        if (true == key_pressed())
        {
            c = get_detected_key();

            if (MENU_EXIT_CRTL == c)   // MENU_EXIT_CRTL Key
            {
                res = false;
            }

            break;
        }

        if (true == did_escape_from_eth())
        {
            /* The higher level timeout occurred in the network thread indicating the base
             * connection can not be established, give up here */
            res = false;
            break;
        }

        if ((count--) == 0)
        {
            sprintf(s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
            print_to_console(s_print_buffer);

            count = KEY_TIMEOUT;
            print_to_console("Data refresh is unsuccessful because API key is not found, or the existing key\r\n");
            print_to_console("is invalid, or it got rejected by the server or Internet connectivity is not\r\n");
            print_to_console("present. Make sure to check the Internet connection.\r\n");
            print_to_console("Enter a valid API below and press enter to save it.\r\n");
            print_to_console("Press any key to continue or space bar to return to MENU.\r\n\r\n");
        }
    }

    if (true == res)
    {
        switch (id)
        {
            case IOT_KEY_CURRENCY:
            {
                i   = 0;
                res = input_new_key(
                    "Enter a valid API below and press enter to save it,\r\n or press space bar to return to MENU.\r\n",
                    false);

/*
 *              res = input_new_key("Please go to www.currencyapi.com, register and obtain a key. \r\n"
 *                            "Then enter the key here to store for future use. \r\n"
 *                            "\r\nCase sensitive characters or digits,\r\n(Return/Enter to submit or ESC key to abort) :\r\n", false);
 */
                sprintf(s_hp_new_flash_data.table[i].data, "%s", (char_t *) g_new_api_key_str);

                s_hp_new_flash_data.table[i].length_of_record_bytes = sizeof(g_new_api_key_str);
                s_hp_new_flash_data.table[i].key_valid              = true;

                if (true == res)
                {
                    if (true == update_stored_keys())
                    {
                        print_to_console("API key stored in Data Flash.\r\n");
                    }
                }

                if (true == res)
                {
                    update_stored_keys();
                }

                break;
            }

            case IOT_KEY_WEATHER:
            {
                i = 1;

                res = input_new_key(
                    "Enter a valid API below and press enter to save it,\r\n or press space bar to return to MENU.\r\n",
                    false);

/*
 *              res = input_new_key("\r\nPlease go to www.weatherapi.com, register and obtain a key. \r\n"
 *                            "Then enter the key here to store for future use. \r\n"
 *                            "\r\nHexadecimal digits only, all lower case.\r\n(Return/Enter to submit or ESC key to abort) :\r\n", true);
 */
                sprintf(s_hp_new_flash_data.table[i].data, (char_t *) g_new_api_key_str);
                s_hp_new_flash_data.table[i].length_of_record_bytes = sizeof(g_new_api_key_str);
                s_hp_new_flash_data.table[i].key_valid              = true;

                if (true == res)
                {
                    if (true == update_stored_keys())
                    {
                        print_to_console("API key stored in Data Flash.\r\n");
                    }
                }

                if (true == res)
                {
                    update_stored_keys();
                }

                break;
            }

            default:
        }
    }

    print_to_console("\r\n");

    return res;
}

/**********************************************************************************************************************
 * End of function update_key_data
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: update_stored_keys
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t update_stored_keys (void)
{
    bool_t         res = false;
    flash_status_t status;
    fsp_err_t      err = FSP_SUCCESS;

    err = R_FLASH_HP_Open(&g_flash1_ctrl, &g_flash1_cfg);

    if (err == FSP_SUCCESS)
    {
        status = FLASH_STATUS_BUSY;
        err    = R_FLASH_HP_Erase(&g_flash1_ctrl, FLASH_DF_BLOCK_0, BLOCK_ERASE_SIZE);

        while (status == FLASH_STATUS_BUSY)
        {
            err = R_FLASH_HP_StatusGet(&g_flash1_ctrl, &status);
        }

        err = R_FLASH_HP_Write(&g_flash1_ctrl,
                               (const uint32_t) &s_hp_new_flash_data,
                               FLASH_DF_BLOCK_0,
                               FLASH_DATA_BLOCK_SIZE);

        while (status == FLASH_STATUS_BUSY)
        {
            err = R_FLASH_HP_StatusGet(&g_flash1_ctrl, &status);
        }

        R_FLASH_HP_Close(&g_flash1_ctrl);
        res = true;
    }

    if (res == true)
    {
// print_to_console("Done OK\r\n");
    }
    else
    {
        print_to_console("Error Storing Keys\r\n");
    }

    return res;
}

/**********************************************************************************************************************
 * End of function update_stored_keys
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: is_key_data_available
 * Description  : .
 * Argument     : id
 * Return Value : .
 *********************************************************************************************************************/
bool_t is_key_data_available (e_hp_data_flash_iot_keys_t id)
{
    bool_t res = false;

    fsp_err_t err = FSP_SUCCESS;

    err = R_FLASH_HP_Open(&g_flash1_ctrl, &g_flash1_cfg);

    if (err == FSP_SUCCESS)
    {
        flash_status_t status;
        err = R_FLASH_HP_StatusGet(&g_flash1_ctrl, &status);

        if (FLASH_STATUS_IDLE == status)
        {
            initialised = true;

            if (strstr(stored_iot->id_block, s_hp_new_flash_data.id_block) == NULL)
            {
// print_to_console("Invalid iot keys stored:\r\n");

                status = FLASH_STATUS_BUSY;
                err    = R_FLASH_HP_Erase(&g_flash1_ctrl, FLASH_DF_BLOCK_0, BLOCK_ERASE_SIZE);

                while (status == FLASH_STATUS_BUSY)
                {
                    err = R_FLASH_HP_StatusGet(&g_flash1_ctrl, &status);
                }

// use Simulated keys
#ifdef USE_SIMULATED_KEYS
                s_hp_new_flash_data.table[0].key_valid = true;
                sprintf(s_hp_new_flash_data.table[0].data, (char_t *) ACTIVE_ROB_CURRENCY_KEY);
                s_hp_new_flash_data.table[0].length_of_record_bytes = strlen(s_hp_new_flash_data.table[0].data);

                s_hp_new_flash_data.table[1].key_valid = true;
                sprintf(s_hp_new_flash_data.table[1].data, (char_t *) ACTIVE_ROB_WEATHER_KEY);
                s_hp_new_flash_data.table[1].length_of_record_bytes = strlen(s_hp_new_flash_data.table[1].data);
#endif

// print_to_console("Writing blank record\r\n");

                status = FLASH_STATUS_BUSY;
                err    = R_FLASH_HP_Write(&g_flash1_ctrl,
                                          (const uint32_t) &s_hp_new_flash_data,
                                          FLASH_DF_BLOCK_0,
                                          FLASH_DATA_BLOCK_SIZE);

                while (status == FLASH_STATUS_BUSY)
                {
                    err = R_FLASH_HP_StatusGet(&g_flash1_ctrl, &status);
                }
            }
            else
            {
                memcpy(&s_hp_new_flash_data, FLASH_DF_BLOCK_0, sizeof(s_hp_new_flash_data));

                /* record is (id -1) as id 0 is not used */
                res = s_hp_new_flash_data.table[id - 1].key_valid;

                if (s_hp_new_flash_data.table[id - 1].key_valid == false)
                {
                    update_key_data(id);
                }
            }
        }

        R_FLASH_HP_Close(&g_flash1_ctrl);
    }

    return res;
}

/**********************************************************************************************************************
 * End of function is_key_data_available
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: erase_stored_keys
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
bool_t erase_stored_keys (void)
{
    bool_t res = true;

    memset(&s_hp_new_flash_data, '\0', sizeof(s_hp_new_flash_data));
    memcpy(&s_hp_new_flash_data.id_block, KEY_DATA_IDENTIFIER, strlen(KEY_DATA_IDENTIFIER));

    s_hp_new_flash_data.table[0].key_valid = false;
    s_hp_new_flash_data.table[1].key_valid = false;

    update_stored_keys();

    return res;
}

/**********************************************************************************************************************
 * End of function erase_stored_keys
 *********************************************************************************************************************/
