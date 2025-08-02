/***********************************************************************************************************************
 * File Name    : littlefs_qspi_ep.c
 * Description  : Contains macros data structures and functions used in littlefs_qspi_ep.c and hal_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "littlefs_qspi_ep.h"

#if (USE_VIRTUAL_COM == 1)
extern bsp_leds_t g_bsp_leds;
#endif /* USE_VIRTUAL_COM */

/***********************************************************************************************************************
 * Private global variable
 **********************************************************************************************************************/
static char g_dir_name[]    = DIR_NAME_ONE;
static char g_file_name[]   = FILE_NAME_ONE;
static char g_write_data[WRITE_BUFFER_SIZE] = {RESET_VALUE};
static char g_read_data[READ_BUFFER_SIZE]   = {RESET_VALUE};
static char * gp_menu_type[MENU_MAX] =
{
    [MENU_MAIN]     = MAIN_MENU,
    [MENU_FLASH]    = FLASH_MENU,
    [MENU_DIR]      = DIR_MENU,
    [MENU_FILE]     = FILE_MENU,
};
static menu_t g_menu = MENU_MAIN;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static int32_t littlefs_qspi_init (void);
static void littlefs_qspi_deinit (void);
static uint8_t get_user_input (void);
static uint8_t handle_user_request (void);
static int32_t littlefs_qspi_operation (void);
static int32_t file_system_format (void);
static int32_t dir_create (void);
static int32_t dir_get_property (void);
static int32_t dir_delete (void);
static int32_t file_open (void);
static int32_t file_write (void);
static int32_t file_read (void);
static int32_t file_delete (void);
static void create_fixed_buffer (void);

/***********************************************************************************************************************
 *  Function Name: littlefs_qspi_entry
 *  Description  : This function starts the LittleFS for QSPI flash operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void littlefs_qspi_entry(void)
{
    fsp_pack_version_t version = { RESET_VALUE };

    fsp_err_t err = FSP_SUCCESS;

    int32_t lfs_err = LFS_ERR_OK;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

#if (USE_VIRTUAL_COM == 1)

    /* Turn OFF error LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_LOW);

    /* Initialize UART module first to print log to serial terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn ON error LED to indicate uart_init failed */
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[ERR_LED], (bsp_io_level_t)BSP_IO_LEVEL_HIGH);

        /* Error trap */
        ERROR_TRAP;
    }
#endif /* USE_VIRTUAL_COM */

    /* Print EP banner information */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);

    /* Print EP information */
    APP_PRINT(EP_INFO);

    /* Initialize the QSPI Flash */
    err = qspi_init();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nqspi_init failed!!!\r\n");
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nQSPI initialized successfully!\r\n");

    /* Initialize LittleFS for QSPI */
    lfs_err = littlefs_qspi_init();
    if (LFS_ERR_OK != lfs_err)
    {
        APP_ERR_PRINT("\r\nlittlefs_qspi_init failed!!!\r\n");
        APP_ERR_TRAP(lfs_err);
    }
    APP_PRINT("\r\nLittleFS QSPI initialized successfully!\r\n");

    while (true)
    {
        /* Print the EP Menu */
        APP_PRINT(gp_menu_type[g_menu]);

        /* Perform file system operations as required by the user */
        lfs_err = littlefs_qspi_operation();
        if (LFS_ERR_OK != lfs_err)
        {
            APP_ERR_PRINT("littlefs_qspi_operation failed!!!\r\n");
            littlefs_qspi_deinit();
            APP_ERR_TRAP(lfs_err);
        }
    }
}
/***********************************************************************************************************************
* End of function littlefs_qspi_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: littlefs_qspi_operation
 *  Description  : This function performs file system operations based on user requests.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t littlefs_qspi_operation(void)
{
    fsp_err_t   err     = FSP_SUCCESS;

    int32_t     lfs_err = LFS_ERR_OK;

    uint8_t user_request = RESET_VALUE;

    /* Get the user request */
    user_request = handle_user_request();

    if (RESET_VALUE != user_request)
    {
        /* Perform operation base on the user request */
        switch (user_request)
        {
            case REQUEST_ERASE_ENTIRE_QSPI:
            {
                /* Erase the entire qspi flash */
                err = erase_qspi_flash(SPI_FLASH_ERASE_SIZE_CHIP_ERASE);
                lfs_err = (int32_t) err;
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "erase_qspi_flash failed!!!\r\n");
                break;
            }
            case REQUEST_ERASE_32KB_QSPI:
            {
                /* Erase the first 32 KB of the qspi flash */
                err = erase_qspi_flash(QSPI_32KB_BLOCK_SIZE);
                lfs_err = (int32_t) err;
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "erase_qspi_flash failed!!!\r\n");
                break;
            }
            case REQUEST_FILE_SYSTEM_FORMAT:
            {
                /* Format the file system */
                lfs_err = file_system_format();
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "file_system_format failed!!!\r\n");
                break;
            }
            case REQUEST_DIR_CREATE:
            {
                /* Create a new directory */
                lfs_err = dir_create();
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "dir_create failed!!!\r\n");
                break;
            }
            case REQUEST_DIR_PROPERTY:
            {
                /* Get the directory properties */
                lfs_err = dir_get_property();
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "dir_get_property failed!!!\r\n");
                break;
            }
            case REQUEST_DIR_DELETE:
            {
                /* Delete the directory */
                lfs_err = dir_delete();
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "dir_delete failed!!!\r\n");
                break;
            }

            case REQUEST_FILE_OPEN:
            {
                /* Create a new file or open a existed file */
                lfs_err = file_open();
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "file_open failed!!!\r\n");
                break;
            }
            case REQUEST_FILE_WRITE:
            {
                /* Write to the file */
                lfs_err = file_write();
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "file_write failed!!!\r\n");
                break;
            }
            case REQUEST_FILE_READ:
            {
                /* Read from the file */
                lfs_err = file_read();
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "file_read failed!!!\r\n");
                break;
            }
            case REQUEST_FILE_DELETE:
            {
                /* Delete the file */
                lfs_err = file_delete();
                APP_ERR_RET(lfs_err != LFS_ERR_OK, lfs_err, "file_delete failed!!!\r\n");
                break;
            }
            default:
            {
                APP_PRINT("\r\n\r\nInvalid Input!!!");
                APP_PRINT("\r\nPlease provide input from below option\r\n");
                break;
            }
        }
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function littlefs_qspi_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: littlefs_qspi_init
 *  Description  : This function initializes LittleFS for QSPI.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t littlefs_qspi_init(void)
{
    fsp_err_t   err     = FSP_SUCCESS;

    int32_t     lfs_err = LFS_ERR_OK;

    /* Open LittleFS SPI Flash */
    err = RM_LITTLEFS_SPI_FLASH_Open(&g_rm_littlefs_ctrl, &g_rm_littlefs_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n**RM_LITTLEFS_FLASH_Open API FAILED!!!**\r\n");
        lfs_err = (int32_t) err;
        return lfs_err;
    }
    else
    {
        lfs_err = (int32_t) err;
    }

    /* Mount the file system */
    lfs_err = lfs_mount(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
    if (LFS_ERR_OK != lfs_err)
    {
        APP_PRINT("\r\nFile system was not formatted or is corrupt, formatting now...\r\n");

        /* Format the file system */
        lfs_err = lfs_format(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
        if(LFS_ERR_OK != lfs_err)
        {
            APP_ERR_PRINT("\r\n**lfs_format API FAILED!!!**\r\n");
            littlefs_qspi_deinit();
            return lfs_err;
        }

        /* Mount the file system again */
        lfs_err = lfs_mount(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
        if(LFS_ERR_OK != lfs_err)
        {
            APP_ERR_PRINT("\r\n**lfs_mount API FAILED after formatting!!!**\r\n");
            littlefs_qspi_deinit();
            return lfs_err;
        }
        APP_PRINT("\r\nMonunted file system successfully!\r\n");
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function littlefs_qspi_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: littlefs_qspi_deinit
 *  Description  : This function de-initializes LittleFS for QSPI flash.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void littlefs_qspi_deinit(void)
{
    fsp_err_t   err     = FSP_SUCCESS;

    int32_t     lfs_err = LFS_ERR_OK;

    /* Unmount the file system */
    lfs_err = lfs_unmount(&g_rm_littlefs_lfs);
    if(LFS_ERR_OK != lfs_err)
    {
        APP_ERR_PRINT("\r\n**lfs_unmount API FAILED!!!**\r\n");
    }

    /* Close the lower level driver */
    err = RM_LITTLEFS_SPI_FLASH_Close(&g_rm_littlefs_ctrl);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n**RM_LITTLEFS_SPI_FLASH_Close API FAILED!!!**\r\n");
    }
}
/***********************************************************************************************************************
* End of function littlefs_qspi_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: file_system_format
 *  Description  : This function formats the file system.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t file_system_format(void)
{
    int32_t lfs_err = LFS_ERR_OK;

    APP_PRINT ("\r\n\r\nFormatting file system...\r\n");

    /* Format the file system */
    lfs_err = lfs_format(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
    if(LFS_ERR_OK != lfs_err)
    {
        APP_ERR_PRINT("\r\n**lfs_format API FAILED!!!**\r\n");
        littlefs_qspi_deinit();
        return lfs_err;
    }

    /* Mount the file system */
    lfs_err = lfs_mount(&g_rm_littlefs_lfs, &g_rm_littlefs_lfs_cfg);
    if(LFS_ERR_OK != lfs_err)
    {
        APP_ERR_PRINT("\r\n**lfs_mount API FAILED!!!**\r\n");
        littlefs_qspi_deinit();
        return lfs_err;
    }

    /* Format file system successfully */
    APP_PRINT ("\r\nFile system has been formatted!\r\n");

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_system_format
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: dir_create
 *  Description  : This function creates a new directory.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t dir_create(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct  lfs_info dir_info = {RESET_VALUE};

    APP_PRINT("\r\n\r\nCreate a new directory...\r\n");

    /* Create a new directory */
    lfs_err = lfs_mkdir(&g_rm_littlefs_lfs, g_dir_name);
    APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_EXIST != lfs_err), lfs_err, "\r\n**lfs_mkdir API FAILED!!!**\r\n");
    if (LFS_ERR_EXIST == lfs_err)
    {
        APP_PRINT("\r\nDirectory had already exited!\r\n");
        lfs_err = LFS_ERR_OK;
    }
    else
    {
        APP_PRINT("\r\nDirectory created successfully!\r\n");
    }

    /* Check the directory */
    lfs_err = lfs_stat(&g_rm_littlefs_lfs, g_dir_name, &dir_info);
    APP_ERR_RET(LFS_ERR_OK != lfs_err, lfs_err, "\r\n**lfs_stat API FAILED!!!**\r\n");
    if (LFS_TYPE_DIR == dir_info.type)
    {
        APP_PRINT("\r\nName of directory: %s\r\n", dir_info.name);
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function dir_create
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: dir_get_property
 *  Description  : This function gets the properties of a directory.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t dir_get_property(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct  lfs_info dir_info = {RESET_VALUE};

    APP_PRINT("\r\n\r\nGet directory properties...\r\n");

    /* Check the directory */
    lfs_err = lfs_stat(&g_rm_littlefs_lfs, g_dir_name, &dir_info);
    APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err), lfs_err, "\r\n**lfs_stat API FAILED!!!**\r\n");
    if (LFS_ERR_NOENT == lfs_err)
    {
        APP_PRINT("\r\nDirectory does not exist! \r\n");
        lfs_err = LFS_ERR_OK;
    }
    else
    {
        /* Get the directory properties */
        if (LFS_TYPE_DIR == dir_info.type)
        {
            APP_PRINT("\r\nName of directory: %s\r\n", dir_info.name);
            APP_PRINT("\r\nType of directory: %s\r\n", TYPE_DIR);
            APP_PRINT("\r\nGot directory properties successfully!\r\n");
        }
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function dir_get_property
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: dir_delete
 *  Description  : This function deletes a directory.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t dir_delete(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct  lfs_info dir_info = {RESET_VALUE};

    APP_PRINT("\r\n\r\nDelete a directory...\r\n");

    /* Delete a directory */
    lfs_err = lfs_remove(&g_rm_littlefs_lfs, g_dir_name);
    APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err) &&
            (LFS_ERR_NOTEMPTY == lfs_err), lfs_err, "\r\n**lfs_remove API FAILED!!!**\r\n");

    if (LFS_ERR_NOENT == lfs_err)
    {
        APP_PRINT("\r\nDirectory does not exist!\r\n");
        lfs_err = LFS_ERR_OK;
    }
    else if (LFS_ERR_NOTEMPTY == lfs_err)
    {
        APP_ERR_PRINT("Directory is not empty, can not delete!!!\r\n");
        return lfs_err;
    }
    else
    {
        /* Check the directory after deleting */
        lfs_err = lfs_stat(&g_rm_littlefs_lfs, g_dir_name, &dir_info);
        APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err), lfs_err, "\r\n**lfs_stat API FAILED!!!**\r\n");
        if (LFS_ERR_NOENT == lfs_err)
        {
            APP_PRINT("\r\nDirectory deleted successfully!\r\n");
            lfs_err = LFS_ERR_OK;
        }
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function dir_delete
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: file_open
 *  Description  : This function creates a new file or open an existed file.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t file_open(void)
{
    int32_t lfs_err = LFS_ERR_OK;
    struct  lfs_info file_info = {RESET_VALUE};

    lfs_file_t  file;
#ifdef LFS_NO_MALLOC
    struct lfs_file_config file_cfg =
    { .buffer = g_rm_littlefs_lfs_cfg.prog_buffer, };
#endif

    APP_PRINT("\r\n\r\nCreate a new file...\r\n");

    /* Check if the file exists and open with read and write flag */
#ifndef LFS_NO_MALLOC
    lfs_err = lfs_file_open(&g_rm_littlefs_lfs, &file, g_file_name, LFS_O_RDWR);
#else
    lfs_err = lfs_file_opencfg(&g_rm_littlefs_lfs, &file, g_file_name, LFS_O_RDWR, &file_cfg);
#endif
    APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err), lfs_err, "**lfs_file_open API FAILED!!!**\r\n");
    if (LFS_ERR_NOENT == lfs_err)
    {
        /* The file does not exists, create a new file with read and write flag */
#ifndef LFS_NO_MALLOC
        lfs_err = lfs_file_open(&g_rm_littlefs_lfs, &file, g_file_name, LFS_O_CREAT | LFS_O_RDWR);
#else
        lfs_err = lfs_file_opencfg(&g_rm_littlefs_lfs, &file, g_file_name, LFS_O_CREAT | LFS_O_RDWR, &file_cfg);
#endif
        APP_ERR_RET((LFS_ERR_OK != lfs_err), lfs_err, "**lfs_file_open API FAILED!!!**\r\n");
        APP_PRINT("\r\nFile was created and opened successfully!\r\n");
    }
    else
    {
        APP_PRINT("\r\nFile already existed and opened successfully!\r\n");
    }

    /* Check the file */
    lfs_err = lfs_stat(&g_rm_littlefs_lfs, g_file_name, &file_info);
    APP_ERR_RET((LFS_ERR_OK != lfs_err), lfs_err, "\r\n**lfs_stat API FAILED!!!**\r\n");
    if (LFS_TYPE_REG == file_info.type)
    {
        APP_PRINT("\r\nName of the file: %s\r\n", file_info.name);
    }

    /* Close the file */
    lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
    APP_ERR_RET((LFS_ERR_OK != lfs_err), lfs_err, "**lfs_file_close API FAILED!!!**\r\n");

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_open
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: file_write
 *  Description  : This function writes fixed data to a file.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t file_write(void)
{
    int32_t lfs_err = LFS_ERR_OK;

    lfs_file_t file;

    lfs_ssize_t bytes_written = RESET_VALUE;
#ifdef LFS_NO_MALLOC
    struct lfs_file_config file_cfg =
    { .buffer = g_rm_littlefs_lfs_cfg.prog_buffer, };
#endif

    APP_PRINT("\r\n\r\nWrite to a file...\r\n");

    /* Open the existed file as write and clean content of file */
#ifndef LFS_NO_MALLOC
    lfs_err = lfs_file_open(&g_rm_littlefs_lfs, &file, g_file_name, LFS_O_WRONLY | LFS_O_TRUNC);
#else
    lfs_err = lfs_file_opencfg(&g_rm_littlefs_lfs, &file, g_file_name, LFS_O_WRONLY | LFS_O_TRUNC, &file_cfg);
#endif
    APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err), lfs_err, "**lfs_file_open API FAILED!!!**\r\n");
    if (LFS_ERR_NOENT == lfs_err)
    {
        APP_PRINT("\r\nFile does not exist! \r\n");
        lfs_err = LFS_ERR_OK;
        return lfs_err;
    }

    /* Create fixed buffer */
    create_fixed_buffer();

    /* Write fixed buffer to the opened file */
    bytes_written = lfs_file_write(&g_rm_littlefs_lfs, &file, (void *)g_write_data, WRITE_BUFFER_SIZE);
    if (RESET_VALUE > bytes_written)
    {
        APP_ERR_PRINT("**lfs_file_write API FAILED!!!**\r\n");

        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        APP_ERR_RET((LFS_ERR_OK != lfs_err), lfs_err, "**lfs_file_close API FAILED!!!**\r\n");

        return bytes_written;
    }

    /* Close the file */
    lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
    APP_ERR_RET((LFS_ERR_OK != lfs_err), lfs_err, "**lfs_file_close API FAILED!!!**\r\n");

    /* Write to the file successfully */
    APP_PRINT("\r\nNumber of bytes were written: %d Byte\r\n", bytes_written);
    APP_PRINT("\r\nWrote to the file successfully!\r\n");

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_write
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: file_read
 *  Description  : This function moves the file position pointer to the beginning of the file and
 *                 reads data from a file.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t file_read(void)
{
    int32_t lfs_err = LFS_ERR_OK;

    lfs_file_t file;

    char print_buffer[PRINT_BUFFER_SIZE + ONE_BYTE] = {RESET_VALUE};

    /* Reset read data buffer */
    memset(g_read_data, RESET_VALUE, READ_BUFFER_SIZE);

    lfs_ssize_t bytes_read  = RESET_VALUE;
    lfs_size_t  size_read   = READ_BUFFER_SIZE;
    lfs_soff_t  pos         = RESET_VALUE;
#ifdef LFS_NO_MALLOC
    struct lfs_file_config file_cfg =
    { .buffer = g_rm_littlefs_lfs_cfg.prog_buffer, };
#endif

    APP_PRINT("\r\n\r\nRead from a file ...\r\n");

    /* Open the existed file as read */
#ifndef LFS_NO_MALLOC
    lfs_err = lfs_file_open(&g_rm_littlefs_lfs, &file, g_file_name, LFS_O_RDONLY);
#else
    lfs_err = lfs_file_opencfg(&g_rm_littlefs_lfs, &file, g_file_name, LFS_O_RDONLY, &file_cfg);
#endif
    APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err), lfs_err, "**lfs_file_open API FAILED!!!**\r\n");
    if (LFS_ERR_NOENT == lfs_err)
    {
        APP_PRINT("\r\nFile does not exist! \r\n");
        lfs_err = LFS_ERR_OK;
        return lfs_err;
    }

    /* Move the file position pointer to the beginning of the file */
    pos = lfs_file_seek(&g_rm_littlefs_lfs, &file, SEEK_VALUE, LFS_SEEK_SET);
    APP_ERR_RET((RESET_VALUE > pos), pos, "**lfs_file_seek API FAILED!!!**\r\n");

    /* Read from the file */
    bytes_read = lfs_file_read(&g_rm_littlefs_lfs, &file, g_read_data, size_read);
    if (RESET_VALUE > bytes_read)
    {
        APP_ERR_PRINT("**lfs_file_read API FAILED!!!**\r\n");

        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        APP_ERR_RET((LFS_ERR_OK != lfs_err), lfs_err, "**lfs_file_close API FAILED!!!**\r\n");

        return bytes_read;
    }
    else if (RESET_VALUE == bytes_read)
    {
        APP_PRINT("\r\nFile was empty or reached the end of line!\r\n");

        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        APP_ERR_RET((LFS_ERR_OK != lfs_err), lfs_err, "**lfs_file_close API FAILED!!!**\r\n");

        return bytes_read;
    }
    else
    {
        /* Close the file */
        lfs_err = lfs_file_close(&g_rm_littlefs_lfs, &file);
        APP_ERR_RET((LFS_ERR_OK != lfs_err), lfs_err, "**lfs_file_close API FAILED!!!**\r\n");

        /* Read from the file successfully */
        APP_PRINT("\r\nNumber of bytes were read: %d Byte\r\n", bytes_read);
        int32_t compare_result = memcmp(g_write_data, g_read_data, WRITE_BUFFER_SIZE);
        if (RESET_VALUE == compare_result)
        {
            APP_PRINT("\r\nThe data matches: The Write and Read buffers are identical!\r\n");
        }
        else
        {
            APP_PRINT("\r\nThe data mismatches: The Write and Read buffers are different!!!\r\n");
        }

        /* Get and Print the first 1 KB of the file content */
        memcpy(print_buffer, g_read_data, PRINT_BUFFER_SIZE);
        APP_PRINT("\r\nThe content of the first 1 KB of the file:\r\n%s\r\n", print_buffer);

        APP_PRINT("\r\nRead from the file successfully!\r\n");
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_read
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: file_delete
 *  Description  : This function deletes a file.
 *  Arguments    : None
 *  Return Value : LFS_ERR_OK    Upon successful operation
 *                 Any Other Error code apart from LFS_ERR_OK
 **********************************************************************************************************************/
static int32_t file_delete(void)
{
    int32_t lfs_err = LFS_ERR_OK;

    struct  lfs_info file_info = {RESET_VALUE};

    APP_PRINT("\r\n\r\nDelete a file...\r\n");

    /* Delete the file */
    lfs_err = lfs_remove(&g_rm_littlefs_lfs, g_file_name);
    APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err), lfs_err, "\r\n**lfs_remove API FAILED!!!**\r\n");
    if (LFS_ERR_NOENT == lfs_err)
    {
        APP_PRINT("\r\nFile does not exist!\r\n");
        lfs_err = LFS_ERR_OK;
    }
    else
    {
        /* Check the file after deleting */
        lfs_err = lfs_stat(&g_rm_littlefs_lfs, g_file_name, &file_info);
        APP_ERR_RET((LFS_ERR_OK != lfs_err) && (LFS_ERR_NOENT != lfs_err), lfs_err, "\r\n**lfs_stat API FAILED!!!**\r\n");
        if (LFS_ERR_NOENT == lfs_err)
        {
            APP_PRINT("\r\nFile deleted successfully!\r\n");
            lfs_err = LFS_ERR_OK;
        }
    }

    return lfs_err;
}
/***********************************************************************************************************************
* End of function file_delete
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: create_fixed_buffer
 *  Description  : This function creates a fixed data buffer.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
static void create_fixed_buffer(void)
{
    char * p_data = g_write_data;

    /* Clean write buffer */
    memset(p_data, NULL_CHAR, WRITE_BUFFER_SIZE);

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
 * Description  : This function is called to get user input from Terminal Application.
 * Arguments    : None
 * Return value : input_value   Return input value after receive enter key
 **********************************************************************************************************************/
static uint8_t get_user_input(void)
{
    uint8_t input_value = RESET_VALUE;

    char user_input[TERM_BUFFER_SIZE + 1];
    memset(&user_input[0], NULL_CHAR, sizeof(user_input));

    /* Wait until there is any user input */
    while (!APP_CHECK_DATA)
    {
        __NOP();
    }

    /* Read user input from the terminal */
    APP_READ(&user_input[0], TERM_BUFFER_SIZE);

    /* Convert to integer value */
    input_value = (uint8_t)atoi((char*) &user_input[0]);

    return input_value;
}
/***********************************************************************************************************************
* End of function get_user_input
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_user_request
 * Description  : This function is called to get user request from Terminal Application.
 * Arguments    : None
 * Return value : user_request   Return user request after receive enter key
 **********************************************************************************************************************/
static uint8_t handle_user_request(void)
{
    uint8_t user_input      = RESET_VALUE;
    uint8_t user_request    = RESET_VALUE;

    /* Get user input */
    user_input = get_user_input();

    /* Handle to get user request */
    switch (g_menu)
    {
        case MENU_MAIN:
        {
            switch (user_input)
            {
                case MENU_FLASH:
                {
                    g_menu = MENU_FLASH;
                    break;
                }
                case MENU_DIR:
                {
                    g_menu = MENU_DIR;
                    break;
                }
                case MENU_FILE:
                {
                    g_menu = MENU_FILE;
                    break;
                }
                default:
                {
                    APP_PRINT("\r\n\r\nInvalid Input!!!");
                    APP_PRINT("\r\nPlease provide input from below option\r\n");
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
                    user_request = REQUEST_ERASE_ENTIRE_QSPI;
                    break;
                }
                case SELECT_ERASE_32KB_FLASH:
                {
                    user_request = REQUEST_ERASE_32KB_QSPI;
                    break;
                }
                case SELECT_FORMAT_FILE_SYSTEM:
                {
                    user_request = REQUEST_FILE_SYSTEM_FORMAT;
                    break;
                }
                case SELECT_FLASH_GO_BACK:
                {
                    g_menu = MENU_MAIN;
                    break;
                }
                default:
                {
                    APP_PRINT("\r\n\r\nInvalid Input!!!");
                    APP_PRINT("\r\nPlease provide input from below option\r\n");
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
                    g_menu = MENU_MAIN;
                    break;
                }
                default:
                {
                    APP_PRINT("\r\n\r\nInvalid Input!!!");
                    APP_PRINT("\r\nPlease provide input from below option\r\n");
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
                    g_menu = MENU_MAIN;
                    break;
                }
                default:
                {
                    APP_PRINT("\r\n\r\nInvalid Input!!!");
                    APP_PRINT("\r\nPlease provide input from below option\r\n");
                    break;
                }
            }
            break;
        }
        default:
        {
            APP_PRINT("\r\n\r\nInvalid Input!!!");
            APP_PRINT("\r\nPlease provide input from below option\r\n");
            break;
        }
    }

    return user_request;
}
/***********************************************************************************************************************
* End of function handle_user_request
***********************************************************************************************************************/
