/***********************************************************************************************************************
 * File Name    : littlefs_ospi_b_example_app.c
 * Description  : Example application layer for the LittleFS OSPI_B EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: littlefs_ospi_b_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "littlefs_ospi_b_ep.h"
#include "littlefs_ospi_b_example_app.h"
#include "console/uart_console.h"

/* Private variables */
static char s_dir_name[] = DIR_NAME_ONE;
static char s_file_name[] = FILE_NAME_ONE;
static char s_write_data[WRITE_BUFFER_SIZE] = {RESET_VALUE};
static char s_read_data[READ_BUFFER_SIZE] = {RESET_VALUE};
static char * s_p_menu_type[MENU_MAX] =
{
    [MENU_MAIN]  = MAIN_MENU,
    [MENU_FLASH] = FLASH_MENU,
    [MENU_DIR]   = DIR_MENU,
    [MENU_FILE]  = FILE_MENU,
};
static menu_t s_menu = MENU_MAIN;

/* Private functions */
static void littlefs_ospi_b_operation(void);
static int32_t file_system_format(void);
static int32_t dir_create(void);
static int32_t dir_get_property(void);
static int32_t dir_delete(void);
static int32_t file_open(void);
static int32_t file_write(void);
static int32_t file_read(void);
static int32_t file_delete(void);
static void create_fixed_buffer(void);
static uint8_t get_user_input(void);
static uint8_t handle_user_request(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/***********************************************************************************************************************
 * Function Name: littlefs_ospi_b_example_app_run.
 * Description  : Single entry-point for the LittleFS OSPI_B example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void littlefs_ospi_b_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;
    int32_t lfs_err = LFS_ERR_OK;

    /* Get FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Turn OFF the error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_LOW);

    /* Initialize UART module first to print log to serial terminal */
    if (FSP_SUCCESS != uart_console_init())
    {
        /* Turn ON error LED to indicate uart_console_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_HIGH);

        return;     /* UART failed to open - cannot print anything */
    }

    /* Print EP banner information */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);

    /* Print EP information */
    printf(EP_INFO);

    /* Initialize the OSPI flash */
    err = ospi_b_init();
    handle_error(err, "ospi_b_init failed\r\n");
    printf("\r\nOSPI initialized successfully!");

    /* Initialize LittleFS for OSPI */
    lfs_err = littlefs_ospi_b_init();
    handle_error((fsp_err_t) lfs_err, "littlefs_ospi_b_init failed\r\n");
    printf("\r\nLittleFS OSPI initialized successfully!");

    while (true)
    {
        /* Print the EP Menu */
        printf("%s", s_p_menu_type[s_menu]);

        /* Perform file system operations as required by the user */
        littlefs_ospi_b_operation();
    }
}
/***********************************************************************************************************************
* End of function littlefs_ospi_b_example_app_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: littlefs_ospi_b_operation
 * Description  : This function performs file system operations based on the user request.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void littlefs_ospi_b_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t lfs_err = LFS_ERR_OK;
    uint8_t user_request = RESET_VALUE;

    /* Get the user request */
    user_request = handle_user_request();

    if (RESET_VALUE != user_request)
    {
        /* Perform operation base on the user request */
        switch (user_request)
        {
            case REQUEST_ERASE_ENTIRE_OSPI:
            {
                printf("\r\nErase the entire ospi flash");
                printf("\r\nThis operation will take several minutes. Please wait...");

                /* Erase the entire OSPI flash */
                err = erase_ospi_flash(SPI_FLASH_ERASE_SIZE_CHIP_ERASE);
                handle_error(err, "erase_ospi_flash for the entire OSPI flash failed\r\n");

                printf("\r\nErased entire OSPI flash successfully!");
                printf("\r\nPlease FORMAT THE FILE SYSTEM after erase the entire OSPI flash!");
                break;
            }

            case REQUEST_ERASE_32KB_OSPI:
            {
                printf("\r\nErase the first 32 KB of the OSPI flash. Please wait...");

                /* Erase the first 32 KB of the OSPI flash */
                err = erase_ospi_flash(OSPI_B_32K_SIZE);
                handle_error(err, "erase_ospi_flash for the first 32 KB of the OSPI flash failed\r\n");

                printf("\r\nErased the first 32 KB of the OSPI flash successfully!");
                printf("\r\nPlease FORMAT THE FILE SYSTEM after erase the first 32 KB of the OSPI flash!");
                break;
            }

            case REQUEST_FILE_SYSTEM_FORMAT:
            {
                /* Format the file system */
                lfs_err = file_system_format();
                handle_error((fsp_err_t)lfs_err, "file_system_format failed\r\n");
                break;
            }

            case REQUEST_DIR_CREATE:
            {
                /* Create a new directory */
                lfs_err = dir_create();
                handle_error((fsp_err_t)lfs_err, "dir_create failed\r\n");
                break;
            }

            case REQUEST_DIR_PROPERTY:
            {
                /* Get the directory properties */
                lfs_err = dir_get_property();
                handle_error((fsp_err_t)lfs_err, "dir_get_property failed\r\n");
                break;
            }

            case REQUEST_DIR_DELETE:
            {
                /* Delete the directory */
                lfs_err = dir_delete();
                handle_error((fsp_err_t)lfs_err, "dir_delete failed\r\n");
                break;
            }

            case REQUEST_FILE_OPEN:
            {
                /* Create a new file or open a existed file */
                lfs_err = file_open();
                handle_error((fsp_err_t)lfs_err, "file_open failed\r\n");
                break;
            }

            case REQUEST_FILE_WRITE:
            {
                /* Write to the file */
                lfs_err = file_write();
                handle_error((fsp_err_t)lfs_err, "file_write failed\r\n");
                break;
            }

            case REQUEST_FILE_READ:
            {
                /* Read from the file */
                lfs_err = file_read();
                handle_error((fsp_err_t)lfs_err, "file_read failed\r\n");
                break;
            }

            case REQUEST_FILE_DELETE:
            {
                /* Delete the file */
                lfs_err = file_delete();
                handle_error((fsp_err_t)lfs_err, "file_delete failed\r\n");
                break;
            }

            default:
            {
                printf("\r\nInvalid input. Please provide input from below option");
                break;
            }
        }
    }
}
/***********************************************************************************************************************
* End of function littlefs_ospi_b_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_system_format
 * Description  : This function formats the file system.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Formatted successfully.
 *                Any other error codes     Failed to format.
 **********************************************************************************************************************/
static int32_t file_system_format(void)
{
    int32_t lfs_err = LFS_ERR_OK;

    printf("\r\nFormat file system...");

    /* Format the file system */
    lfs_err = lfs_format(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
    if (LFS_ERR_OK != lfs_err)
    {
        return lfs_err;
    }

    /* Mount the file system */
    lfs_err = lfs_mount(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
    if (LFS_ERR_OK != lfs_err)
    {
        return lfs_err;
    }

    /* Format file system successfully */
    printf("\r\nFile system has been formatted!");

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_system_format
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_create
 * Description  : This function creates a new directory.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Created successfully.
 *                Any other error codes     Failed to create.
 **********************************************************************************************************************/
static int32_t dir_create(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct lfs_info dir_info;
    memset(&dir_info, RESET_VALUE, sizeof(dir_info));

    printf("\r\nCreate a new directory...");

    /* Create a new directory */
    lfs_err = lfs_mkdir(&g_rm_littlefs_lfs, s_dir_name);
    if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_EXIST != lfs_err))
    {
        return lfs_err;
    }
    if (LFS_ERR_EXIST == lfs_err)
    {
        printf("\r\nDirectory had already exited!");
        lfs_err = LFS_ERR_OK;
    }
    else
    {
        printf("\r\nDirectory created successfully!");
    }

    /* Check the directory */
    lfs_err = lfs_stat(&g_rm_littlefs_lfs, s_dir_name, &dir_info);
    if (LFS_ERR_OK != lfs_err)
    {
        return lfs_err;
    }
    if (LFS_TYPE_DIR == dir_info.type)
    {
        printf("\r\nName of directory: %s", dir_info.name);
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function dir_create
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_get_property
 * Description  : This function gets the properties of a directory.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Got successfully.
 *                Any other error codes     Failed to get.
 **********************************************************************************************************************/
static int32_t dir_get_property(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct lfs_info dir_info;
    memset(&dir_info, RESET_VALUE, sizeof(dir_info));

    printf("\r\nGet directory properties...");

    /* Check the directory */
    lfs_err = lfs_stat(&g_rm_littlefs_lfs, s_dir_name, &dir_info);
    if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err))
    {
        return lfs_err;
    }
    if (LFS_ERR_NOENT == lfs_err)
    {
        printf("\r\nDirectory does not exist!");
        lfs_err = LFS_ERR_OK;
    }
    else
    {
        /* Get the directory properties */
        if (LFS_TYPE_DIR == dir_info.type)
        {
            printf("\r\nName of directory: %s", dir_info.name);
            printf("\r\nType of directory: %s", TYPE_DIR);
            printf("\r\nGot directory properties successfully!");
        }
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function dir_get_property
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: dir_delete
 * Description  : This function deletes a directory.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Deleted successfully.
 *                Any other error codes     Failed to delete.
 **********************************************************************************************************************/
static int32_t dir_delete(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct lfs_info dir_info;
    memset(&dir_info, RESET_VALUE, sizeof(dir_info));

    printf("\r\nDelete a directory...");

    /* Delete a directory */
    lfs_err = lfs_remove(&g_rm_littlefs_lfs, s_dir_name);
    if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err) && (LFS_ERR_NOTEMPTY != lfs_err))
    {
        return lfs_err;
    }

    if (LFS_ERR_NOENT == lfs_err)
    {
        printf("\r\nDirectory does not exist!");
        lfs_err = LFS_ERR_OK;
    }
    else if (LFS_ERR_NOTEMPTY == lfs_err)
    {
        printf("\r\nDirectory is not empty, can not delete!!!");
        return lfs_err;
    }
    else
    {
        /* Check the directory after deleting */
        lfs_err = lfs_stat(&g_rm_littlefs_lfs, s_dir_name, &dir_info);
        if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err))
        {
            return lfs_err;
        }
        if (LFS_ERR_NOENT == lfs_err)
        {
            printf("\r\nDirectory deleted successfully!");
            lfs_err = LFS_ERR_OK;
        }
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function dir_delete
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_open
 * Description  : This function creates a new file or open an existed file.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Created and opened successfully.
 *                Any other error codes     Failed to create or open.
 **********************************************************************************************************************/
static int32_t file_open(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct lfs_info file_info;
    memset(&file_info, RESET_VALUE, sizeof(file_info));
    lfs_file_t file;

#ifdef LFS_NO_MALLOC
    struct lfs_file_config file_cfg =
    { .buffer = g_rm_littlefs_lfs_cfg.prog_buffer, };
#endif /* LFS_NO_MALLOC */

    printf("\r\nCreate a new file...");

    /* Check if the file exists and open with read and write flag */
#ifndef LFS_NO_MALLOC
    lfs_err = lfs_file_open(&g_rm_littlefs_lfs, &file, s_file_name, LFS_O_RDWR);
#else
    lfs_err = lfs_file_opencfg(&g_rm_littlefs_lfs, &file, s_file_name, LFS_O_RDWR, &file_cfg);
#endif /* LFS_NO_MALLOC */
    if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err))
    {
        return lfs_err;
    }
    if (LFS_ERR_NOENT == lfs_err)
    {
        /* The file does not exists, create a new file with read and write flag */
#ifndef LFS_NO_MALLOC
        lfs_err = lfs_file_open(&g_rm_littlefs_lfs, &file, s_file_name, LFS_O_CREAT | LFS_O_RDWR);
#else
        lfs_err = lfs_file_opencfg(&g_rm_littlefs_lfs, &file, s_file_name, LFS_O_CREAT | LFS_O_RDWR, &file_cfg);
#endif /* LFS_NO_MALLOC */
        if (LFS_ERR_OK != lfs_err)
        {
            return lfs_err;
        }
        printf("\r\nFile was created and opened successfully!");
    }
    else
    {
        printf("\r\nFile already existed and opened successfully!");
    }

    /* Check the file */
    lfs_err = lfs_stat(&g_rm_littlefs_lfs, s_file_name, &file_info);
    if (LFS_ERR_OK != lfs_err)
    {
        return lfs_err;
    }
    if (LFS_TYPE_REG == file_info.type)
    {
        printf("\r\nName of the file: %s", file_info.name);
    }

    /* Close the file */
    lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
    if (LFS_ERR_OK != lfs_err)
    {
        return lfs_err;
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_open
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_write
 * Description  : This function writes fixed data to a file.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Wrote successfully.
 *                Any other error codes     Failed to write.
 **********************************************************************************************************************/
static int32_t file_write(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    lfs_file_t file;
    lfs_ssize_t bytes_written = RESET_VALUE;

#ifdef LFS_NO_MALLOC
    struct lfs_file_config file_cfg =
    { .buffer = g_rm_littlefs_lfs_cfg.prog_buffer, };
#endif /* LFS_NO_MALLOC */

    printf("\r\nWrite to a file...");

    /* Open the existed file as write and clean content of file */
#ifndef LFS_NO_MALLOC
    lfs_err = lfs_file_open(&g_rm_littlefs_lfs, &file, s_file_name, LFS_O_WRONLY | LFS_O_TRUNC);
#else
    lfs_err = lfs_file_opencfg(&g_rm_littlefs_lfs, &file, s_file_name, LFS_O_WRONLY | LFS_O_TRUNC, &file_cfg);
#endif /* LFS_NO_MALLOC */
    if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err))
    {
        return lfs_err;
    }
    if (LFS_ERR_NOENT == lfs_err)
    {
        printf("\r\nFile does not exist!");
        return LFS_ERR_OK;
    }

    /* Create fixed buffer */
    create_fixed_buffer();

    /* Write fixed buffer to the opened file */
    bytes_written = lfs_file_write(&g_rm_littlefs_lfs, &file, (void *)s_write_data, WRITE_BUFFER_SIZE);
    if (0 > bytes_written)
    {
        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        if (LFS_ERR_OK != lfs_err)
        {
            return lfs_err;
        }

        return (int32_t)bytes_written;
    }

    /* Close the file */
    lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
    if (LFS_ERR_OK != lfs_err)
    {
        return lfs_err;
    }

    /* Write to the file successfully */
    printf("\r\nNumber of bytes were written: %d Byte", (int)bytes_written);
    printf("\r\nWrote to the file successfully!");

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_read
 * Description  : This function moves the file position pointer to the beginning of the file and
 *                reads data from a file.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Read successfully.
 *                Any other error codes     Failed to read.
 **********************************************************************************************************************/
static int32_t file_read(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    lfs_file_t file;
    char print_buffer[PRINT_BUFFER_SIZE + ONE_BYTE] = {RESET_VALUE};

    /* Reset read data buffer */
    memset(s_read_data, RESET_VALUE, READ_BUFFER_SIZE);

    lfs_ssize_t bytes_read = RESET_VALUE;
    lfs_size_t  size_read  = READ_BUFFER_SIZE;
#ifdef LFS_NO_MALLOC
    struct lfs_file_config file_cfg =
    { .buffer = g_rm_littlefs_lfs_cfg.prog_buffer, };
#endif /* LFS_NO_MALLOC */

    printf("\r\nRead from a file...");

    /* Open the existed file as read */
#ifndef LFS_NO_MALLOC
    lfs_err = lfs_file_open(&g_rm_littlefs_lfs, &file, s_file_name, LFS_O_RDONLY);
#else
    lfs_err = lfs_file_opencfg(&g_rm_littlefs_lfs, &file, s_file_name, LFS_O_RDONLY, &file_cfg);
#endif /* LFS_NO_MALLOC */
    if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err))
    {
        return lfs_err;
    }
    if (LFS_ERR_NOENT == lfs_err)
    {
        printf("\r\nFile does not exist!");
        return LFS_ERR_OK;
    }

    /* Move the file position pointer to the beginning of the file */
    lfs_soff_t seek_result = lfs_file_seek(&g_rm_littlefs_lfs, &file, SEEK_VALUE, LFS_SEEK_SET);
    if (0 > seek_result)
    {
        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        if (LFS_ERR_OK != lfs_err)
        {
            return lfs_err;
        }

        return (int32_t)seek_result;
    }

    /* Read from the file */
    bytes_read = lfs_file_read(&g_rm_littlefs_lfs, &file, s_read_data, size_read);
    if (0 > bytes_read)
    {
        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        if (LFS_ERR_OK != lfs_err)
        {
            return lfs_err;
        }

        return (int32_t)bytes_read;
    }
    else if (0 == bytes_read)
    {
        printf("\r\nFile was empty or reached the end of line!");

        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        if (LFS_ERR_OK != lfs_err)
        {
            return lfs_err;
        }

        return LFS_ERR_OK;
    }
    else
    {
        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        if (LFS_ERR_OK != lfs_err)
        {
            return lfs_err;
        }

        /* Read from the file successfully */
        printf("\r\nNumber of bytes were read: %d Byte", (int) bytes_read);
        int32_t compare_result = memcmp(s_write_data, s_read_data, WRITE_BUFFER_SIZE);
        if (0 == compare_result)
        {
            printf("\r\nThe data matches: The Write and Read buffers are identical!");
        }
        else
        {
            printf("\r\nThe data mismatches: The Write and Read buffers are different!!!");
        }

        /* Get and print the first 1 KB of the file content */
        memcpy(print_buffer, s_read_data, PRINT_BUFFER_SIZE);
        printf("\r\nThe content of the first 1 KB of the file:\r\n%s", print_buffer);

        printf("\r\nRead from the file successfully!");
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: file_delete
 * Description  : This function deletes a file.
 * Arguments    : None.
 * Return Value : LFS_ERR_OK                Deleted successfully.
 *                Any other error codes     Failed to delete.
 **********************************************************************************************************************/
static int32_t file_delete(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct lfs_info file_info;

    memset(&file_info, RESET_VALUE, sizeof(file_info));

    printf("\r\nDelete a file...");

    /* Delete the file */
    lfs_err = lfs_remove(&g_rm_littlefs_lfs, s_file_name);
    if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err))
    {
        return lfs_err;
    }
    if (LFS_ERR_NOENT == lfs_err)
    {
        printf("\r\nFile does not exist!");
        lfs_err = LFS_ERR_OK;
    }
    else
    {
        /* Check the file after deleting */
        lfs_err = lfs_stat(&g_rm_littlefs_lfs, s_file_name, &file_info);
        if ((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err))
        {
            return lfs_err;
        }
        if (LFS_ERR_NOENT == lfs_err)
        {
            printf("\r\nFile deleted successfully!");
            lfs_err = LFS_ERR_OK;
        }
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_delete
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: create_fixed_buffer
 * Description  : This function creates a fixed data buffer.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
static void create_fixed_buffer(void)
{
    char * p_data = s_write_data;

    /* Clean write buffer */
    memset(p_data, '\0', WRITE_BUFFER_SIZE);

    /* Create fixed buffer */
    for (uint16_t i = 0; i < WRITE_BUFFER_SIZE / WRITE_LINE_SIZE; i++)
    {
        strncpy(p_data, WRITE_LINE_TEXT, WRITE_LINE_SIZE);
        p_data += WRITE_LINE_SIZE;
    }
}
/***********************************************************************************************************************
* End of function create_fixed_buffer
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: get_user_input
 * Description  : This function gets user input from the terminal application.
 * Arguments    : None.
 * Return value : input_value               Return input value after receive enter key.
 **********************************************************************************************************************/
static uint8_t get_user_input(void)
{
    uint8_t input_value = RESET_VALUE;
    char input_buf[USER_INPUT_BUFFER_SIZE] = {RESET_VALUE};

    /* Read a line of user input from the terminal */
    user_input(input_buf, (int)sizeof(input_buf));

    /* Convert to integer value */
    input_value = (uint8_t)atoi(input_buf);

    return input_value;
}
/***********************************************************************************************************************
* End of function get_user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_user_request
 * Description  : This function gets user request from the terminal application.
 * Arguments    : None.
 * Return value : user_request              Return user request after receive enter key.
 **********************************************************************************************************************/
static uint8_t handle_user_request(void)
{
    uint8_t user_input = RESET_VALUE;
    uint8_t user_request = RESET_VALUE;

    /* Get user input */
    user_input = get_user_input();

    /* Handle to get user request */
    switch (s_menu)
    {
        case MENU_MAIN:
        {
            switch (user_input)
            {
                case SELECT_FLASH_OPERATION:
                {
                    s_menu = MENU_FLASH;
                    break;
                }

                case SELECT_DIR_OPERATION:
                {
                    s_menu = MENU_DIR;
                    break;
                }

                case SELECT_FILE_OPERATION:
                {
                    s_menu = MENU_FILE;
                    break;
                }

                default:
                {
                    printf("\r\nInvalid input. Please provide input from below option");
                    break;
                }
            }
            break;
        }

        case MENU_FLASH:
        {
            switch (user_input)
            {
                case SELECT_ERASE_ENTIRE_FLASH:
                {
                    user_request = REQUEST_ERASE_ENTIRE_OSPI;
                    break;
                }

                case SELECT_ERASE_32KB_FLASH:
                {
                    user_request = REQUEST_ERASE_32KB_OSPI;
                    break;
                }

                case SELECT_FORMAT_FILE_SYSTEM:
                {
                    user_request = REQUEST_FILE_SYSTEM_FORMAT;
                    break;
                }

                case SELECT_FLASH_GO_BACK:
                {
                    s_menu = MENU_MAIN;
                    break;
                }

                default:
                {
                    printf("\r\nInvalid input. Please provide input from below option");
                    break;
                }
            }
            break;
        }

        case MENU_DIR:
        {
            switch (user_input)
            {
                case SELECT_CREATE_DIR:
                {
                    user_request = REQUEST_DIR_CREATE;
                    break;
                }

                case SELECT_GET_DIR_PROPERTY:
                {
                    user_request = REQUEST_DIR_PROPERTY;
                    break;
                }

                case SELECT_DELETE_DIR:
                {
                    user_request = REQUEST_DIR_DELETE;
                    break;
                }

                case SELECT_DIR_GO_BACK:
                {
                    s_menu = MENU_MAIN;
                    break;
                }

                default:
                {
                    printf("\r\nInvalid input. Please provide input from below option");
                    break;
                }
            }
            break;
        }

        case MENU_FILE:
        {
            switch (user_input)
            {
                case SELECT_OPEN_FILE:
                {
                    user_request = REQUEST_FILE_OPEN;
                    break;
                }

                case SELECT_WRITE_FILE:
                {
                    user_request = REQUEST_FILE_WRITE;
                    break;
                }

                case SELECT_READ_FILE:
                {
                    user_request = REQUEST_FILE_READ;
                    break;
                }

                case SELECT_DELETE_FILE:
                {
                    user_request = REQUEST_FILE_DELETE;
                    break;
                }

                case SELECT_FILE_GO_BACK:
                {
                    s_menu = MENU_MAIN;
                    break;
                }

                default:
                {
                    printf("\r\nInvalid input. Please provide input from below option");
                    break;
                }
            }
            break;
        }

        default:
        {
            printf("\r\nInvalid input. Please provide input from below option");
            break;
        }
    }

    return user_request;
}
/***********************************************************************************************************************
* End of function handle_user_request
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: user_input.
 * Description  : This function reads a line of user input from stdin and removes trailing newline characters.
 * Arguments    : buf                       Pointer to buffer where the input string is stored.
 *                len                       Maximum number of characters to read (buffer size).
 * Return Value : None.
 **********************************************************************************************************************/
static void user_input(char *buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}
/***********************************************************************************************************************
* End of function user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err                       Error code.
 *                err_str                   Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        littlefs_ospi_b_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
