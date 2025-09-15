/***********************************************************************************************************************
 * File Name    : vee_flash_ep.c
 * Description  : Contains function definition.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "vee_flash_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup vee_flash_ep
 * @{
 **********************************************************************************************************************/

/* Capture callback event */
static volatile bool g_write_flag = false;

/* Capture the vee_flash status */
rm_vee_status_t p_status          = {(rm_vee_state_t)RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};

/*******************************************************************************************************************//**
 * @brief       This functions initializes VEE_FLASH module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful open of VEE_FLASH module.
 * @retval      Any other error code apart from FSP_SUCCESS  Unsuccessful open.
 **********************************************************************************************************************/
fsp_err_t vee_flash_init(void)
{
    fsp_err_t err                = FSP_SUCCESS;

    /* Open the RM_VEE_FLASH driver module */
    err = RM_VEE_FLASH_Open(&g_vee_ctrl, &g_vee_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** RM_VEE_FLASH_Open API FAILED **\r\n");
    }
    return err;
}

/*****************************************************************************************************************
 * @brief       Writes data to data flash.
 * @param[IN]   p_data          Actual data.
 * @param[IN]   rec_id          Record ID.
 * @param[IN]   bytes           Number of bytes to write.
 * @retval      FSP_SUCCESS     Upon successful data write.
 * @retval      Any other error code apart from FSP_SUCCESS on Unsuccessful data write.
 ****************************************************************************************************************/
fsp_err_t vee_write_operation(uint32_t const rec_id, void *  p_data, uint32_t bytes)
{
    fsp_err_t err                = FSP_SUCCESS;
    uint16_t write_time_out      = UINT16_MAX;

    /* Write the data to a Virtual EEPROM Record */
    err = RM_VEE_FLASH_RecordWrite(&g_vee_ctrl, rec_id, p_data, bytes);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** RM_VEE_FLASH_RecordWrite API FAILED **\r\n");
        return err;
    }

    /* Wait for the Virtual EEPROM callback to indicate that it has finished writing data
     * and VEE Flash is in ready state */
    while (true != g_write_flag)
    {
        /* Start checking for time out to avoid infinite loop */
        --write_time_out;
        if (RESET_VALUE == write_time_out)
        {
            /* We have reached to a scenario where callback event did not occur */
            APP_ERR_PRINT("\r\n** callback event not received. **\r\n");
            return FSP_ERR_TIMEOUT;
        }
    }

    /* Reset the flag */
    g_write_flag = false;
    return err;
}

/*****************************************************************************************************************
 * @brief       Reads data from Record_ID.
 * @param[IN]   p_record_data   Pointer to the record.
 * @param[IN]   rec_id          Record ID.
 * @retval      FSP_SUCCESS     Upon successful data write.
 * @retval      Any other error code apart from FSP_SUCCESS on Unsuccessful data write.
 ****************************************************************************************************************/
fsp_err_t vee_read_operation(uint32_t const rec_id)
{
    fsp_err_t err                = FSP_SUCCESS;
    uint8_t flt_str[BUF_SIZE]    = {RESET_VALUE};
    uint32_t     out_len         = RESET_VALUE;
    void *  p_record_data        = NULL;
    adxl_t  adxl_data            = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* Get a pointer to the record that is stored in data flash */
    err = RM_VEE_FLASH_RecordPtrGet(&g_vee_ctrl, rec_id, (uint8_t **)&p_record_data, &out_len);
    if (FSP_SUCCESS == err)
    {
        /* Print Integer data read out from Record_ID1 */
        if(REC_ID_1 == rec_id)
        {
            for(uint8_t cnt = RESET_VALUE; cnt< out_len; cnt++)
            {
                APP_PRINT("data[%d] = 0x%x\r\n", cnt, *((uint8_t *)p_record_data));
                p_record_data = ((uint8_t *)p_record_data + ONE);
            }
        }
        /* Print Char data read out from Record_ID2 */
        else if(REC_ID_2 == rec_id)
        {
            for(uint8_t cnt = RESET_VALUE; cnt< out_len; cnt++)
            {
                APP_PRINT("data[%d] = %c\r\n", cnt, *((uint8_t *)p_record_data));
                p_record_data = ((uint8_t *)p_record_data + ONE);
            }
        }
        /* Print float data read out from Record_ID3 */
        else if(REC_ID_3 == rec_id)
        {
            for(uint8_t cnt = RESET_VALUE; cnt< DATA_SIZE; cnt++)
            {
                snprintf((char *)flt_str, sizeof(flt_str), "data[%d] = %0.02f", cnt, *((float *)p_record_data));
                APP_PRINT("%s\r\n", flt_str);
                p_record_data = ((float *)p_record_data + ONE);
            }
        }
        /* Print structure data read out from Record_ID4 */
        else
        {
            /* Copying record data to instance of the adxl_t data structure */
            memcpy(&adxl_data, (adxl_t *)p_record_data, sizeof(adxl_t));
            APP_PRINT("X-axis  = %d\r\n", adxl_data.x_data);
            APP_PRINT("Y-axis  = %d\r\n", adxl_data.y_data);
            APP_PRINT("Z-axis  = %d\r\n", adxl_data.z_data);
            APP_PRINT("XY-axis = %d\r\n", adxl_data.xy_data);
        }
        APP_PRINT("\r\nReading out previously written data from requested Record_ID completed successfully.\r\n");
    }
    /* This condition will occur, when the user tries to read data from the Record_ID
     * where no data was written previously or flash is blank */
    else if (FSP_ERR_NOT_FOUND == err)
    {
        APP_PRINT("\r\nNo data present on requested Record_ID or flash is formatted.\r\n");
    }
    else
    {
        APP_ERR_PRINT("\r\n** RM_VEE_FLASH_RecordPtrGet API failed **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function erases the entire data flash used by VEEPROM.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful format.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t vee_format_operation(void)
{
    fsp_err_t err                = FSP_SUCCESS;
    uint8_t ref_data             = RESET_VALUE;

    /* Start a manual format operation */
    err = RM_VEE_FLASH_Format(&g_vee_ctrl, &ref_data);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** RM_VEE_FLASH_Format API FAILED **\r\n");
        return err;
    }

    /* Get the current status of the driver */
    err = RM_VEE_FLASH_StatusGet(&g_vee_ctrl, &p_status);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** RM_VEE_FLASH_StatusGet API FAILED **\r\n");
        return err;
    }
    /* Compare Last ID written with Default ID */
    if (DEFAULT_ID == p_status.last_id)
    {
        APP_PRINT("\r\nFormat operation completed successfully.\r\n");
    }
    else
    {
        err = FSP_ERR_NOT_ERASED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function gets status of vee_flash.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful status get.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t vee_status_get_operation(void)
{
    fsp_err_t err                = FSP_SUCCESS;

    /* Get the current status of the driver */
    err = RM_VEE_FLASH_StatusGet(&g_vee_ctrl, &p_status);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** RM_VEE_FLASH_StatusGet API FAILED **\r\n");
    }
    else
    {
        APP_PRINT("Last ID written = %d\r\n", p_status.last_id);
        APP_PRINT("Remaining space available in the segment = %d\r\n", p_status.space_available);
        APP_PRINT("\r\nStatusGet operation completed successfully.\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function performs refresh on vee_flash.
 * @param[IN]   None
 * @retval      FSP_SUCCESS     Upon successful refresh.
 * @retval      Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t vee_refresh_operation(void)
{
    fsp_err_t err                = FSP_SUCCESS;
    int32_t status_time_out      = INT32_MAX;

    /* Force refresh and wait to complete */
    err = RM_VEE_FLASH_Refresh(&g_vee_ctrl);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** RM_VEE_FLASH_Refresh API FAILED **\r\n");
        return err;
    }

    do
    {
        /* Get the current status of the driver */
        err = RM_VEE_FLASH_StatusGet(&g_vee_ctrl, &p_status);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** RM_VEE_FLASH_StatusGet API FAILED **\r\n");
            return err;
        }
    }while((RM_VEE_STATE_READY != p_status.state) && (RESET_VALUE < --status_time_out));
    if (RESET_VALUE == status_time_out)
    {
        APP_ERR_PRINT("\r\n** vee flash state is not ready due to time out **\r\n");
        return FSP_ERR_TIMEOUT;
    }
    else
    {
        APP_PRINT("Current segment erase count = %d\r\n", p_status.segment_erase_count);
    }
    APP_PRINT("\r\nRefresh operation completed successfully.\r\n");
    return err;
}

/*****************************************************************************************************************
 *  @brief      Process input string to integer value.
 *  @param[in]  None
 *  @retval     Integer value of input string.
 ****************************************************************************************************************/
uint8_t process_input_data(void)
{
    uint8_t  buf[BUFFER_SIZE_DOWN]      = {INITIAL_VALUE};
    uint32_t num_bytes                  = RESET_VALUE;
    uint8_t  value                      = RESET_VALUE;

    while (RESET_VALUE == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            num_bytes = APP_READ(buf);
            if (RESET_VALUE == num_bytes)
            {
                APP_PRINT("\r\nInvalid Input\r\n");
            }
        }
    }

    /* Conversion from input string to integer value */
    value = (uint8_t) (atoi((char *)buf));
    return value;
}

/*******************************************************************************************************************//**
 *  @brief        User defined callback function.
 *  @param[IN]    p_args
 *  @retval       None
 **********************************************************************************************************************/
void vee_callback(rm_vee_callback_args_t * p_args)
{
    if ((NULL != p_args) && (RM_VEE_STATE_READY == p_args->state))
    {
        g_write_flag = true;
    }
}

/*******************************************************************************************************************//**
 *  @brief       Closes the Flash driver and VEE driver, prints and traps error.
 *  @param[IN]   status    Error status.
 *  @param[IN]   err_str   Error string.
 *  @retval      None
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        if (FSP_SUCCESS != RM_VEE_FLASH_Close(&g_vee_ctrl))
        {
            APP_ERR_PRINT("\r\n** RM_VEE_FLASH_Close API Failed **\r\n ");
        }
        APP_PRINT(err_str);
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup vee_flash_ep)
 **********************************************************************************************************************/
