/***********************************************************************************************************************
 * File Name    : flash_lp_ep.c
 * Description  : Contains data structures and functions used in flash_lp_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas RA Flex Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#include "common_utils.h"
#include "flash_lp_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup flash_lp_ep
 * @{
 **********************************************************************************************************************/

/* Flags, set from Callback function */
static volatile _Bool g_b_flash_event_not_blank = false;
static volatile _Bool g_b_flash_event_blank = false;
static volatile _Bool g_b_flash_event_erase_complete = false;
static volatile _Bool g_b_flash_event_write_complete = false;

/*
 * private function declarations
 */

static uint8_t block_num_input(void);

/*******************************************************************************************************************//**
 * @brief       Opens the Flash_LP module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS             Upon successful open of Flash_LP module
 * @retval      err   					Upon unsuccessful open
 **********************************************************************************************************************/
fsp_err_t flash_lp_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Open Flash_LP module */
    err = R_FLASH_LP_Open(&g_flash_ctrl, &g_flash_cfg);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* Flash_LP Failure message */
        APP_ERR_PRINT("** R_FLASH_LP_Open API for Flash_LP failed ** \r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       Calls APIs to perform read/write/erase and other operations on Data FLASH.
 * @param[IN]   Block number for data flash operations
 * @retval      FSP_SUCCESS             Upon successful operations on Data FLASH
 * @retval      err						Upon unsuccessful operations on Data FLASH
 **********************************************************************************************************************/
fsp_err_t flash_lp_data_flash_operations(uint8_t block_num)
{
    fsp_err_t err = FSP_SUCCESS;	// error status
    uint8_t read_buff[BLOCK_SIZE] = {RESET_VALUE};	// read data buffer
    uint8_t write_buff[BLOCK_SIZE] = {RESET_VALUE};	// write data buffer
    flash_result_t blankCheck = RESET_VALUE;	// out parameter for BlankCheck()

    /* Set write buffer and clear read buffer */
    for(uint8_t index = RESET_VALUE; BLOCK_SIZE > index; index++)
    {
        write_buff[index] = index;
    }

    /* Perform Blank check before erase */
    /* Reset flag */
    g_b_flash_event_not_blank = false;
    g_b_flash_event_blank = false;
    err = R_FLASH_LP_BlankCheck(&g_flash_ctrl, FLASH_DF_BLOCK(block_num), BLOCK_SIZE, &blankCheck);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* Flash_LP Failure message */
        APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        /* Blank check result */
        if (FLASH_RESULT_BLANK == blankCheck)
        {
            APP_PRINT("Block %d is blank \r\n", block_num);
        }
        else if (FLASH_RESULT_NOT_BLANK == blankCheck)
        {
            APP_PRINT("Block %d is not blank \r\n", block_num);
        }
        else if (FLASH_RESULT_BGO_ACTIVE == blankCheck)
        {
            APP_PRINT("Flash is configured for BGO mode. Result is returned in callback. \r\n");
            if(FLASH_RESULT_NOT_BLANK == flash_lp_wait_for_blankcheck_flag())
            {
                APP_PRINT("Block %d is not blank \r\n", block_num);
            }
            else
            {
                APP_PRINT("Block %d is blank \r\n", block_num);
            }
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Reset erase complete flag */
    g_b_flash_event_erase_complete = false;
    /* If not blank, then perform erase operation */
    if((FLASH_RESULT_NOT_BLANK == blankCheck) || (FLASH_RESULT_NOT_BLANK == flash_lp_wait_for_blankcheck_flag()))
    {
        /* Erase a block */
        err = R_FLASH_LP_Erase(&g_flash_ctrl, FLASH_DF_BLOCK(block_num), NUM_BLOCKS);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            /* wait here for the erase operation to complete (only needed when in BGO) */
            if (g_flash_cfg.data_flash_bgo)
            {
                while (!g_b_flash_event_erase_complete)
                {
                    ;
                }
            }
            /* RTT output for erase success */
            APP_PRINT("Erase Successful \r\n");
        }
    }

    /* Perform blank check if erase operation performed */
    if(true == g_b_flash_event_erase_complete)
    {
        /* Reset flag */
        g_b_flash_event_not_blank = false;
        g_b_flash_event_blank = false;
        /* Perform blank check */
        err = R_FLASH_LP_BlankCheck(&g_flash_ctrl, FLASH_DF_BLOCK(block_num), BLOCK_SIZE, &blankCheck);
        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* Flash_LP Failure message */
            APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            /* Blank check result */
            if (FLASH_RESULT_BLANK == blankCheck)
            {
                APP_PRINT("Block %d is blank \r\n", block_num);
            }
            else if (FLASH_RESULT_NOT_BLANK == blankCheck)
            {
                APP_PRINT("Block %d is not blank \r\n", block_num);
                APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                return FSP_ERR_ERASE_FAILED;
            }
            else if (FLASH_RESULT_BGO_ACTIVE == blankCheck)
            {
                APP_PRINT("Flash is configured for BGO mode. Result is returned in callback. \r\n");
                if(FLASH_RESULT_NOT_BLANK == flash_lp_wait_for_blankcheck_flag())
                {
                    APP_PRINT("Block %d is not blank \r\n", block_num);
                    APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                    return FSP_ERR_ERASE_FAILED;
                }
                else
                {
                    APP_PRINT("Block %d is blank \r\n", block_num);
                }
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    /* Data-flash is blank, proceed further */
    /* Read From DATA FLASH */
    memcpy(read_buff, (uint8_t*)FLASH_DF_BLOCK(block_num), BLOCK_SIZE);
    /* Print the read data on RTT viewer */
    APP_PRINT("Data Read:\r\n");
    READ_DATA_PRINT(read_buff);

    /* Reset flag */
    g_b_flash_event_write_complete = false;
    /* Write Data to Data Flash */
    err = R_FLASH_LP_Write(&g_flash_ctrl, (uint32_t) write_buff, FLASH_DF_BLOCK(block_num), BLOCK_SIZE);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        /* wait here for the write operation to complete (only needed with BGO) */
        if (g_flash_cfg.data_flash_bgo)
        {
            while (!g_b_flash_event_write_complete)
            {
                ;
            }
        }
        /* RTT output for write success */
        APP_PRINT("Write successful \r\n");
    }

    /* Read From DATA FLASH */
    memcpy(read_buff, (uint8_t*)FLASH_DF_BLOCK(block_num), BLOCK_SIZE);
    /* Comparing read_buff and write_buff */
    if( RESET_VALUE == memcmp(read_buff, write_buff, BLOCK_SIZE))
    {
        /* Print the read data on RTT viewer */
        APP_PRINT("Data Read:\r\n");
        READ_DATA_PRINT(read_buff);
    }
    else
    {
        /* Print error message on RTT and return error value */
        APP_PRINT("\r\nRead data does not matches Write data.\r\n");
        APP_PRINT("\r\nPlease check configuration and addresses...\r\n");
        return FSP_ERR_PARITY;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function calls APIs to perform read/write/erase and other operations on Code FLASH.
 * @param[IN]   Block number for code flash operations
 * @retval      FSP_SUCCESS             Upon successful operations on Code FLASH
 * @retval      err    					Upon unsuccessful operations on Code FLASH
 **********************************************************************************************************************/
fsp_err_t flash_lp_code_flash_operations(uint8_t block_num)
{
    fsp_err_t err = FSP_SUCCESS;	// error status
    uint8_t read_buff[BLOCK_SIZE] = {RESET_VALUE};	// read data buffer
    uint8_t write_buff[BLOCK_SIZE] = {RESET_VALUE};	// write data buffer
    flash_result_t blankCheck = RESET_VALUE;	//	Out parameter for BlankCheck()

    /* Set write buffer and clear read buffer */
    for(uint8_t index = RESET_VALUE; BLOCK_SIZE > index; index++)
    {
        write_buff[index] = index;
    }

    /* Perform blank check */
    err = R_FLASH_LP_BlankCheck(&g_flash_ctrl, FLASH_CF_BLOCK(block_num), BLOCK_SIZE, &blankCheck);
    /* error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        /* Print BlankCheck result */
        if (FLASH_RESULT_BLANK == blankCheck)
        {
            APP_PRINT("Block %d is blank \r\n", block_num);
        }
        else if (FLASH_RESULT_NOT_BLANK == blankCheck)
        {
            APP_PRINT("Block %d is not blank \r\n", block_num);
        }
        else
        {
            /* Do nothing */
        }
    }

    /* Reset erase complete flag */
    g_b_flash_event_erase_complete = false;
    /* If not blank, then perform erase operation */
    if(FLASH_RESULT_NOT_BLANK == blankCheck)
    {
        /* Erase a block */
        err = R_FLASH_LP_Erase(&g_flash_ctrl, FLASH_CF_BLOCK(block_num), NUM_BLOCKS);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            APP_PRINT("Erase Successful \r\n");
            g_b_flash_event_erase_complete = true;
        }

    }

    /* Perform Blank check after erase operation */
    if(true == g_b_flash_event_erase_complete)
    {
        err = R_FLASH_LP_BlankCheck(&g_flash_ctrl, FLASH_CF_BLOCK(block_num), BLOCK_SIZE, &blankCheck);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("** R_FLASH_LP_BlankCheck API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            /* Print BlankCheck result */
            if (FLASH_RESULT_BLANK == blankCheck)
            {
                APP_PRINT("Block %d is blank \r\n", block_num);
            }
            else if (FLASH_RESULT_NOT_BLANK == blankCheck)
            {
                APP_PRINT("Block %d is not blank \r\n", block_num);
                APP_PRINT("R_FLASH_LP_Erase() failed to erase %d block\r\n", block_num);
                return FSP_ERR_ERASE_FAILED;
            }
            else
            {
                /* Do nothing */
            }
        }
    }

    /* If code-flash is blank proceed further */
    /* Read code flash data */
    memcpy(read_buff, (uint8_t*)FLASH_CF_BLOCK(block_num), BLOCK_SIZE);
    /* Print the read data on RTT viewer */
    APP_PRINT("Data Read:\r\n");
    READ_DATA_PRINT(read_buff);

    /* Start sector write operation */
    err = R_FLASH_LP_Write(&g_flash_ctrl, (uint32_t) write_buff, FLASH_CF_BLOCK(block_num), BLOCK_SIZE);
    /* error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        APP_PRINT("Write successful\n");
    }
    /* Read code flash data */
    memcpy(read_buff, (uint8_t*)FLASH_CF_BLOCK(block_num), BLOCK_SIZE);

    /* Comparing read_buff and write_buff */
    if( RESET_VALUE == memcmp(read_buff, write_buff, BLOCK_SIZE))
    {
        /* Print the read data on RTT viewer */
        APP_PRINT("Data Read:\r\n");
        READ_DATA_PRINT(read_buff);
    }
    else
    {
        /* Print error message on RTT and return error value */
        APP_PRINT("\r\nRead data does not matches Write data.\r\n");
        APP_PRINT("\r\nPlease check configuration and addresses...\r\n");
        return FSP_ERR_PARITY;
    }
    /* Code flash access window example */
    APP_PRINT("\r\n************************************\r\n");
    APP_PRINT("*  Code Flash Access Window example\r\n");
    APP_PRINT("************************************\r\n");

    /* Block number for access window example */
    block_num = block_num_input();

    /* Set Access window.
     * CAUTION: Highly recommended not to use this function if not aware of consequences OR use it with the accessWindowClear API at the end of application.
     * This API locks the Code flash and the lock retains even after power cycle.
     * Which means, even after power cycle, user will not be able to program the code to code flash if the
     * access window is wrongly set.
     *
     * WORKAROUND: If uses uses accessWindowSet and locks the window. Flash can be unlocked by running different application
     * performing just "open" call and "accessWindowClear()" running from the RAM.
     */
    err = R_FLASH_LP_AccessWindowSet(&g_flash_ctrl, FLASH_CF_START_ADDR(block_num), FLASH_CF_END_ADDR(block_num));
    /* error handle */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** R_FLASH_LP_AccessWindowSet API for Flash_LP failed ** \r\n");
        return err;
    }
    else
    {
        APP_PRINT("Code flash Access window set\n");

        /* Erase block again */
        err = R_FLASH_LP_Erase(&g_flash_ctrl, FLASH_CF_BLOCK(block_num), NUM_BLOCKS );
        /* error handle */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("** R_FLASH_LP_Erase API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            APP_PRINT("Erase Successful \r\n");
        }

        /* Perform write operation and check error code */
        err = R_FLASH_LP_Write(&g_flash_ctrl, (uint32_t) write_buff, FLASH_CF_BLOCK(block_num), BLOCK_SIZE);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("** R_FLASH_LP_Write API for Flash_LP failed ** \r\n");
            return err;
        }
        else
        {
            APP_PRINT("Write successful\n");
        }

        /* Clear Flash Access Window */
        err = R_FLASH_LP_AccessWindowClear(&g_flash_ctrl);
        /* error handle */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("** R_FLASH_LP_AccessWindowClear API for Flash_LP failed ** \r\n");
        }
        else
        {
            APP_PRINT("Code flash Access window cleared \r\n");
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions checks the flag for blank check.
 * @param[IN]   block_num				Block number of Data-Flash.
 * @retval      FLASH_RESULT_BLANK		If block is blank.
 * @retval		FLASH_RESULT_NOT_BLANK	If block is not blank.
 **********************************************************************************************************************/
flash_result_t flash_lp_wait_for_blankcheck_flag(void)
{
    /* Wait for callback function to set flag */
    while (!(g_b_flash_event_not_blank||g_b_flash_event_blank))
    {
        ;
    }
    if (g_b_flash_event_not_blank)
    {
        /* Reset flag */
        return FLASH_RESULT_NOT_BLANK;
    }
    else
    {
        /* Reset flag */
        return FLASH_RESULT_BLANK;
    }
}

/*******************************************************************************************************************//**
 * @brief       Closes the Flash_LP module.
 * @retval      FSP_SUCCESS             Upon successful close of Flash_LP module.
 * @retval      err						Upon unsuccessful close.
 **********************************************************************************************************************/
fsp_err_t flash_lp_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Close Flash_LP module */
    err = R_FLASH_LP_Close(&g_flash_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* Flash_LP Failure message */
        APP_ERR_PRINT("** R_FLASH_LP_Close API for Flash_LP failed ** \r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       Takes block number input from user for access windows operations.
 * @param[IN]  	None.
 * @retval      block number          	Block number for code flash access windows operations.
 **********************************************************************************************************************/
static uint8_t block_num_input(void)
{
    uint8_t block_num = RESET_VALUE;	// User input data/code flash block number
    char rByte[BUFF_LEN] = {'\0'};	// RTT Input buffer
    unsigned num_bytes = RESET_VALUE;	// Number of bytes read by RTT

    /* Code-Flash block number input and validation */
    APP_PRINT("\r\nEnter block number greater than 10 for Access Window operations\r\n");
    do
    {
        while (BYTES_RECEIVED_ZERO == num_bytes)
        {
            if (APP_CHECK_DATA)
            {
                num_bytes = APP_READ(rByte);
                if (BYTES_RECEIVED_ZERO == num_bytes)
                {
                    APP_PRINT("\r\nInvalid Input\r\n");
                }
            }
        }

        /* Checking whether input is numerical value or not */
        if(('0' <= rByte[ARRAY_INDEX]) && ('9' >= rByte[ARRAY_INDEX]))
        {
            if(('.' != rByte[ARRAY_INDEX + 2]) && ('.' != rByte[ARRAY_INDEX + 3]) &&
                    (('\n' == rByte[ARRAY_INDEX + 2]) || ('\n' == rByte[ARRAY_INDEX + 3])))
            {
                /* Converting block number string to integer */
                block_num = (uint8_t)atoi(rByte);
            }
            else
            {
                APP_PRINT("\r\nEntered Code-Flash block number is not valid \r\n");
                APP_PRINT("Please enter block number between  10 and 126 \r\n");
                num_bytes = RESET_VALUE;
                continue;
            }
        }
        else
        {
            APP_PRINT("\r\nEntered Code-Flash block number is not valid \r\n");
            APP_PRINT("Please enter block number greater than 10 \r\n");
            num_bytes = RESET_VALUE;
            continue;
        }

        /* Code-Flash block number validation */
        if (FLASH_CF_BLOCK10 > FLASH_CF_BLOCK(block_num))
        {
            APP_PRINT("\r\nEntered Code-Flash block number is not valid \r\n");
            APP_PRINT("Please enter block number greater than 10 \r\n");
            num_bytes = RESET_VALUE;
        }
        else
        {
            break;
        }

    }while (FLASH_CF_BLOCK10 > FLASH_CF_BLOCK(block_num));

    return block_num;
}

/* Callback function for FLASH LP HAL. */
void bgo_callback(flash_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case FLASH_EVENT_NOT_BLANK:
            g_b_flash_event_not_blank = true;
            break;
        case FLASH_EVENT_BLANK:
            g_b_flash_event_blank = true;
            break;
        case FLASH_EVENT_ERASE_COMPLETE:
            g_b_flash_event_erase_complete = true;
            break;
        case FLASH_EVENT_WRITE_COMPLETE:
            g_b_flash_event_write_complete = true;
            break;
        default:
            /* Do nothing */
            break;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup flash_lp_ep)
 **********************************************************************************************************************/
