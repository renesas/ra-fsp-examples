/***********************************************************************************************************************
 * File Name    : mram_ep.c
 * Description  : Contains macros, data structures and functions used in mram_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "mram_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup MRAM_EP
 * @{
 **********************************************************************************************************************/

/* Private functions */
static fsp_err_t mram_write(void);
static fsp_err_t mram_erase(void);
static bool mram_is_blank(uint32_t mram_address, uint32_t num_bytes);
static void mram_read(void);
static void mram_deinit(void);

/***********************************************************************************************************************
 * @brief      This function is used to start the MRAM example operation.
 * @param[IN]  None
 * @retval     None
 **********************************************************************************************************************/
void mram_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version;
    uint32_t read_data = RESET_VALUE;
    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    APP_PRINT(BANNER_1);
    APP_PRINT(BANNER_2);
    APP_PRINT(BANNER_3, EP_VERSION);
    APP_PRINT(BANNER_4, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(BANNER_5);
    APP_PRINT(BANNER_6);
    APP_PRINT(BANNER_INFO);

    /* Open MRAM */
    err = R_MRAM_Open(&g_mram_ctrl, &g_mram_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_PRINT("\r\nR_MRAM_Open API failed");
        APP_ERR_TRAP(err);
    }

    /* Setup Default Block 0 as Startup Setup Block */
    err = R_MRAM_StartUpAreaSelect(&g_mram_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);
    if (err != FSP_SUCCESS)
    {
        APP_PRINT("\r\nR_MRAM_StartUpAreaSelect API failed");
        APP_ERR_TRAP(err);
    }
    APP_PRINT(MENU);

    unsigned char rByte[BUFFER_SIZE_DOWN] = { RESET_VALUE };
    while (EXIT != read_data)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte);
            read_data = (uint32_t)(atoi((char *)(rByte)));
            switch (read_data)
            {
                case MRAM_WRITE:
                    err = mram_write();
                    if (FSP_SUCCESS != err)
                    {
                        mram_deinit();
                        APP_ERR_TRAP(err);
                    }
                break;
                case MRAM_READ:
                    APP_PRINT("\r\nReading from MRAM...");
                    mram_read();
                break;
                case MRAM_ERASE:
                    APP_PRINT("\r\nErasing MRAM...");
                    /* Erase Block */
                    err = mram_erase();
                    if (FSP_SUCCESS != err)
                    {
                        mram_deinit();
                        APP_ERR_TRAP(err);
                    }
                break;
                case EXIT:
                    APP_PRINT("\r\nExiting the MRAM, the user has to restart the application");
                    mram_deinit();
                break;
                default:
                    APP_PRINT(MENU);
                break;
            }
            if ((MRAM_WRITE <= read_data) && (EXIT > read_data))
            {
                APP_PRINT("\r\nEnter any key (other than 1, 2, 3, 4) to go back to the main menu\r\n");
                read_data = RESET_VALUE;
                memset(rByte, RESET_VALUE, BUFFER_SIZE_DOWN);
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       Writes sequential data to MRAM.
 *              The write region is defined by MRAM_START_ADDR and NUM_BYTES macros.
 * @param[in]   None
 * @retval      FSP_SUCCESS if the write operation is successful;
 *              error code otherwise.
 **********************************************************************************************************************/
static fsp_err_t mram_write(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t write_buffer[NUM_BYTES] = {RESET_VALUE};

    /* Check blank before writing */
    if (!mram_is_blank(MRAM_START_ADDR, NUM_BYTES))
    {
        APP_PRINT("\r\nExisting data detected in the MRAM region. The region will be overwritten.\r\n");
    }

    /* Set the write buffer */
    for (uint8_t index = 0; index < NUM_BYTES; index++)
    {
        write_buffer[index] = index;
    }

    APP_PRINT("\r\nWriting to MRAM...");

    /* Disable interrupts to prevent vector table access while code MRAM is in P/E mode */
    __disable_irq();

    /* Write data to MRAM. There is no need to perform an erase first. */
    err = R_MRAM_Write(&g_mram_ctrl, (uint32_t)write_buffer, MRAM_START_ADDR, NUM_BYTES);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_MRAM_Write API failed. Please restart the application.");
        return err;
    }

    APP_PRINT("\r\nMRAM write successful. Data written: 0x00 to 0x%02X\r\n", NUM_BYTES - 1);

    /* Compare the data written to MRAM with the original buffer */
    if (RESET_VALUE == memcmp((const void *) MRAM_START_ADDR, write_buffer, NUM_BYTES))
    {
        APP_PRINT("\r\nData verification successful\r\n");
    }
    else
    {
        APP_PRINT("\r\nData verification failed: mismatch detected\r\n");
    }

    /* Enable interrupts after code MRAM operations are complete */
    __enable_irq();

    return err;
}

/*******************************************************************************************************************//**
 * @brief       Reads data from MRAM and prints it to the RTT Viewer in hexadecimal format.
 *              The read region is defined by MRAM_START_ADDR and NUM_BYTES macros.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
static void mram_read(void)
{
    uint8_t read_buffer[NUM_BYTES] = {RESET_VALUE};
    /* Read data from MRAM */
    memcpy(read_buffer, (uint8_t *) MRAM_START_ADDR, NUM_BYTES);

    APP_PRINT("\r\nMRAM read successful\r\n");

    /* Print the data read from MRAM to RTT Viewer */
    APP_PRINT("\r\nRead data:\r\n");
    for(uint8_t index = RESET_VALUE; NUM_BYTES > index; index++)
    {
        APP_PRINT("%02X ", *(read_buffer + index));
        if((index + 1) % 8 == 0)
        {
            APP_PRINT("\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       Erases MRAM blocks starting from a predefined address.
 *              The erase region is defined by MRAM_START_ADDR and BLOCK_NUM macros.
 * @param[in]   None
 * @retval      FSP_SUCCESS if the erase operation is successful;
 *              error code otherwise.
 **********************************************************************************************************************/
static fsp_err_t mram_erase(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Disable interrupts to prevent vector table access while code MRAM is in P/E mode */
    __disable_irq();

    err = R_MRAM_Erase(&g_mram_ctrl, MRAM_START_ADDR, BLOCK_NUM);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_MRAM_Erase API failed. Please restart the application.");
        return err;
    }
    
    APP_PRINT("\r\nMRAM erase successful\r\n");

    /* Enable interrupts after code MRAM operations are complete */
    __enable_irq();
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Checks whether a specified MRAM region is blank (all bytes are 0xFF).
 * @param[in]   mram_address    Start address of the MRAM region to check.
 *              num_bytes       Number of bytes to check.
 * @retval      true            Region is blank.
 *              false           Region contains non-blank data.
 **********************************************************************************************************************/
static bool mram_is_blank(uint32_t mram_address, uint32_t num_bytes)
{
    uint8_t *p_addr = (uint8_t *) mram_address;
    for (uint32_t index = 0; index < num_bytes; index++)
    {
        if (p_addr[index] != UINT8_MAX)
        {
            return false; /* Not blank */
        }
    }
    return true;
}

/*******************************************************************************************************************//**
 * @brief       De-initializes MRAM module.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void mram_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    err = R_MRAM_Close(&g_mram_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_MRAM_Close API failed");
    }
}

/*******************************************************************************************************************//**
 * @} (end defgroup MRAM_EP)
 **********************************************************************************************************************/
