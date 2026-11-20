/***********************************************************************************************************************
 * File Name    : vee_flash_example_app.c
 * Description  : Example application layer for the VEE FLASH EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: vee_flash_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "console/uart_console.h"
#include "vee_flash_example_app.h"

/* Variable to store the status */
rm_vee_status_t g_status =
{
    .state = RESET_VALUE,
    .last_id = RESET_VALUE,
    .space_available = RESET_VALUE,
    .segment_erase_count = RESET_VALUE
};

/* Private functions */
static void vee_write_operation(void);
static void vee_read_operation(void);
static void vee_format_operation(void);
static void vee_get_status_operation(void);
static void vee_refresh_operation(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the VEE FLASH example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void vee_flash_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    uint8_t user_option = RESET_VALUE;

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize the RM_VEE_FLASH module */
    err = vee_flash_init();
    handle_error(err, "vee_flash_init failed\r\n");

    while (true)
    {
        /* Print EP menu option */
        printf(EP_MENU);

        while (!uart_console_has_char())
        {
            /* Do nothing */
        }

        /* Get the user option */
        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int)sizeof(input_buff));
        user_option = (uint8_t)atoi(input_buff);

        switch (user_option)
        {
            /* Perform Virtual EEPROM (VEEPROM) write operation */
            case WRITE:
            {
                vee_write_operation();
                break;
            }

            /* Perform VEEPROM read operation */
            case READ:
            {
                vee_read_operation();
                break;
            }

            /* Perform VEEPROM format operation */
            case FORMAT:
            {
                vee_format_operation();
                break;
            }

            /* Perform VEEPROM get status operation */
            case STATUS:
            {
                vee_get_status_operation();
                break;
            }

            /* Perform VEEPROM refresh operation */
            case REFRESH:
            {
                vee_refresh_operation();
                break;
            }

            default:
            {
                printf("\r\nInvalid input\r\n");
                break;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs the write operation on VEEPROM.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void vee_write_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    uint8_t user_option = RESET_VALUE;

    /* Record data to write on the different Record ID's */
    uint8_t data_1[DATA_SIZE] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11};
    uint8_t data_2[DATA_SIZE] = {'R', 'E', 'A', 'E', 'P', 'V', 'E', 'E', 'P', 'R', 'O', 'M'};
    float data_3[DATA_SIZE] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f};
    adxl_t data_4 = {.x_data = 100, .y_data = 150, .z_data = 190, .xy_data = 60};

    /* Print menu for write operations */
    printf(WRITE_MENU);

    while (!uart_console_has_char())
    {
        /* Do nothing */
    }

    while ((WRITE_READ_ID_1 > user_option) || (WRITE_READ_ID_4 < user_option))
    {
        /* Get the user option */
        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int)sizeof(input_buff));
        user_option = (uint8_t)atoi(input_buff);

        switch (user_option)
        {
            case WRITE_READ_ID_1:
            {
                /* Write the integer data to a VEEPROM RECORD_ID1 */
                err = vee_write(&g_vee_ctrl, RECORD_ID_1, data_1, sizeof(data_1));
                handle_error(err, "vee_write for RECORD_ID1 failed\r\n");
                printf("\r\nInteger data is written successfully on RECORD_ID1\r\n");
                break;
            }

            case WRITE_READ_ID_2:
            {
                /* Write the char data to a VEEPROM RECORD_ID2 */
                err = vee_write(&g_vee_ctrl, RECORD_ID_2, data_2, sizeof(data_2));
                handle_error(err, "vee_write for RECORD_ID2 failed\r\n");
                printf("\r\nChar data is written successfully on RECORD_ID2\r\n");
                break;
            }

            case WRITE_READ_ID_3:
            {
                /* Write the float data to a VEEPROM RECORD_ID3 */
                err = vee_write(&g_vee_ctrl, RECORD_ID_3, data_3, sizeof(data_3));
                handle_error(err, "vee_write for RECORD_ID3 failed\r\n");
                printf("\r\nFloat data is written successfully on RECORD_ID3\r\n");
                break;
            }

            case WRITE_READ_ID_4:
            {
                /* Write the structure data to a VEEPROM RECORD_ID4 */
                err = vee_write(&g_vee_ctrl, RECORD_ID_4, &data_4, sizeof(data_4));
                handle_error(err, "vee_write for RECORD_ID4 failed\r\n");
                printf("\r\nStructure data is written successfully on RECORD_ID4\r\n");
                break;
            }

            default:
            {
                printf("\r\nInvalid input");
                printf(WRITE_MENU);
                break;
            }
        }
    }

    printf("VEEPROM is in ready state\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs the read operation from VEEPROM.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void vee_read_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t out_len = RESET_VALUE;
    void * p_record_data = NULL;
    adxl_t adxl_data = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};
    char input_buff[BUFFER_SIZE] = {RESET_VALUE};
    uint8_t user_option = RESET_VALUE;

    /* Print menu for read operations */
    printf(READ_MENU);

    while (!uart_console_has_char())
    {
        /* Do nothing */
    }

    while ((WRITE_READ_ID_1 > user_option) || (WRITE_READ_ID_4 < user_option))
    {
        /* Get the user option */
        memset(&input_buff[0], NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int)sizeof(input_buff));
        user_option = (uint8_t)atoi(input_buff);

        switch (user_option)
        {
            case WRITE_READ_ID_1:
            {
                /* Read data from RECORD_ID1 */
                err = vee_read(&g_vee_ctrl, RECORD_ID_1, (uint8_t **)&p_record_data, &out_len);
                if (FSP_ERR_NOT_FOUND == err)
                {
                    printf("\r\nNo data present on the requested RECORD_ID or flash is formatted\r\n");
                }
                else if (FSP_SUCCESS == err)
                {
                    printf("\r\nThe data in RECORD_ID1:\r\n");
                    for (uint8_t cnt = RESET_VALUE; cnt< out_len; cnt++)
                    {
                        printf("data[%d] = 0x%x\r\n", cnt, *((uint8_t *)p_record_data));
                        p_record_data = ((uint8_t *)p_record_data + 1U);
                    }
                }
                else
                {
                    handle_error(err, "vee_read for RECORD_ID1 failed\r\n");
                }
                break;
            }

            case WRITE_READ_ID_2:
            {
                /* Read data from RECORD_ID2 */
                err = vee_read(&g_vee_ctrl, RECORD_ID_2, (uint8_t **)&p_record_data, &out_len);
                if (FSP_ERR_NOT_FOUND == err)
                {
                    printf("\r\nNo data present on the requested RECORD_ID or flash is formatted\r\n");
                }
                else if (FSP_SUCCESS == err)
                {
                    printf("\r\nThe data in RECORD_ID2:\r\n");
                    for (uint8_t cnt = RESET_VALUE; cnt< out_len; cnt++)
                    {
                        printf("data[%d] = %c\r\n", cnt, *((uint8_t *)p_record_data));
                        p_record_data = ((uint8_t *)p_record_data + 1U);
                    }
                }
                else
                {
                    handle_error(err, "vee_read for RECORD_ID2 failed\r\n");
                }
                break;
            }

            case WRITE_READ_ID_3:
            {
                /* Read data from RECORD_ID3 */
                err = vee_read(&g_vee_ctrl, RECORD_ID_3, (uint8_t **)&p_record_data, &out_len);
                if (FSP_ERR_NOT_FOUND == err)
                {
                    printf("\r\nNo data present on the requested RECORD_ID or flash is formatted\r\n");
                }
                else if (FSP_SUCCESS == err)
                {
                    printf("\r\nThe data in RECORD_ID3:\r\n");
                    for (uint8_t cnt = RESET_VALUE; cnt< DATA_SIZE; cnt++)
                    {
                        printf("data[%d] = %0.02f\r\n", cnt, *((float *) p_record_data));
                        p_record_data = ((float *)p_record_data + 1U);
                    }
                }
                else
                {
                    handle_error(err, "vee_read for RECORD_ID3 failed\r\n");
                }
                break;
            }

            case WRITE_READ_ID_4:
            {
                /* Read data from RECORD_ID4 */
                err = vee_read(&g_vee_ctrl, RECORD_ID_4, (uint8_t **)&p_record_data, &out_len);
                if (FSP_ERR_NOT_FOUND == err)
                {
                    printf("\r\nNo data present on the requested RECORD_ID or flash is formatted\r\n");
                }
                else if (FSP_SUCCESS == err)
                {
                    /* Copy record data to instance of the adxl_t data structure */
                    memcpy(&adxl_data, (adxl_t *)p_record_data, sizeof(adxl_t));
                    printf("\r\nThe data in RECORD_ID4:\r\n");
                    printf("X-axis  = %d\r\n", adxl_data.x_data);
                    printf("Y-axis  = %d\r\n", adxl_data.y_data);
                    printf("Z-axis  = %d\r\n", adxl_data.z_data);
                    printf("XY-axis = %d\r\n", adxl_data.xy_data);
                }
                else
                {
                    handle_error(err, "vee_read for RECORD_ID4 failed\r\n");
                }
                break;
            }

            default:
            {
                printf("\r\nInvalid input");
                printf(READ_MENU);
                break;
            }
        }
    }

    printf("\r\nReading previously written data from the requested RECORD_ID completed successfully\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function erases the entire data flash used by VEEPROM.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Erased successfully.
 * @retval    Any other error codes     Failed to erase.
 **********************************************************************************************************************/
static void vee_format_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    memset(&g_status, RESET_VALUE, sizeof(g_status));

    /* Start a manual format operation */
    err = vee_format();
    handle_error(err, "vee_format failed\r\n");

    /* Get the current status of the module */
    err = vee_status_get(&g_status);
    handle_error(err, "vee_status_get failed\r\n");

    /* Compare the last ID written with default ID */
    if (DEFAULT_ID != g_status.last_id)
    {
        handle_error(FSP_ERR_NOT_ERASED, "The last ID written is not match with default ID\r\n");
    }

    printf("\r\nFormat operation completed successfully\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function gets status of RM_VEE_FLASH module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
static void vee_get_status_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    memset(&g_status, RESET_VALUE, sizeof(g_status));

    /* Get the current status of the module */
    err = vee_status_get(&g_status);
    handle_error(err, "vee_status_get failed\r\n");

    printf("\r\nThe last ID written = %d", g_status.last_id);
    printf("\r\nRemaining space available in the segment = %d", g_status.space_available);
    printf("\r\nGet status operation completed successfully\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs refresh operation on RM_VEE_FLASH module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Refreshed successfully.
 * @retval    Any other error codes     Failed to refresh.
 **********************************************************************************************************************/
static void vee_refresh_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t status_time_out = INT32_MAX;
    memset(&g_status, RESET_VALUE, sizeof(g_status));

    /* Force refresh and wait to complete */
    err = vee_refresh();
    handle_error(err, "vee_refresh failed\r\n");

    do
    {
        /* Get the current status of the module */
        err = vee_status_get(&g_status);
        handle_error(err, "vee_status_get failed\r\n");
    } while ((RM_VEE_STATE_READY != g_status.state) && (RESET_VALUE < --status_time_out));

    if (RESET_VALUE == status_time_out)
    {
        handle_error(FSP_ERR_TIMEOUT, "vee_refresh_operation timed out\r\n");
    }

    printf("\r\nCurrent segment erase count = %d", g_status.segment_erase_count);
    printf("\r\nRefresh operation completed successfully\r\n");
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        vee_flash_deinit();
        __BKPT(0);
    }
}
