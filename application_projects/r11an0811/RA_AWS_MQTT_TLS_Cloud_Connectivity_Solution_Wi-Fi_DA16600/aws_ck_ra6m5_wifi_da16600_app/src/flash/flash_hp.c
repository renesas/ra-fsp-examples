/***********************************************************************************************************************
 * File Name    : flash_hp.c
 * Description  : Contains macros, data structures and functions used in flash_hp.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include <flash/flash_hp.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "common_utils.h"
#include "console_menu/console.h"
#include "console_menu/menu_flash.h"

/* Flags, set from Callback function */
static volatile _Bool is_b_flash_event_not_blank = false;
static volatile _Bool is_b_flash_event_blank = false;
static volatile _Bool is_b_flash_event_erase_complete = false;
static volatile _Bool is_b_flash_event_write_complete = false;

/* Static Function */
static fsp_err_t blankcheck_event_flag(void);

extern char g_write_buffer[BUFFER_SIZE];
extern uint8_t g_start_read_flag;
extern bool g_cred_type_flag;
extern bool g_cred_type_flag_ssid;
extern bool g_cred_type_flag_pass;
extern uint16_t g_counter;
extern bool none_secure_ChkFlag;
static uint32_t g_allocated_size, g_free_size, g_physical_size;
char g_certificate[2048];
char g_private_key[2048];
char g_mqtt_endpoint[128];
char g_iot_thing_name[128];
char g_ssid[64];
char g_pass[64];
char g_secure[64];
uint32_t g_flash_hp_df_block, g_num_bytes, g_num_blocks;
bool mem_flash_info_flag = false;
credentials_mem_map_t g_crdentials_mem[8];

static char read_buffer[2048]= {0};

/*******************************************************************************************************************//**
 * @brief This function display contains of main menu.
 * @param[IN]   credential_type         Type of credential
 **********************************************************************************************************************/
void flash_display_menu(uint8_t credential_type)
{
    char_t s_print_buffer[BUFFER_LENGTH_SHORT] = {};

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    printf_colour((void*)s_print_buffer);

    if (CERTIFICATE == credential_type)
    {
        sprintf (s_print_buffer, FLASH_WRITE_CERT_MENU);
        printf_colour((void*)s_print_buffer);
        sprintf (s_print_buffer, SELECT_FILE);
    }
    else if (PRIVATE_KEY == credential_type)
    {
        sprintf (s_print_buffer, FLASH_WRITE_KEY_MENU);
        printf_colour((void*)s_print_buffer);
        sprintf (s_print_buffer, SELECT_FILE);
    }
    else if (IOT_THING_NAME == credential_type)
    {
        sprintf (s_print_buffer, FLASH_WRITE_THING_NAME);
        printf_colour((void*)s_print_buffer);
        sprintf (s_print_buffer, TYPE_CREDENTIALS);
    }
    else if (MQTT_ENDPOINT == credential_type)
    {
        sprintf (s_print_buffer, FLASH_WRITE_MQTT_END_POINT);
        printf_colour((void*)s_print_buffer);
        sprintf (s_print_buffer, TYPE_CREDENTIALS);
    }
    else if (SSID == credential_type)
    {
        sprintf (s_print_buffer, FLASH_WRITE_SSID_MENU);
        printf_colour((void*)s_print_buffer);
        sprintf (s_print_buffer, TYPE_CREDENTIALS);
    }
    else if (PASS == credential_type)
    {
        sprintf (s_print_buffer, FLASH_WRITE_PASSWORD_MENU);
        printf_colour((void*)s_print_buffer);
        sprintf (s_print_buffer, TYPE_WIFI_PASSWORD);
    }
    else
    {
        /* do nothing */
    }
    printf_colour((void*)s_print_buffer);
}

/*******************************************************************************************************************//**
 * @brief This function is called from the terminal input to do the data flash operations.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful FLash_HP data flash operations.
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP data flash operations.
 **********************************************************************************************************************/
fsp_err_t aws_certficate_write(uint8_t cert_type)
{
    fsp_err_t err = FSP_SUCCESS;
    flash_memory_mapping ();
    bool write_flash = true;

    /* enable storing of incoming data on UART to g_write_buffer */
    g_start_read_flag = true;
    while (1)
    {
        /* Check for the end of the certificate */
        if ((strstr (g_write_buffer, END_OF_AWS_CERTIFICATE)) && (CERTIFICATE == cert_type))
        {
            g_start_read_flag = false;
            APP_DBG_PRINT("\r\nFile content saved in buffer\r\n");
            break;
        }

        /* Check for the end of the private key */
        else if (strstr (g_write_buffer, END_OF_AWS_PRIVATE_KEY) && (PRIVATE_KEY == cert_type))
        {
            g_start_read_flag = false;
            APP_DBG_PRINT("\r\nFile content saved in buffer\r\n");
            break;
        }

        /* Check for end of SSID and length of SSID */
        if ((g_cred_type_flag_ssid == true) && strchr (g_write_buffer, '\r'))
        {
            if (((strlen((const char *)g_write_buffer)-1) < SSID_MIN) || ((strlen((const char *)g_write_buffer)-1) > SSID_MAX))
            {
                APP_PRINT("Length of SSID is out of supported range.\n");
                g_cred_type_flag_ssid = false;
                write_flash = false;
                break;
            }
            else
            {
                g_cred_type_flag_ssid = false;
                g_start_read_flag = false;
                g_cred_type_flag = false;
                APP_DBG_PRINT("\r\nFile content saved in buffer\r\n");
                break;
            }
        }
        else if ((g_cred_type_flag == true) && strchr (g_write_buffer, '\r')) {
            g_start_read_flag = false;
            g_cred_type_flag = false;
            APP_DBG_PRINT("\r\nFile content saved in buffer\r\n");
            break;
        }
        else if (g_cred_type_flag_pass == true){
            g_start_read_flag = false;
            g_cred_type_flag_pass = false;
            APP_DBG_PRINT("\r\nPassword saved in buffer\r\n");
            break;
        }

        vTaskDelay (1);
    }

    if (CERTIFICATE == cert_type)
    {
        g_flash_hp_df_block = FLASH_HP_DF_BLOCK_CERTIFICATE;
        g_num_bytes = BLOCK_SIZE_CERT;
        g_num_blocks = BLOCK_NUM_CERT;
        g_crdentials_mem[CERTIFICATE].length = strlen (g_write_buffer);
    }
    else if (PRIVATE_KEY == cert_type)
    {
        g_flash_hp_df_block = FLASH_HP_DF_BLOCK_KEY;
        g_num_bytes = BLOCK_SIZE_KEY;
        g_num_blocks = BLOCK_NUM_KEY;
        g_crdentials_mem[PRIVATE_KEY].length = strlen (g_write_buffer);
    }
    else if (IOT_THING_NAME == cert_type)
    {
        g_flash_hp_df_block = FLASH_HP_DF_IOT_THING_NAME;
        g_num_bytes = BLOCK_SIZE_IOT_THING;
        g_num_blocks = BLOCK_NUM_IOT_THING;
        g_crdentials_mem[IOT_THING_NAME].length = strlen (g_write_buffer);
    }
    else if (MQTT_ENDPOINT == cert_type)
    {
        g_flash_hp_df_block = FLASH_HP_DF_MQTT_END_POINT;
        g_num_bytes = BLOCK_SIZE_MQTT_ENDPOINT;
        g_num_blocks = BLOCK_NUM_MQTT_ENDPOINT;
        g_crdentials_mem[MQTT_ENDPOINT].length = strlen (g_write_buffer);
    }
    else if (SSID == cert_type)
    {
        if (write_flash == true)
        {
            g_flash_hp_df_block = FLASH_HP_DF_SSID;
            g_num_bytes = BLOCK_SIZE_SSID;
            g_num_blocks = BLOCK_NUM_SSID;
            g_crdentials_mem[SSID].length = strlen (g_write_buffer);
        }
        else
        {
            /* Do nothing */
        }
    }
    else if (PASS == cert_type)
    {
        g_flash_hp_df_block = FLASH_HP_DF_PASSWORD;
        g_num_bytes = BLOCK_SIZE_PASSWORD;
        g_num_blocks = BLOCK_NUM_PASSWORD;
        g_crdentials_mem[PASS].length = strlen (g_write_buffer);
    }

    /* Store details of credentials into flash memory */
    if (write_flash == true){
        err = flash_data_write ();
        if (err != FSP_SUCCESS)
        {
            printf_colour ("\r\n[RED]Writing flash data is failed[WHITE]\r\n");
            return err;
        }
        else
        {
            err = store_flashed_data_info (cert_type);
            if (err != FSP_SUCCESS)
            {
                printf_colour ("\r\n[RED]Writing flash data is failed[WHITE]\r\n");
                return err;
            }
            else
            {
                printf_colour ("\r\n[GREEN]Writing flash data is successful[WHITE]\r\n");
            }
        }
    }
    else {
        memset (g_write_buffer, 0, BUFFER_SIZE);
        g_counter = 0;
        clear_receive_buffer ();
    }

    printf_colour (MENU_RETURN_INFO);
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is called from the terminal input to write security into the data flash.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Write security into the data flash successfully.
 * @retval      Any Other Error code    Write security into the data flash unsuccessfully.
 **********************************************************************************************************************/
fsp_err_t aws_certficate_write_secure(void)
{
    fsp_err_t err = FSP_SUCCESS;
    flash_memory_mapping ();

    /* enable storing of incoming data on UART to g_write_buffer */
    g_start_read_flag = false;
    g_flash_hp_df_block = FLASH_HP_DF_SECURITY;
    g_num_bytes = BLOCK_SIZE_SECURITY;
    g_num_blocks = BLOCK_NUM_SECURITY;
    g_crdentials_mem[SECURE].length = strlen (g_write_buffer);

    /* Store details of credentials into flash memory */
    err = flash_data_write ();
    if (err != FSP_SUCCESS)
    {
        printf_colour ("\r\n[RED]Writing flash data is failed[WHITE]\r\n");
        return err;
    }
    else
    {
        err = store_flashed_data_info (SECURE);
        if (err != FSP_SUCCESS)
        {
            printf_colour ("\r\n[RED]Writing flash data is failed[WHITE]\r\n");
            return err;
        }
        else
        {
            printf_colour ("\r\n[GREEN]Writing flash data is successful[WHITE]\r\n");
        }
    }

    //printf_colour (MENU_RETURN_INFO);
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is called from the RTT input to do the data flash operations.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful FLash_HP data flash operations.
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP data flash operations.
 **********************************************************************************************************************/
fsp_err_t flash_data_write(void)
{
    fsp_err_t err = FSP_SUCCESS;
    flash_result_t blank_check_result = FLASH_RESULT_BLANK;

    /* Erase Block */
    err = R_FLASH_HP_Erase (&user_flash_ctrl, g_flash_hp_df_block, g_num_blocks);
    /* Error Handle */
    if (FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nErase API failed, Restart the Application");
        return err;
    }

    /* Wait for the erase complete event flag, if BGO is SET  */
    if (true == user_flash_cfg.data_flash_bgo)
    {
        APP_DBG_PRINT("\r\nBGO has enabled, Data Flash erase is in progress");
        while (!is_b_flash_event_erase_complete);
        is_b_flash_event_erase_complete = false;
    }

    /* Data flash blank check */
    err = R_FLASH_HP_BlankCheck (&user_flash_ctrl, g_flash_hp_df_block, FLASH_HP_DF_BLOCK_SIZE, &blank_check_result);
    /* Error Handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nBlankCheck API failed, Restart the Application");
        return err;
    }

    /* Validate the blank check result */
    if (FLASH_RESULT_BLANK == blank_check_result)
    {
        APP_DBG_PRINT("\r\n BlankCheck is successful");
    }
    else if (FLASH_RESULT_NOT_BLANK == blank_check_result)
    {
        APP_ERR_PRINT("\r\n BlankCheck failed. cannot write the data. Try Restarting the application");
        return (fsp_err_t) FLASH_RESULT_NOT_BLANK;
    }
    else if (FLASH_RESULT_BGO_ACTIVE == blank_check_result)
    {
        /* BlankCheck will update in Callback */
        /* Event flag will be updated in the blank check function when BGO is enabled */
        err = blankcheck_event_flag ();
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    else
    {
        /* No Operation */
    }

    if (mem_flash_info_flag == true)
    {
        /* Stores details of credentials into flash memory*/
        err = R_FLASH_HP_Write (&user_flash_ctrl, (uint32_t) g_crdentials_mem, g_flash_hp_df_block, g_num_bytes);
        /* Error Handle */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nWrite API failed, Restart the Application");
            return err;
        }
        mem_flash_info_flag = false;
    }
    else
    {

        /* Write credentials into flash memory */
        err = R_FLASH_HP_Write (&user_flash_ctrl, (uint32_t) g_write_buffer, g_flash_hp_df_block, g_num_bytes);
        /* Error Handle */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nWrite API failed, Restart the Application");
            return err;
        }
    }
    /* Wait for the write complete event flag, if BGO is SET  */
    if (true == user_flash_cfg.data_flash_bgo)
    {
        while (!is_b_flash_event_write_complete);
        is_b_flash_event_write_complete = false;
    }

    memset (g_write_buffer, 0, BUFFER_SIZE);
    g_counter = 0;
    clear_receive_buffer ();

    return err;
}

/*******************************************************************************************************************//**
 * @brief This function stores updated credential details in flash memory in form of structure
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful FLash_HP data flash operations.
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP data flash operations.
 **********************************************************************************************************************/
fsp_err_t store_flashed_data_info (uint8_t cert_type)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Assign start address of flash memory to store details related to credentials */
    g_flash_hp_df_block = FLASH_HP_DF_DATA_INFO;
    /* Assign number of bytes */
    g_num_bytes = BLOCK_SIZE_DATA_INFO;
    /* Assign number of blocks */
    g_num_blocks = BLOCK_NUM_DATA_INFO;

    /* assign certificate type */
    if (CERTIFICATE == cert_type)
    {
        strcpy((char *)g_crdentials_mem[0].stored_in_flash, (char *)STRING_SAVE);
    }
    else if (PRIVATE_KEY == cert_type)
    {
        strcpy((char *)g_crdentials_mem[1].stored_in_flash, (char *)STRING_SAVE);
    }
    else if ( MQTT_ENDPOINT == cert_type)
    {
        strcpy((char *)g_crdentials_mem[2].stored_in_flash, (char *)STRING_SAVE);
    }
    else if (IOT_THING_NAME == cert_type)
    {
        strcpy((char *)g_crdentials_mem[3].stored_in_flash, (char *)STRING_SAVE);
    }
    else if (SSID == cert_type)
    {
        strcpy((char *)g_crdentials_mem[4].stored_in_flash, (char *)STRING_SAVE);
    }
    else if (PASS == cert_type)
    {
        strcpy((char *)g_crdentials_mem[5].stored_in_flash, (char *)STRING_SAVE);
    }
    else if (SECURE == cert_type)
    {
        strcpy((char *)g_crdentials_mem[6].stored_in_flash, (char *)STRING_SAVE);
    }


    /* enable flag to write data into flash */
    mem_flash_info_flag = true;
    err = flash_data_write ();
    if (err != FSP_SUCCESS)
    {
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is called from the RTT input to do the data flash operations.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful FLash_HP data flash operations.
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP data flash operations.
 **********************************************************************************************************************/
fsp_err_t flash_hp_data_read(bool print_data)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t count = 0;
    char_t s_print_buffer[BUFFER_LENGTH_SHORT] =
    { };

    /* update g_crdentials_mem structure */
    flash_memory_mapping ();

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_FLASH_READ);
    printf_colour ((void*) s_print_buffer);

    while (count < CREDENTIAL_COUNT)
    {
        if (count == CERTIFICATE)
        {
            /* Check if certificate is stored in flash memory or not */
            if (0 == strncmp ((char *)g_crdentials_mem[count].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
            {
                /* Copy certificate from flash memory to buffer and validate certificate */
                memcpy (read_buffer, (uint8_t*) g_crdentials_mem[count].addr, g_crdentials_mem[count].num_bytes);
                if ((NULL != strstr (read_buffer, "-----END CERTIFICATE-----"))
                        && (strlen (read_buffer) == g_crdentials_mem[CERTIFICATE].length))
                {
                    if (print_data == true)
                    {
                        printf_colour ("\r\n[GREEN]AWS certificate read successful[WHITE]\r\n");
                        /* Print the read data on the RTT terminal */
                        printf_colour (read_buffer);
                        printf_colour ("\r\n");
                    }
                    /* Clear buffer */
                    memset (read_buffer, 0, g_crdentials_mem[count].num_bytes);
                }
                else
                {
                    printf_colour (
                            "\r\n [RED]Certificate is not saved in data flash[WHITE]\r\n");

                }
            }

        }
        if (count == PRIVATE_KEY)
        {
            if (0 == strncmp ((char *)g_crdentials_mem[count].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
            {
                /* Copy key from flash memory to buffer */
                memcpy (read_buffer, (uint8_t*) g_crdentials_mem[count].addr, g_crdentials_mem[count].num_bytes);
                if (print_data == true)
                {
                    printf_colour ("\r\n[GREEN]AWS private key read is successful[WHITE]\r\n");
                    /* Print the read data on the RTT terminal */
                    printf_colour (read_buffer);
                    printf_colour ("\r\n");
                }
                memset (read_buffer, 0, g_crdentials_mem[count].num_bytes);
            }

        }
        if (count == MQTT_ENDPOINT)
        {
            if (0 == strncmp ((char *)g_crdentials_mem[count].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
            {
                /* Copy mqtt end point from flash memory to buffer */
                memcpy (read_buffer, (uint8_t*) g_crdentials_mem[count].addr, g_crdentials_mem[count].num_bytes);
                {
                    printf_colour ("\r\n[GREEN]AWS MQTT end point read successful[WHITE]\r\n");
                    /* Print the read data on the RTT terminal */
                    printf_colour (read_buffer);
                }
                memset (read_buffer, 0, g_crdentials_mem[count].num_bytes);
            }
        }

        if (count == IOT_THING_NAME)
        {
            if (0 == strncmp ((char *)g_crdentials_mem[count].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
            {
                /* Copy IOT thing name from flash memory to buffer */
                memcpy (read_buffer, (uint8_t*) g_crdentials_mem[count].addr, g_crdentials_mem[count].num_bytes);

                if (print_data == true)
                {
                    printf_colour ("\r\n[GREEN]IOT thing name read successful[WHITE]\r\n");
                    /* Print the read data on the RTT terminal */
                    printf_colour (read_buffer);
                }
            }
        }
        if (count == SSID)
        {
            if (0 == strncmp ((char *)g_crdentials_mem[count].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
            {
                /* Copy SSID from flash memory to buffer */
                memcpy (read_buffer, (uint8_t*) g_crdentials_mem[count].addr, g_crdentials_mem[count].num_bytes);

                if (print_data == true)
                {
                    printf_colour ("\r\n[GREEN]SSID read successful[WHITE]\r\n");
                    /* Print the read data on the RTT terminal */
                    printf_colour (read_buffer);
                }
            }
        }
        if (count == PASS)
        {
            if (0 == strncmp ((char *)g_crdentials_mem[count].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
            {
                /* Copy PASS from flash memory to buffer */
                memcpy (read_buffer, (uint8_t*) g_crdentials_mem[count].addr, g_crdentials_mem[count].num_bytes);

                if (print_data == true)
                {
                    printf_colour ("\r\n[GREEN]PASS read successful[WHITE]\r\n");
                    /* Print the read data on the RTT terminal */
                    printf_colour (read_buffer);
                }
            }
        }
        if (count == SECURE)
            {
            if (0 == strncmp ((char *)g_crdentials_mem[count].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
                {
                    /* Copy PASS from flash memory to buffer */
                    memcpy (read_buffer, (uint8_t*) g_crdentials_mem[count].addr, g_crdentials_mem[count].num_bytes);

                    if (print_data == true)
                    {
                        printf_colour ("\r\n[GREEN]SECURE read successful[WHITE]\r\n");
                        /* Print the read data on the RTT terminal */
                        printf_colour (read_buffer);
                    }
                }
            }

        count++;
    }

    printf_colour (MENU_RETURN_INFO);
    count = 0;
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function init flash memory
 * @param[IN]
 * @retval      FSP_SUCCESS             Upon successful Flash_HP is blank
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP is not blank
 **********************************************************************************************************************/
fsp_err_t flash_mem_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = flash_stored_data_info ();
    assert(FSP_SUCCESS == err);

    flash_memory_mapping ();

    return err;
}

/*******************************************************************************************************************//**
 * @brief This function checks stored credentials in flash memory
 * @retval      FSP_SUCCESS             Upon successful Flash_HP is blank
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP is not blank
 **********************************************************************************************************************/
fsp_err_t flash_stored_data_info(void)
{
    fsp_err_t err = FSP_SUCCESS;
    memcpy (g_crdentials_mem, (credentials_mem_map_t*) FLASH_HP_DF_DATA_INFO, sizeof(g_crdentials_mem));

    if (0 != strncmp ((char *)g_crdentials_mem[CERTIFICATE].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        memset (g_crdentials_mem[CERTIFICATE].stored_in_flash, 0, LENGTH_SAVE);
    }

    if (0 != strncmp ((char *)g_crdentials_mem[PRIVATE_KEY].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        memset (g_crdentials_mem[PRIVATE_KEY].stored_in_flash, 0, LENGTH_SAVE);
    }

    if (0 != strncmp ((char *)g_crdentials_mem[MQTT_ENDPOINT].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        memset (g_crdentials_mem[MQTT_ENDPOINT].stored_in_flash, 0, LENGTH_SAVE);
    }

    if (0 != strncmp ((char *)g_crdentials_mem[IOT_THING_NAME].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        memset (g_crdentials_mem[IOT_THING_NAME].stored_in_flash, 0, LENGTH_SAVE);
    }
    if (0 != strncmp ((char *)g_crdentials_mem[SSID].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        memset (g_crdentials_mem[SSID].stored_in_flash, 0, LENGTH_SAVE);
    }
    if (0 != strncmp ((char *)g_crdentials_mem[PASS].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        memset (g_crdentials_mem[PASS].stored_in_flash, 0, LENGTH_SAVE);
    }
    if (0 != strncmp ((char *)g_crdentials_mem[SECURE].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        memset (g_crdentials_mem[SECURE].stored_in_flash, 0, LENGTH_SAVE);
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function stores flash memory details in structure
 **********************************************************************************************************************/
void flash_memory_mapping(void)
{
    g_crdentials_mem[CERTIFICATE].num_bytes = BLOCK_SIZE_CERT;
    g_crdentials_mem[CERTIFICATE].num_block = BLOCK_NUM_CERT;
    g_crdentials_mem[CERTIFICATE].addr = FLASH_HP_DF_BLOCK_CERTIFICATE;

    g_crdentials_mem[PRIVATE_KEY].num_bytes = BLOCK_SIZE_KEY;
    g_crdentials_mem[PRIVATE_KEY].num_block = BLOCK_NUM_KEY;
    g_crdentials_mem[PRIVATE_KEY].addr = FLASH_HP_DF_BLOCK_KEY;

    g_crdentials_mem[MQTT_ENDPOINT].num_bytes = BLOCK_SIZE_MQTT_ENDPOINT;
    g_crdentials_mem[MQTT_ENDPOINT].num_block = BLOCK_NUM_MQTT_ENDPOINT;
    g_crdentials_mem[MQTT_ENDPOINT].addr = FLASH_HP_DF_MQTT_END_POINT;

    g_crdentials_mem[IOT_THING_NAME].num_bytes = BLOCK_SIZE_IOT_THING;
    g_crdentials_mem[IOT_THING_NAME].num_block = BLOCK_NUM_IOT_THING;
    g_crdentials_mem[IOT_THING_NAME].addr = FLASH_HP_DF_IOT_THING_NAME;

    g_crdentials_mem[SSID].num_bytes = BLOCK_SIZE_SSID;
    g_crdentials_mem[SSID].num_block = BLOCK_NUM_SSID;
    g_crdentials_mem[SSID].addr = FLASH_HP_DF_SSID;

    g_crdentials_mem[PASS].num_bytes = BLOCK_SIZE_PASSWORD;
    g_crdentials_mem[PASS].num_block = BLOCK_NUM_PASSWORD;
    g_crdentials_mem[PASS].addr = FLASH_HP_DF_PASSWORD;

    g_crdentials_mem[SECURE].num_bytes = BLOCK_SIZE_SECURITY;
    g_crdentials_mem[SECURE].num_block = BLOCK_NUM_SECURITY;
    g_crdentials_mem[SECURE].addr = FLASH_HP_DF_SECURITY;

    g_crdentials_mem[FLASH_DATA_INFO].num_bytes = BLOCK_SIZE_DATA_INFO;
    g_crdentials_mem[FLASH_DATA_INFO].num_block = BLOCK_NUM_DATA_INFO;
    g_crdentials_mem[FLASH_DATA_INFO].addr = FLASH_HP_DF_DATA_INFO;
}

/*******************************************************************************************************************//**
 * @brief This function provide flash memory information
 **********************************************************************************************************************/
void flash_info(void)
{
    static char_t s_print_buffer[BUFFER_LENGTH_SHORT] ={ };
    g_allocated_size = 0;

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_FLASH_INFO);
    printf_colour ((void*) s_print_buffer);

    /* Total available flash memory */
    g_physical_size = TOTAL_BLOCK_SIZE;

    if (0 == strncmp ((char *)g_crdentials_mem[CERTIFICATE].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        g_allocated_size = g_crdentials_mem[CERTIFICATE].length;
    }
    if (0 == strncmp ((char *)g_crdentials_mem[PRIVATE_KEY].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        g_allocated_size += g_crdentials_mem[PRIVATE_KEY].length;
    }
    if (0 == strncmp ((char *)g_crdentials_mem[MQTT_ENDPOINT].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        g_allocated_size += g_crdentials_mem[MQTT_ENDPOINT].length;
    }
    if (0 == strncmp ((char *)g_crdentials_mem[IOT_THING_NAME].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        g_allocated_size += g_crdentials_mem[IOT_THING_NAME].length;
    }
    if (0 == strncmp ((char *)g_crdentials_mem[SSID].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        g_allocated_size += g_crdentials_mem[SSID].length;
    }
    if (0 == strncmp ((char *)g_crdentials_mem[PASS].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        g_allocated_size += g_crdentials_mem[PASS].length;
    }
    if (0 == strncmp ((char *)g_crdentials_mem[SECURE].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        g_allocated_size += g_crdentials_mem[SECURE].length;
    }

    /* Free size available in flash memory */
    g_free_size = g_physical_size - g_allocated_size;

    sprintf (s_print_buffer, " \r\n Physical size:  %ld bytes \r\n Allocated_size: %ld bytes"
             "\r\n Free_size:      %ld bytes",
             g_physical_size, g_allocated_size, g_free_size);
    printf_colour (s_print_buffer);

    printf_colour (MENU_RETURN_INFO);
}

/*******************************************************************************************************************//**
 * @brief This function validate AWS credentials stored in flash.
 * @param[IN]
 * @retval      FSP_SUCCESS             Upon successful Flash_HP is blank
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP is not blank
 **********************************************************************************************************************/
fsp_err_t check_credentials_stored(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char_t s_print_buffer[BUFFER_LENGTH_SHORT] =
    { };
    err = flash_stored_data_info ();
    assert(FSP_SUCCESS == err);
    /* Clear screen */
    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    printf_colour ((void*) s_print_buffer);

    /* Print credential check menu title */
    sprintf (s_print_buffer, FLASH_CHECK_CREDENTIALS);
    printf_colour ((void*) s_print_buffer);

    /* Check if credential is stored in flash */
    if (0 == strncmp ((char *)g_crdentials_mem[CERTIFICATE].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        /* Copy credential from flash memory to buffer */
        memcpy (read_buffer, (uint8_t*) g_crdentials_mem[CERTIFICATE].addr, g_crdentials_mem[CERTIFICATE].num_bytes);
        /* Validate credential */
        if ((NULL != strstr (read_buffer, "-----END CERTIFICATE-----"))
                && (strlen (read_buffer) == g_crdentials_mem[CERTIFICATE].length))
        {
            memcpy (g_certificate, read_buffer, strlen (read_buffer));
            printf_colour ("\r\n [GREEN]Certificate saved in data flash is verified and successful[WHITE]\r\n");
            memset (read_buffer, 0, strlen (read_buffer));
        }
        else
        {
            printf_colour ("\r\n [RED]Certificate is not saved in data flash[WHITE]\r\n");
            memset (g_crdentials_mem[CERTIFICATE].stored_in_flash, 0, LENGTH_SAVE);
            return FSP_ERR_ABORTED;
        }
    }
    else
    {
        printf_colour ("\r\n [RED]Certificate is not saved in data flash[WHITE]\r\n");
        return FSP_ERR_ABORTED;
    }

    /* Check if credential is stored in flash */
    if (0 == strncmp ((char *)g_crdentials_mem[PRIVATE_KEY].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        /* Copy credential from flash memory to buffer */
        memcpy (read_buffer, (uint8_t*) g_crdentials_mem[PRIVATE_KEY].addr, g_crdentials_mem[PRIVATE_KEY].num_bytes);
        /* Validate credential */
        if ((NULL != strstr (read_buffer, "-----END RSA PRIVATE KEY-----"))
                && (strlen (read_buffer) == g_crdentials_mem[PRIVATE_KEY].length))
        {
            memcpy (g_private_key, read_buffer, strlen (read_buffer));
            printf_colour ("\r\n [GREEN]Private key saved in data flash is verified and successful[WHITE]\r\n");
            memset (read_buffer, 0, strlen (read_buffer));
        }
        else
        {
            printf_colour ("\r\n [RED]Private key is not saved in data flash[WHITE]\r\n");
            memset (g_crdentials_mem[PRIVATE_KEY].stored_in_flash, 0, LENGTH_SAVE);
            return FSP_ERR_ABORTED;
        }
    }
    else
    {
        printf_colour ("\r\n [RED]Private key is not saved in data flash[WHITE]\r\n");
        return FSP_ERR_ABORTED;
    }

    /* Check if credential is stored in flash */
    if (0 == strncmp ((char *)g_crdentials_mem[MQTT_ENDPOINT].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        /* Copy credential from flash memory to buffer */
        memcpy (read_buffer, (uint8_t*) g_crdentials_mem[MQTT_ENDPOINT].addr,
                g_crdentials_mem[MQTT_ENDPOINT].num_bytes);
        /* Validate credential */
        if (strlen (read_buffer) == g_crdentials_mem[MQTT_ENDPOINT].length)
        {

            memcpy (g_mqtt_endpoint, read_buffer, strlen (read_buffer));
            /* Remove enter character '\r' from MQTT end point */
            if (g_mqtt_endpoint[strlen (g_mqtt_endpoint) - 1] == '\r')
            {
                g_mqtt_endpoint[strlen (g_mqtt_endpoint) - 1] = '\0';
            }
            printf_colour ("\r\n [GREEN]MQTT end point saved in data flash is verified and successful[WHITE]\r\n");
            memset (read_buffer, 0, strlen (read_buffer));
        }
        else
        {
            printf_colour (
                    "\r\n [RED]MQTT end point is not saved in data flash[WHITE]\r\n");
            memset (g_crdentials_mem[MQTT_ENDPOINT].stored_in_flash, 0, LENGTH_SAVE);
            return FSP_ERR_ABORTED;
        }
    }
    else
    {
        printf_colour ("\r\n [RED]MQTT end point is not saved in data flash[WHITE]\r\n");
        return FSP_ERR_ABORTED;
    }

    /* Check if credential is stored in flash */
    if (0 == strncmp ((char *)g_crdentials_mem[IOT_THING_NAME].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        /* Copy credential from flash memory to buffer */
        memcpy (read_buffer, (uint8_t*) g_crdentials_mem[IOT_THING_NAME].addr,
                g_crdentials_mem[IOT_THING_NAME].num_bytes);
        /* Validate credential */
        if (strlen (read_buffer) == g_crdentials_mem[IOT_THING_NAME].length)
        {
            memcpy (g_iot_thing_name, read_buffer, strlen (read_buffer));
            /* Remove enter character '\r' from IOT thing name */
            if (g_iot_thing_name[strlen (g_iot_thing_name) - 1] == '\r')
            {
                g_iot_thing_name[strlen (g_iot_thing_name) - 1] = '\0';
            }
            printf_colour ("\r\n [GREEN]IOT thing name saved in data flash is verified and successful[WHITE]\r\n");
            printf_colour ("\r\n");
            memset (read_buffer, 0, strlen (read_buffer));
        }
        else
        {
            printf_colour (
                    "\r\n [RED]IOT thing name is not saved in data flash[WHITE]\r\n");
            memset (g_crdentials_mem[IOT_THING_NAME].stored_in_flash, 0, LENGTH_SAVE);
            return FSP_ERR_ABORTED;
        }
    }
    else
    {
        printf_colour ("\r\n [RED]IOT thing name is not saved in data flash[WHITE]\r\n");
        return FSP_ERR_ABORTED;
    }

    /* Check if credential is stored in flash */
    if (0 == strncmp ((char *)g_crdentials_mem[SSID].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        /* Copy credential from flash memory to buffer */
        memcpy (read_buffer, (uint8_t*) g_crdentials_mem[SSID].addr,
                g_crdentials_mem[SSID].num_bytes);
        /* Validate credential */
        if (strlen (read_buffer) == g_crdentials_mem[SSID].length)
        {
            memcpy (g_ssid, read_buffer, strlen (read_buffer));
            /* Remove enter character '\r' from IOT thing name */
            if (g_ssid[strlen (g_ssid) - 1] == '\r')
            {
                g_ssid[strlen (g_ssid) - 1] = '\0';
            }
            printf_colour ("\r\n [GREEN]SSID saved in data flash is verified and successful[WHITE]\r\n");
            printf_colour ("\r\n");
            memset (read_buffer, 0, strlen (read_buffer));
        }
        else
        {
            printf_colour (
                    "\r\n [RED]SSID is not saved in data flash[WHITE]\r\n");
            memset (g_crdentials_mem[SSID].stored_in_flash, 0, LENGTH_SAVE);
            return FSP_ERR_ABORTED;
        }
    }
    else
    {
        printf_colour ("\r\n [RED]SSID is not saved in data flash[WHITE]\r\n");
        return FSP_ERR_ABORTED;
    }

    /* Check if credential is stored in flash */
    if (0 == strncmp ((char *)g_crdentials_mem[SECURE].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        /* Copy credential from flash memory to buffer */
        memcpy (read_buffer, (uint8_t*) g_crdentials_mem[SECURE].addr,
                g_crdentials_mem[SECURE].num_bytes);
        /* Validate credential */
        if (strlen (read_buffer) == g_crdentials_mem[SECURE].length)
        {
            memcpy (g_secure, read_buffer, strlen (read_buffer));
            /* Remove enter character '\r' from IOT thing name */
            if (g_secure[strlen (g_secure) - 1] == '\r')
            {
                g_secure[strlen (g_secure) - 1] = '\0';
            }
            printf_colour ("\r\n [GREEN]Security saved in data flash is verified and successful[WHITE]\r\n");
            printf_colour ("\r\n");
            memset (read_buffer, 0, strlen (read_buffer));
            if( strcmp(g_secure, "None") == 0 )
            {
                none_secure_ChkFlag = true;
            }
            else
            {
                none_secure_ChkFlag = false;
            }
        }
        else
        {
            printf_colour (
                    "\r\n [RED]Security is not saved in data flash[WHITE]\r\n");
            memset (g_crdentials_mem[SECURE].stored_in_flash, 0, LENGTH_SAVE);
            none_secure_ChkFlag = false;
            return FSP_ERR_ABORTED;
        }
    }
    else
    {
        printf_colour ("\r\n [RED]Security is not saved in data flash[WHITE]\r\n");
        return FSP_ERR_ABORTED;
    }

    /* Check if credential is stored in flash */
    if(none_secure_ChkFlag == true)
    {
        printf_colour("\r\n [GREEN]Is using security open, do not need Wi-fi Password[WHITE]\r\n");
    }
    else if (0 == strncmp ((char *)g_crdentials_mem[PASS].stored_in_flash, (char *)STRING_SAVE, LENGTH_SAVE))
    {
        /* Copy credential from flash memory to buffer */
        memcpy (read_buffer, (uint8_t*) g_crdentials_mem[PASS].addr,
                g_crdentials_mem[PASS].num_bytes);
        /* Validate credential */
        if (strlen (read_buffer) == g_crdentials_mem[PASS].length)
        {
            memcpy (g_pass, read_buffer, strlen (read_buffer));
            /* Remove enter character '\r' from IOT thing name */
            if (g_pass[strlen (g_pass) - 1] == '\r')
            {
                g_pass[strlen (g_pass) - 1] = '\0';
            }
            printf_colour ("\r\n [GREEN]Password saved in data flash is verified and successful[WHITE]\r\n");
            printf_colour ("\r\n");
            memset (read_buffer, 0, strlen (read_buffer));
        }
        else
        {
            printf_colour (
                    "\r\n [RED]Password is not saved in data flash[WHITE]\r\n");
            memset (g_crdentials_mem[PASS].stored_in_flash, 0, LENGTH_SAVE);
            return FSP_ERR_ABORTED;
        }
    }
    else
    {
        printf_colour ("\r\n [RED]Password is not saved in data flash[WHITE]\r\n");
        return FSP_ERR_ABORTED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief This function prints help menu.
 **********************************************************************************************************************/
void help_menu(void)
{
    char_t s_print_buffer[BUFFER_LENGTH_SHORT] =
    { };

    sprintf (s_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    sprintf (s_print_buffer, MODULE_NAME_FLASH_HELP);

    /* ignoring -Wpointer-sign is OK when treating signed char_t array as as unsigned */
    printf_colour ((void*) s_print_buffer);

    /* Print help menu content to console */
    printf_colour (FLASH_HELP_TAG1);
    printf_colour (FLASH_HELP_TAG2);
    printf_colour (FLASH_HELP_TAG3);
    printf_colour (FLASH_HELP_TAG4);
    printf_colour (FLASH_HELP_TAG5);
    printf_colour (FLASH_HELP_TAG6);
    printf_colour (FLASH_HELP_TAG7);
    printf_colour (MENU_RETURN_INFO);
}

/*******************************************************************************************************************//**
 * @brief       This function de-initializes Flash_HP module.
 **********************************************************************************************************************/
void flash_hp_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_FLASH_HP_Close (&user_flash_ctrl);
    /* Error Handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nClose API failed in BlankCheck API");
    }
}

/*******************************************************************************************************************//**
 * @brief Callback function for FLASH HP HAL
 * @param[IN]   p_args                  Pointer to callback argumet
 * @retval
 * @retval
 **********************************************************************************************************************/
void flash_callback(flash_callback_args_t *p_args)
{
    if (FLASH_EVENT_NOT_BLANK == p_args->event)
    {
        is_b_flash_event_not_blank = true;
    }
    else if (FLASH_EVENT_BLANK == p_args->event)
    {
        is_b_flash_event_blank = true;
    }
    else if (FLASH_EVENT_ERASE_COMPLETE == p_args->event)
    {
        is_b_flash_event_erase_complete = true;
    }
    else if (FLASH_EVENT_WRITE_COMPLETE == p_args->event)
    {
        is_b_flash_event_write_complete = true;
    }
    else
    {
        /*No operation */
    }
}

/*******************************************************************************************************************//**
 * @brief This function is called to set the blank check event flags
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful Flash_HP is blank
 * @retval      Any Other Error code    Upon unsuccessful Flash_HP is not blank
 **********************************************************************************************************************/
static fsp_err_t blankcheck_event_flag(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Wait for callback function to set flag */
    while (!(is_b_flash_event_not_blank || is_b_flash_event_blank));
    if (is_b_flash_event_not_blank)
    {
        APP_ERR_PRINT("\n\rFlash is not blank, not to write the data. Restart the application\n");
        /* Reset Flag */
        is_b_flash_event_not_blank = false;
        return (fsp_err_t) FLASH_EVENT_NOT_BLANK;
    }
    else
    {
        APP_PRINT("\r\nFlash is blank\n");
        /* Reset Flag */
        is_b_flash_event_blank = false;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @} (end defgroup FLASH_HP_EP)
 **********************************************************************************************************************/
