/**********************************************************************************************************************
 * File Name    : board_ospi.c
 * Description  : Contains macros data structures and functions used in board_ospi.c and filex_ep.c.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "board_ospi.h"

/* Private functions */
static fsp_err_t ospi_b_write_enable(void);
static fsp_err_t ospi_b_setup_calibrate_data(void);
static fsp_err_t ospi_b_set_protocol_to_opi(void);

/* Private variables */
static ospi_b_instance_ctrl_t * s_ospi_b_ctrl;
static spi_flash_cfg_t s_ospi_b_cfg;
static spi_flash_direct_transfer_t s_ospi_b_direct_transfer [OSPI_B_TRANSFER_MAX] =
{
    /* Transfer structure for SPI mode */
    [OSPI_B_TRANSFER_ENTER_4_ADDR_SPI] =
    {
        .command        = OSPI_B_COMMAND_ENTER_4_ADDR_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_ZERO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_EXIT_4_ADDR_SPI] =
    {
        .command        = OSPI_B_COMMAND_EXIT_4_ADDR_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_ZERO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_ENABLE_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_ENABLE_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_ZERO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_REG_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },

#if defined (BOARD_RA8D1_EK)
    [OSPI_B_TRANSFER_WRITE_CFR5V_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    [OSPI_B_TRANSFER_WRITE_CR_00H_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CR_00H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CR_01H_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CR_01H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_ENTER_4BYTES_ADDRESS] =
    {
        .command        = OSPI_B_COMMAND_ENTER_4BYTES_ADDR,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_DUMMY,
        .data_length    = OSPI_B_DATA_DUMMY,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    [OSPI_B_TRANSFER_WRITE_CR2_000H_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CR2_000H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
    [OSPI_B_TRANSFER_WRITE_CR2_300H_SPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CR2_300H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_SPI
    },
#endif /* Specific board */

    [OSPI_B_TRANSFER_READ_STATUS_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_STATUS_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI
    },
    [OSPI_B_TRANSFER_READ_VOL_REG_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_NON_VOL_REG_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_MEMORY_SPI
    },
    [OSPI_B_TRANSFER_READ_DEVICE_ID_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_DEVICE_ID_SPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_FOUR,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_SPI
    },

#if defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    [OSPI_B_TRANSFER_READ_CR_00H_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CR_00H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_CR_01H_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CR_01H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    [OSPI_B_TRANSFER_READ_CR2_000H_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CR2_000H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
    [OSPI_B_TRANSFER_READ_CR2_300H_SPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_SPI,
        .address        = OSPI_B_ADDRESS_CR2_300H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_SPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_ONE,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_SPI
    },
#endif /* Specific board */

    /* Transfer structure for OPI mode */
    [OSPI_B_TRANSFER_WRITE_ENABLE_OPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_ENABLE_OPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_ZERO,
        .data_length    = OSPI_B_DATA_LENGTH_ZERO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI
    },
    [OSPI_B_TRANSFER_WRITE_REG_OPI] =
    {
        .command        = OSPI_B_COMMAND_WRITE_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_WRITE_OPI
    },
    [OSPI_B_TRANSFER_READ_STATUS_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_STATUS_OPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI
    },
    [OSPI_B_TRANSFER_READ_VOL_REG_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },

#if defined (BOARD_RA8D1_EK)
    [OSPI_B_TRANSFER_READ_CFR5V_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_CFR5V_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    [OSPI_B_TRANSFER_READ_CR_00H_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_CR_00H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [OSPI_B_TRANSFER_READ_CR_01H_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_CR_01H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    [OSPI_B_TRANSFER_READ_CR2_000H_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_CR2_000H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
    [OSPI_B_TRANSFER_READ_CR2_300H_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_REGISTER_OPI,
        .address        = OSPI_B_ADDRESS_CR2_300H_REGISTER,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
        .data_length    = OSPI_B_DATA_LENGTH_TWO,
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_REGISTER_OPI
    },
#endif /* Specific board */

    [OSPI_B_TRANSFER_READ_DEVICE_ID_OPI] =
    {
        .command        = OSPI_B_COMMAND_READ_DEVICE_ID_OPI,
        .address        = OSPI_B_ADDRESS_DUMMY,
        .data           = OSPI_B_DATA_DUMMY,
        .command_length = OSPI_B_COMMAND_LENGTH_OPI,
        .address_length = OSPI_B_ADDRESS_LENGTH_FOUR,
#if defined (BOARD_RA8D1_EK)
        .data_length    = OSPI_B_DATA_LENGTH_FOUR,
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
        .data_length    = OSPI_B_DATA_LENGTH_THREE,
#endif /* Specific board */
        .dummy_cycles   = OSPI_B_DUMMY_CYCLE_READ_STATUS_OPI
    },
};

/***********************************************************************************************************************
 * Function Name: ospi_b_write_enable
 * Description  : This function enables write and verifies the read data.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Enabled successfully.
 *                Any other error codes     Failed to enable.
 **********************************************************************************************************************/
static fsp_err_t ospi_b_write_enable(void)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data           = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };

    /* Transfer write enable command */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == s_ospi_b_ctrl->spi_protocol)
               ? s_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_SPI]
               : s_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_ENABLE_OPI];
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read status register */
    transfer = (SPI_FLASH_PROTOCOL_EXTENDED_SPI == s_ospi_b_ctrl->spi_protocol)
               ? s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_SPI]
               : s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_STATUS_OPI];
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Check write enable bit in status register */
    if (OSPI_B_WEN_BIT_MASK != (transfer.data & OSPI_B_WEN_BIT_MASK))
    {
        return FSP_ERR_ABORTED;
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_write_enable
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: ospi_b_wait_operation
 * Description  : This function waits until OSPI operation completes.
 * Arguments    : timeout                   Maximum waiting time.
 * Return Value : FSP_SUCCESS               Operated successfully.
 *                Any other error codes     Failed to operate.
 **********************************************************************************************************************/
fsp_err_t ospi_b_wait_operation(uint32_t timeout)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_status_t status = {RESET_VALUE};

    status.write_in_progress = true;
    while (status.write_in_progress)
    {
        /* Get device status */
        err = R_OSPI_B_StatusGet(s_ospi_b_ctrl, &status);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
        if (RESET_VALUE == timeout)
        {
            return FSP_ERR_TIMEOUT;
        }
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
        timeout --;
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_wait_operation
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: ospi_b_init
 * Description  : This function initializes OSPI module and Flash device.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Initialized successfully.
 *                Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t ospi_b_init(void)
{
    /* By default, the flash device is in SPI mode, so it is necessary to open the OSPI module in SPI mode first */
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data           = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };

    /* Set active OSPI instance for initialize */
    s_ospi_b_ctrl = &g_ospi_b_init_ctrl;
    memset(&s_ospi_b_cfg, RESET_VALUE, sizeof(spi_flash_cfg_t));
    memcpy(&s_ospi_b_cfg, &g_ospi_b_init_cfg, sizeof(spi_flash_cfg_t));
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open OSPI module */
    err = R_OSPI_B_Open(s_ospi_b_ctrl, &s_ospi_b_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Transfer write enable command */
    err = ospi_b_write_enable();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

#if defined (BOARD_RA8D1_EK)
    /* Transfer enter 4 byte address command */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_ENTER_4_ADDR_SPI];
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read back and verify CFR2V register data to confirm 4 byte address mode */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_VOL_REG_SPI];
    transfer.address = OSPI_B_ADDRESS_CFR2V_REGISTER;
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (OSPI_B_DATA_CFR2V_REGISTER != (uint8_t)transfer.data)
    {
        return FSP_ERR_ABORTED;
    }

    /* Read back and verify CFR3V register data */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_VOL_REG_SPI];
    transfer.address = OSPI_B_ADDRESS_CFR3V_REGISTER;
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (OSPI_B_DATA_DEFAULT_CFR3V_REGISTER != (uint8_t)transfer.data)
    {
        return FSP_ERR_ABORTED;
    }

    /* Read back and verify CFR4V register data */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_VOL_REG_SPI];
    transfer.address = OSPI_B_ADDRESS_CFR4V_REGISTER;
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (OSPI_B_DATA_CFR4V_REGISTER != (uint8_t)transfer.data)
    {
        if (OSPI_B_DATA_DEFAULT_CFR4V_REGISTER != (uint8_t)transfer.data)
        {
            return FSP_ERR_ABORTED;
        }

        /* Transfer write enable command */
        err = ospi_b_write_enable();
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Write to CFR4N to configure Error Correction Code */
        transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_REG_SPI];
        transfer.address = OSPI_B_ADDRESS_CFR4N_REGISTER;
        transfer.data = OSPI_B_DATA_CFR4V_REGISTER;

        err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        err = ospi_b_wait_operation(UINT16_MAX);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Read back and verify CFR4V register data */
        transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_VOL_REG_SPI];
        transfer.address = OSPI_B_ADDRESS_CFR4V_REGISTER;

        err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        if (OSPI_B_DATA_CFR4V_REGISTER != (uint8_t)transfer.data)
        {
            return FSP_ERR_ABORTED;
        }

        /* Erase Chip after configure Error Correction Code */
        err = R_OSPI_B_Erase(s_ospi_b_ctrl, (uint8_t *)OSPI_B_CS1_START_ADDRESS, SPI_FLASH_ERASE_SIZE_CHIP_ERASE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        err = ospi_b_wait_operation(UINT32_MAX);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

    }
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    /* Enter 4-bytes address */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_ENTER_4BYTES_ADDRESS];
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Write to ADDR 00000001H of CR to configure dummy cycle */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR_01H_SPI];
    transfer.data = OSPI_B_DATA_SET_CR_01H;
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read back and verify CR register data */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR_01H_SPI];
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (OSPI_B_DATA_SET_CR_01H != (uint8_t)transfer.data)
    {
        return FSP_ERR_ABORTED;
    }
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    /* Write to ADDR 00000300H of CR2 to configure dummy cycle */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR2_300H_SPI];
    transfer.data = OSPI_B_DATA_SET_CR2_300H;
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read back and verify CR2 register data */
    transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR2_300H_SPI];
    err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    if (OSPI_B_DATA_SET_CR2_300H != (uint8_t)transfer.data)
    {
        return FSP_ERR_ABORTED;
    }
#endif /* Specific board */

    /* Setup auto-calibration data */
    err = ospi_b_setup_calibrate_data();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Configure OSPI flash to OPI mode */
    err = ospi_b_set_protocol_to_opi();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Close OSPI flash */
    err = R_OSPI_B_Close(s_ospi_b_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set active OSPI instance for FileX */
    s_ospi_b_ctrl = &g_ospi_b_filex_ctrl;
    memset(&s_ospi_b_cfg, RESET_VALUE, sizeof(spi_flash_cfg_t));
    memcpy(&s_ospi_b_cfg, &g_ospi_b_filex_cfg, sizeof(spi_flash_cfg_t));

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: erase_ospi_flash
 * Description  : This function erases the flash memory using lower level flash APIs.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Erased successfully.
 *                Any other error codes     Failed to erase.
 **********************************************************************************************************************/
fsp_err_t erase_ospi_flash(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (!s_ospi_b_ctrl->open)
    {
        /* Open OSPI module */
        err = R_OSPI_B_Open(s_ospi_b_ctrl, &s_ospi_b_cfg);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    err = R_OSPI_B_Erase(s_ospi_b_ctrl, (uint8_t *)OSPI_B_CS1_START_ADDRESS, SPI_FLASH_ERASE_SIZE_CHIP_ERASE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = ospi_b_wait_operation(UINT32_MAX);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Re-setup calibrate data after erase the entire OSPI Flash */
    err = ospi_b_setup_calibrate_data();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Close OSPI flash */
    err = R_OSPI_B_Close(s_ospi_b_ctrl);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Clear the opened flag after close flash */
    g_media_opened = false;

    return err;
}
/***********************************************************************************************************************
* End of function erase_ospi_flash
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: erase_ospi_flash_range
 * Description  : This function erases a range of the flash memory using lower level flash APIs.
 * Arguments    : p_address                 Start address of the range to be erased.
 *                size                      Size, in bytes, of the range to be erased.
 * Return Value : FSP_SUCCESS               Erased successfully.
 *                Any other error codes     Failed to erase.
 **********************************************************************************************************************/
fsp_err_t erase_ospi_flash_range(uint8_t * p_address, uint32_t size)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t * p_current_address = p_address;

#if defined (BOARD_RA8D1_EK)
    uint32_t sector_size = OSPI_B_SECTOR_256K_SIZE;
    uint32_t erase_timeout = OSPI_B_TIME_ERASE_256K;
#else
    uint32_t sector_size = OSPI_B_SECTOR_4K_SIZE;
    uint32_t erase_timeout = OSPI_B_TIME_ERASE_4K;
#endif /* Specific board */

    if (!s_ospi_b_ctrl->open)
    {
        /* Open OSPI module */
        err = R_OSPI_B_Open(s_ospi_b_ctrl, &s_ospi_b_cfg);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    for (uint32_t erased_size = RESET_VALUE; erased_size < size; erased_size += sector_size)
    {
        err = R_OSPI_B_Erase(s_ospi_b_ctrl, p_current_address, sector_size);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        err = ospi_b_wait_operation(erase_timeout);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        p_current_address += sector_size;
    }

    return R_OSPI_B_Close(s_ospi_b_ctrl);
}
/***********************************************************************************************************************
* End of function erase_ospi_flash_range
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: ospi_b_setup_calibrate_data
 * Description  : This function sets up the auto-calibrate data for the flash.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Set up successfully.
 *                Any other error codes     Failed to set up.
 **********************************************************************************************************************/
static fsp_err_t ospi_b_setup_calibrate_data(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Calibration pattern data, which is set in both the OSPI_B Command Calibration Control register
     * and OSPI_B_AUTO_CALIB_ADDRESS. Refer to the OSPI_B_PRV_AUTOCALIBRATION_PREAMBLE_PATTERN in r_ospi_b.c */
#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
    uint32_t auto_calibration_data[] =
    {
        0xFFFF0000U,
        0x000800FFU,
        0x00FFF700U,
        0xF700F708U
    };
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
    uint32_t auto_calibration_data[4];
    uint32_t auto_calibration_data_spi[] =
    {
        0xFFFF0000U,
        0x0800FF00U,
        0xFF0000F7U,
        0x00F708F7U
    };
    uint32_t auto_calibration_data_opi[] =
    {
        0xFFFF0000U,
        0x000800FFU,
        0x00FFF700U,
        0xF700F708U
    };

    if (SPI_FLASH_PROTOCOL_EXTENDED_SPI == s_ospi_b_ctrl->spi_protocol)
    {
        memcpy(auto_calibration_data, auto_calibration_data_spi, sizeof(auto_calibration_data));
    }
    else
    {
        memcpy(auto_calibration_data, auto_calibration_data_opi, sizeof(auto_calibration_data));
    }
#endif /* Specific board */

    /* Verify auto-calibration data */
    if (RESET_VALUE != memcmp((uint8_t *)(OSPI_B_AUTO_CALIB_ADDRESS), &auto_calibration_data,
                              sizeof(auto_calibration_data)))
    {
        /* Erase the flash sector that stores auto-calibration data */
#if defined (BOARD_RA8D1_EK)
        err = R_OSPI_B_Erase(s_ospi_b_ctrl, (uint8_t *)OSPI_B_AUTO_CALIB_ADDRESS, OSPI_B_SECTOR_256K_SIZE);
#else
        err = R_OSPI_B_Erase(s_ospi_b_ctrl, (uint8_t *)OSPI_B_AUTO_CALIB_ADDRESS, OSPI_B_SECTOR_4K_SIZE);
#endif /* BOARD_RA8D1_EK */
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait until erase operation completes */
#if defined (BOARD_RA8D1_EK)
        err = ospi_b_wait_operation(OSPI_B_TIME_ERASE_256K);
#else
        err = ospi_b_wait_operation(OSPI_B_TIME_ERASE_4K);
#endif /* BOARD_RA8D1_EK */
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Write auto-calibration data to the flash */
        err = R_OSPI_B_Write(s_ospi_b_ctrl, (uint8_t *)&auto_calibration_data,
                             (uint8_t *)OSPI_B_AUTO_CALIB_ADDRESS, sizeof(auto_calibration_data));
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Wait until write operation completes */
        err = ospi_b_wait_operation(OSPI_B_TIME_WRITE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_setup_calibrate_data
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: ospi_b_set_protocol_to_opi
 * Description  : This function configures OSPI to OPI mode.
 * Arguments    : None.
 * Return Value : FSP_SUCCESS               Configured successfully.
 *                Any other error codes     Failed to configure.
 **********************************************************************************************************************/
static fsp_err_t ospi_b_set_protocol_to_opi(void)
{
    fsp_err_t err = FSP_SUCCESS;
    spi_flash_direct_transfer_t transfer =
    {
        .command        = RESET_VALUE,
        .address        = RESET_VALUE,
        .data           = RESET_VALUE,
        .command_length = RESET_VALUE,
        .address_length = RESET_VALUE,
        .data_length    = RESET_VALUE,
        .dummy_cycles   = RESET_VALUE
    };

    bsp_octaclk_settings_t octaclk =
    {
        .source_clock = RESET_VALUE,
        .divider      = RESET_VALUE
    };

    if (SPI_FLASH_PROTOCOL_8D_8D_8D == s_ospi_b_ctrl->spi_protocol)
    {
        /* Do nothing */
    }
    else if (SPI_FLASH_PROTOCOL_EXTENDED_SPI == s_ospi_b_ctrl->spi_protocol)
    {
        /* Transfer write enable command */
        err = ospi_b_write_enable();
        if (FSP_SUCCESS != err)
        {
            return err;
        }

#if defined (BOARD_RA8D1_EK)
        /* Write to CFR5V Register to Configure flash device interface mode */
        transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CFR5V_SPI];
        transfer.data = OSPI_B_DATA_SET_OPI_CFR5V_REGISTER;
        err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Change the OCTACLK clock to 200 MHz in DDR mode */
        octaclk.source_clock = BSP_CLOCKS_SOURCE_CLOCK_PLL2P;
        octaclk.divider      = BSP_CLOCKS_OCTA_CLOCK_DIV_2;
        R_BSP_OctaclkUpdate(&octaclk);

        /* Switch OSPI module mode to OPI mode */
        err = R_OSPI_B_SpiProtocolSet(s_ospi_b_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Read back and verify CFR5V register data */
        transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CFR5V_OPI];
        err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        if (OSPI_B_DATA_SET_OPI_CFR5V_REGISTER != (uint8_t)transfer.data)
        {
            return FSP_ERR_ABORTED;
        }
#elif defined (BOARD_RA8T2_EK) || defined (BOARD_RA8D2_EK)
        /* Change to OPI mode */
        transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR_00H_SPI];
        transfer.data = OSPI_B_DATA_SET_OPI_CR_00H;
        err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Change the OCTACLK clock to 266 MHz in DDR mode */
        octaclk.source_clock = BSP_CLOCKS_SOURCE_CLOCK_PLL2P;
        octaclk.divider      = BSP_CLOCKS_OCTA_CLOCK_DIV_1;
        R_BSP_OctaclkUpdate(&octaclk);

        /* Switch OSPI module mode to OPI mode */
        err = R_OSPI_B_SpiProtocolSet(s_ospi_b_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Read back and verify CR register data */
        transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR_00H_OPI];
        err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        if (OSPI_B_DATA_SET_OPI_CR_00H != (uint8_t)transfer.data)
        {
            return FSP_ERR_ABORTED;
        }
#elif defined (BOARD_RA8M2_EK) || defined (BOARD_RA8P1_EK)
        /* Write to CR2 Register to Configure flash device interface mode */
        transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_WRITE_CR2_000H_SPI];
        transfer.data = OSPI_B_DATA_SET_OPI_CR2_000H;
        err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Change the OCTACLK clock to 266 MHz in DDR mode with OM_DQS */
        octaclk.source_clock = BSP_CLOCKS_SOURCE_CLOCK_PLL2P;
        octaclk.divider      = BSP_CLOCKS_OCTA_CLOCK_DIV_1;
        R_BSP_OctaclkUpdate(&octaclk);

        /* Switch OSPI module mode to OPI mode */
        err = R_OSPI_B_SpiProtocolSet(s_ospi_b_ctrl, SPI_FLASH_PROTOCOL_8D_8D_8D);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        /* Read back and verify CR2 register data */
        transfer = s_ospi_b_direct_transfer[OSPI_B_TRANSFER_READ_CR2_000H_OPI];
        err = R_OSPI_B_DirectTransfer(s_ospi_b_ctrl, &transfer, SPI_FLASH_DIRECT_TRANSFER_DIR_READ);
        if (FSP_SUCCESS != err)
        {
            return err;
        }

        if (OSPI_B_DATA_SET_OPI_CR2_000H != (uint8_t)transfer.data)
        {
            return FSP_ERR_ABORTED;
        }
#endif /* Specific board */
    }
    else
    {
        return FSP_ERR_INVALID_MODE;
    }

    return err;
}
/***********************************************************************************************************************
* End of function ospi_b_set_protocol_to_opi
***********************************************************************************************************************/
