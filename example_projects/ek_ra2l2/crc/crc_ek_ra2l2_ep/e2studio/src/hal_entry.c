/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "crc_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup crc_ep
 * @{
 **********************************************************************************************************************/

/* Flags to indicate UART TX, RX events */
static volatile bool b_uart_rxflag  =  false;
static volatile bool b_uart_txflag  =  false;

/* For on board LEDs */
extern bsp_leds_t g_bsp_leds;

/* Private functions */
static fsp_err_t crc_operation(void);
static void set_led(bsp_io_level_t b_value);

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used. This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
	fsp_err_t err = FSP_SUCCESS;
	uint8_t rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};
	fsp_pack_version_t version      = {RESET_VALUE};

	/* Version get API for FLEX pack information */
	R_FSP_VersionGet(&version);

	/* Project information printed on the Console */
	APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
	            version.version_id_b.patch);
	APP_PRINT(EP_INFO);

	/* Open UART module */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_UART_Open(&g_uart_ctrl, &g_uart_cfg);
    /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT("R_SCI_B_UART_Open API FAILED \r\n");
            APP_ERR_TRAP(err);
        }
#elif (BSP_PERIPHERAL_SAU_PRESENT)
    err = R_SAU_UART_Open(&g_uart_ctrl, &g_uart_cfg);
    /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT("R_SAU_UART_Open API FAILED \r\n");
            APP_ERR_TRAP(err);
        }
#else
    err = R_SCI_UART_Open(&g_uart_ctrl, &g_uart_cfg);
    /* Handle error */
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT("R_SCI_UART_Open API FAILED \r\n");
            APP_ERR_TRAP(err);
        }
#endif

	/* Open CRC module */
	err = R_CRC_Open(&g_crc_ctrl, &g_crc_cfg);
	/* Handle error */
	if (FSP_SUCCESS != err)
	{
		/* Close opened UART module */
		deinit_uart();
		/* Display failure message in RTT */
		APP_ERR_PRINT ("R_CRC_Open API FAILED \r\n");
		APP_ERR_TRAP(err);
	}
	APP_PRINT("\r\nPress any key for CRC Operation");

	while(true)
	{
		memset(rByte,RESET_VALUE,BUFFER_SIZE_DOWN);
		/* If the user input from RTT is valid perform CRC operation */
		if (APP_CHECK_DATA)
		{
			APP_READ(rByte);
			/* Notify the user the start of CRC operation */
			APP_PRINT("\r\nStart CRC Operation\r\n");

			/* Perform CRC operation in normal and snoop mode */
			err = crc_operation();
			/* Handle error */
			if (FSP_SUCCESS != err)
			{
				/* Turn on LED as sign of CRC operation failure */
				set_led(LED_ON);

				/* Print RTT message */
				APP_ERR_PRINT ("\r\n ** CRC operation failed ** \r\n");
				/* Close all the opened modules before trap */
				cleanup();
				APP_ERR_TRAP(err);
			}
			APP_PRINT("\r\nPress any key for CRC Operation");
		}
	}
}

/*******************************************************************************************************************//**
 *  @brief      Performs CRC calculation in normal and snoop mode and toggle LED on successful operation
 *              else Turn LED ON on failure. Also displays failure messages in RTT.
 *  @param[IN]  None
 *  @retval     FSP_SUCCESS    On successfully CRC result in snoop and normal operation mode.
 *  @retval     err            Any Other Error code apart from FSP_SUCCESS  Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t crc_operation (void)
{
	fsp_err_t err = FSP_SUCCESS;
	/* CRC inputs structure */
	crc_input_t input_data;
	uint32_t normal_crc_value      = RESET_VALUE;               /* CRC value in normal mode */
	uint32_t uart_time_out         = UINT32_MAX;                /* Timeout value to check RX, TX events */
	uint8_t input_buffer[BUF_LEN]  = {0x05,0x02,0x03,0x04};     /* Source data */
	uint8_t dest_buffer[BUF_LEN]   = {RESET_VALUE};             /* Buffer to store UART read data */
	uint8_t  uart_data_len         = RESET_VALUE;               /* Data length for polynomial operation */

	/* Before beginning the operation turn OFF LED */
	set_led(LED_OFF);

	/* Clear callback event flag */
	b_uart_rxflag  = false;
	b_uart_txflag  = false;

	/* Update seed value and transfer uart_data_len as per the polynomial used */
	if(CRC_POLYNOMIAL_CRC_8 == g_crc_cfg.polynomial)
	{
		uart_data_len   = EIGHT_BIT_DATA_LEN;       /* Data length for 8 bit polynomial operation */
	}
	else if ((CRC_POLYNOMIAL_CRC_16 == g_crc_cfg.polynomial) || (CRC_POLYNOMIAL_CRC_CCITT == g_crc_cfg.polynomial))
	{
		uart_data_len   = SIXTEEN_BIT_DATA_LEN;     /* Data length for 16 bit polynomial operation */
	}
	else
	{
		/* Display failure message in RTT */
		APP_ERR_PRINT ("UNSUPPORTED CRC POLYNOMIAL SELECECTED \r\n");
		return FSP_ERR_UNSUPPORTED;
	}

	/* Update CRC input structure for normal mode */
	input_data.num_bytes      = NUM_BYTES;
	input_data.crc_seed       = SEED_VALUE;
	input_data.p_input_buffer = &input_buffer;

	/* Calculate CRC value for input data in Normal mode */
	err = R_CRC_Calculate(&g_crc_ctrl, &input_data , &normal_crc_value);
	if (FSP_SUCCESS != err)
	{
		/* Display failure message in RTT */
		APP_ERR_PRINT ("R_CRC_Calculate API FAILED \r\n");
		return err;
	}

	/* Append calculated CRC value from normal mode to input buffer */
	if (CRC_POLYNOMIAL_CRC_8 == g_crc_cfg.polynomial)
	{
		/* Append 8 bit CRC value to input data */
		input_buffer[4] = (uint8_t) normal_crc_value;
	}
	else if ((CRC_POLYNOMIAL_CRC_16 == g_crc_cfg.polynomial) || (CRC_POLYNOMIAL_CRC_CCITT == g_crc_cfg.polynomial))
	{
		/* Extract the bytes from 16 bit CRC value and append to input buffer as per the selected byte order
		 * in CRC configuration. */
		if(CRC_BIT_ORDER_LMS_LSB == g_crc_cfg.bit_order)
		{
			input_buffer[4] = (uint8_t) (normal_crc_value & 0xFF);       /* Extract first byte */
			input_buffer[5] = (uint8_t) ((normal_crc_value >>8) & 0xFF); /* Extract second byte */
		}
		else
		{
			input_buffer[5] = (uint8_t) (normal_crc_value & 0xFF);       /* Extract first byte */
			input_buffer[4] = (uint8_t) ((normal_crc_value >>8) & 0xFF); /* Extract second byte */
		}
	}
	else
	{
		/* Do nothing */
	}

	/* Board not support CRC snoop mode */
    #if (BSP_FEATURE_CRC_HAS_SNOOP == 0)
	    crc_input_t rx_input_data;
	    uint32_t rx_normal_crc_value       = RESET_VALUE; /* CRC value in Normal mode */

        /* Perform SCI UART (BOARD_RA4E2_EK, BOARD_RA6E2_EK, BOARD_RA4T1_MCK, BOARD_RA6T3_MCK, EK-RA4L1) loop-back
         * transmission from TX to RX */
	    /* Perform SAU UART (BOARD_RA0E1_FPB) loop-back transmission from TX to RX*/
        /* Perform UART write operation */

#if defined (BOARD_RA0E1_FPB)
        err =  R_SAU_UART_Write(&g_uart_ctrl,input_buffer,uart_data_len);
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_SAU_UART_Write API FAILED \r\n");
            return err;
        }

        /* Perform UART read operation */
        err =  R_SAU_UART_Read(&g_uart_ctrl,dest_buffer,uart_data_len);
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_SAU_UART_Read API FAILED \r\n");
            return err;
        }

#else
        err =  R_SCI_UART_Write(&g_uart_ctrl,input_buffer,uart_data_len);
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_SCI_UART_Write API FAILED \r\n");
            return err;
        }

        /* Perform UART read operation */
        err =  R_SCI_UART_Read(&g_uart_ctrl,dest_buffer,uart_data_len);
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_SCI_UART_Read API FAILED \r\n");
            return err;
        }

#endif

        /* Wait for TX and RX complete event */
        while((true != b_uart_txflag) || (true != b_uart_rxflag))
        {
            /* Start checking for timeout to avoid infinite loop */
            --uart_time_out;

            /* Check for time elapse */
            if (RESET_VALUE == uart_time_out)
            {
                /* We have reached to a scenario where UART TX and RX events not occurred */
                APP_ERR_PRINT (" ** UART TX and RX events not received during UART write or read operation **\r\r");
                return FSP_ERR_TIMEOUT;
            }
        }

        /* Update CRC input structure for normal mode */
        rx_input_data.num_bytes      = NUM_BYTES;
        rx_input_data.crc_seed       = SEED_VALUE;
        rx_input_data.p_input_buffer = &dest_buffer;

        /* Calculate CRC value for input data in normal mode */
        err = R_CRC_Calculate(&g_crc_ctrl, &rx_input_data , &rx_normal_crc_value);
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_CRC_Calculate API FAILED \r\n");
            return err;
        }

        /* Compare CRC for TX buffer and RX buffer*/
        if (normal_crc_value == rx_normal_crc_value)
        {
            APP_PRINT ("\r\nCRC Operation is successful.  \r\n");

            /* Compare UART write and read data buffer */
            if ( RESET_VALUE == memcmp(dest_buffer, input_buffer , uart_data_len))
            {
                APP_PRINT ("UART transmitted and received data successfully. \r\n");
                /* Toggle LED as sign of successful operation */
                toggle_led();
            }
        }
        else
        {
            APP_ERR_PRINT ("UART loop-back transmission error \r\n");
            return FSP_ERR_ABORTED;
        }
    #else
        uint32_t snoop_crc_value       = RESET_VALUE; /* CRC value in snoop mode */

        /* Board support CRC snoop address */
        /* Enable snoop mode */
        err = R_CRC_SnoopEnable(&g_crc_ctrl, SEED_VALUE);
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_CRC_SnoopEnable API FAILED \r\n");
            return err;
        }

        /* Perform SCI UART loop-back transmission from TX to RX*/
        /* Perform UART write operation */
#if  (BSP_FEATURE_SCI_VERSION == 2U )
        err =  R_SCI_B_UART_Write(&g_uart_ctrl,input_buffer,uart_data_len);

        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_SCI_B_UART_Write API FAILED \r\n");
            return err;
        }
#else
        err =  R_SCI_UART_Write(&g_uart_ctrl,input_buffer,uart_data_len);

        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_SCI_UART_Write API FAILED \r\n");
            return err;
        }
#endif

        /* Perform UART read operation */
#if  (BSP_FEATURE_SCI_VERSION == 2U )
        err =  R_SCI_B_UART_Read(&g_uart_ctrl,dest_buffer,uart_data_len);

        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_SCI_B_UART_Read API FAILED \r\n");
            return err;
        }

#else
        err =  R_SCI_UART_Read(&g_uart_ctrl,dest_buffer,uart_data_len);

        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_SCI_UART_Read API FAILED \r\n");
            return err;
        }
#endif
        /* Wait for TX and RX complete event */
        while((true != b_uart_txflag) || (true != b_uart_rxflag))
        {
            /* Start checking for timeout to avoid infinite loop */
            --uart_time_out;

            /* Check for time elapse */
            if (RESET_VALUE == uart_time_out)
            {
                /* We have reached to a scenario where UART TX and RX events not occurred */
                APP_ERR_PRINT (" ** UART TX and RX events not received during UART write or read operation **\r\r");
                return FSP_ERR_TIMEOUT;
            }
        }

        /* Get CRC value in snoop mode for receive data */
        err = R_CRC_CalculatedValueGet(&g_crc_ctrl, &snoop_crc_value);
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_CRC_CalculatedValueGet API FAILED \r\n");
            return err;
        }

        /* Disable snoop operation */
        err = R_CRC_SnoopDisable(&g_crc_ctrl);
        if (FSP_SUCCESS != err)
        {
            /* Display failure message in RTT */
            APP_ERR_PRINT ("R_CRC_SnoopDisable API FAILED \r\n");
            return err;
        }

        /* Validate the CRC results */
        if (RESET_VALUE == snoop_crc_value)
        {
            APP_PRINT ("\r\nCRC Operation is successful.  \r\n");

            /* Compare UART write and read data buffer */
            if ( RESET_VALUE == memcmp(dest_buffer, input_buffer , uart_data_len))
            {
                APP_PRINT ("UART transmitted and received data successfully. \r\n");
                /* Toggle LED as sign of successful operation */
                toggle_led();
            }
        }
        else
        {
            APP_ERR_PRINT ("UART loop-back transmission error \r\n");
            return FSP_ERR_ABORTED;
        }
    #endif
	return err;
}

/*******************************************************************************************************************//**
 *  @brief         The user defined SCI UART/ SAU UART driver callback function to indicate occurrence of RX, TX events
 *  @param[IN]     p_args
 *  @retval        None
 **********************************************************************************************************************/
void uart_cb(uart_callback_args_t *p_args)
{
	if (NULL != p_args)
	{
		switch(p_args->event)
		{
		case UART_EVENT_RX_COMPLETE:
			b_uart_rxflag = true;
			break;
		case UART_EVENT_TX_COMPLETE:
			b_uart_txflag = true;
			break;
		default:
			break;
		}
	}
}

/*******************************************************************************************************************//**
 * This function is called to do closing of CRC module using its HAL level API.
 * @brief     Close the CRC module. Handle the Error internally with Proper Message.
 *            Application handles the rest.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void deinit_crc(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Close opened CRC module */
	err = R_CRC_Close(&g_crc_ctrl);
	/* Handle error */
	if(FSP_SUCCESS != err)
	{
		APP_ERR_PRINT ("** R_CRC_Close API FAILED **\r\n");
	}
}

/*******************************************************************************************************************//**
 * This function is called to do closing of SCI UART/ SAU UART module using its HAL level API.
 * @brief     Close the SCI UART module. Handle the Error internally with Proper Message.
 *            Application handles the rest.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void deinit_uart(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Close opened External IRQ module */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
    err = R_SCI_B_UART_Close(&g_uart_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("** R_SCI_B_UART_Close API FAILED **\r\n");
    }

#elif (BSP_PERIPHERAL_SAU_PRESENT)
    err = R_SAU_UART_Close(&g_uart_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("** R_SAU_UART_Close API FAILED **\r\n");
    }

#else
    err = R_SCI_UART_Close(&g_uart_ctrl);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("** R_SCI_UART_Close API FAILED **\r\n");
    }

#endif
}

/*******************************************************************************************************************//**
 * @brief     This function is called to do closing of all opened module.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void cleanup(void)
{
	/* Close all the opened modules */
	deinit_crc();
	deinit_uart();
}

/*******************************************************************************************************************//**
 * @brief     Toggle on board LED, which are connected and supported by BSP
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void toggle_led(void)
{
	VALIDATE_IO_PORT_API(R_IOPORT_PinWrite(g_ioport.p_ctrl,(bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], LED_ON));
	R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
	VALIDATE_IO_PORT_API(R_IOPORT_PinWrite(g_ioport.p_ctrl,(bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], LED_OFF));
}

/*******************************************************************************************************************//**
 *  @brief       Turn on_board LED ON or OFF.
 *  @param[IN]   b_value     LED_ON or LED_OFF
 *  @retval      None
 **********************************************************************************************************************/
static void set_led(bsp_io_level_t b_value)
{
	VALIDATE_IO_PORT_API(R_IOPORT_PinWrite(g_ioport.p_ctrl,(bsp_io_port_pin_t)g_bsp_leds.p_leds[RESET_VALUE], b_value));
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process. This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
	if (BSP_WARM_START_POST_C == event) {
		/* C runtime environment and system clocks are setup */

		/* Configure pins */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}

/*******************************************************************************************************************//**
 * @} (end addtogroup crc_ep)
 **********************************************************************************************************************/
