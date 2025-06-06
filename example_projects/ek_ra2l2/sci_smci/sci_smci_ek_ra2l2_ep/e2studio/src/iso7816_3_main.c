/***********************************************************************************************************************
 * File Name    : iso7816_3_main.c
 * Description  : Contains data structures and functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/*
 * iso7816_3_activation.c
 *
 *  Created on: Jan 17, 2022
 *      Author: a5123412
 */

#include "common_utils.h"
#include "iso7816_3_main.h"
#include "iso7816_pps.h"
#include "iso7816_T0_protocol.h"
#include "user_config.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MICRO_SEC_PER_SEC                      (1000000)
#define SCI_SMCI_RX_SIZE                       (32)
#define MAX_ATR_TABLE_SIZE                     (32)

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/* Variable for recording SCI SMCI callback events */
volatile uint8_t g_received_smci_event = 0;

/* Variables for detecting card insertion */
static volatile bool smc_is_inserted = false;

/* Variables for monitoring wait time */
static volatile bool wt_is_reached = false;
static uint32_t pclkd_freq_hz = 0;
static uint32_t wait_period_count = 0;

/* Array and variable for ATR */
static uint8_t  smc_atr_table[MAX_ATR_TABLE_SIZE];
static uint8_t  smc_atr_table_pos = 0;
static uint8_t  next_td_position = 0;
static uint8_t  atr_interface_length = 0;
static uint8_t  atr_historical_length = 0;
static uint8_t  atr_length = 0;
static volatile uint8_t have_next_td = 0;
static smc_atr_message_t smc_atr_message;

/* SMCI Transfer Mode settings */
static smci_transfer_mode_t smc_transfer_mode;

/* SMCI driver specific information */
static     smci_state_t e_smci_state;

/* SCI SMCI baud setting updated */
static smci_baud_setting_t updated_smci_baud_setting;

/* ATS parameters mapping to the Fi, Di settings */
static const smci_clock_conversion_integer_t Ta1_Fi_lookup_table[16] =
{
    [0] = SMCI_CLOCK_CONVERSION_INTEGER_372_4,
    [1] = SMCI_CLOCK_CONVERSION_INTEGER_372_5,
    [2] = SMCI_CLOCK_CONVERSION_INTEGER_558_6,
    [3] = SMCI_CLOCK_CONVERSION_INTEGER_744_8,
    [4] = SMCI_CLOCK_CONVERSION_INTEGER_1116_12,
    [5] = SMCI_CLOCK_CONVERSION_INTEGER_1488_16,
    [6] = SMCI_CLOCK_CONVERSION_INTEGER_1860_20,
    [7] = SMCI_CLOCK_CONVERSION_INTEGER_UNSUPPORTED7,
    [8] = SMCI_CLOCK_CONVERSION_INTEGER_UNSUPPORTED8,
    [9] = SMCI_CLOCK_CONVERSION_INTEGER_512_5,
    [10] = SMCI_CLOCK_CONVERSION_INTEGER_768_75,
    [11] = SMCI_CLOCK_CONVERSION_INTEGER_1024_10,
    [12] = SMCI_CLOCK_CONVERSION_INTEGER_1536_15,
    [13] = SMCI_CLOCK_CONVERSION_INTEGER_2048_20,
    [14] = SMCI_CLOCK_CONVERSION_INTEGER_UNSUPPORTED14,
    [15] = SMCI_CLOCK_CONVERSION_INTEGER_UNSUPPORTED15,
};

static const uint8_t Ta1_Di_lookup_table[17] =
{
 [0] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_RFU0,  ///< RESERVED
 [1] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_1,  ///< Di=1
 [2] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_2,  ///< Di=2
 [3] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_4,  ///< Di=4
 [4] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_8,  ///< Di=8
 [5] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_16,  ///< Di=16
 [6] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_32,  ///< Di=32
 [7] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_64,  ///< Di=64
 [8] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_12,  ///< Di=12
 [9] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_20,  ///< Di=20
 [10] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_RFU10, ///< RESERVED
 [11] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_RFU11, ///< RESERVED
 [12] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_RFU12, ///< RESERVED
 [13] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_RFU13, ///< RESERVED
 [14] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_RFU14, ///< RESERVED
 [15] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_RFU15, ///< RESERVED
 [16] = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_MAX
};

/* Array for receiving character through user callback */
static uint8_t  rx_buffer[SCI_SMCI_RX_SIZE]={0};
static bool     RxIsData = false;
static uint16_t RxIndex = 0;

/* Smart Card status control */
static volatile smc_control_status_t smc_control_status;

/* Value of etu_in_microsend */
static uint32_t etu_in_us = 101;

/* TO parameters */
static smci_t0_parameters_t t0_parameters;

fsp_err_t Smart_Card_Interface_Init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize SCI in Smart Card mode */
    err = R_SCI_SMCI_Open(&g_smci0_ctrl, &g_smci0_cfg);
    if(err != FSP_SUCCESS)
    {
        return err;
    }

    /* Configure lower-level Timer */
    err = R_GPT_Open(&g_smci_etu_count_timer_ctrl, &g_smci_etu_count_timer_cfg);
    if(err != FSP_SUCCESS)
    {
        return err;
    }
    pclkd_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKD) >> g_smci_etu_count_timer_cfg.source_div;

#if CARD_DETECTION_PIN_IS_USED
    /* Configure lower-level External IRQ */
    err = R_ICU_ExternalIrqOpen(&g_ext_irq_smc_detect_ctrl, &g_ext_irq_smc_detect_cfg);
    if(err != FSP_SUCCESS)
    {
        return err;
    }

    err = R_ICU_ExternalIrqEnable(&g_ext_irq_smc_detect_ctrl);
    if(err != FSP_SUCCESS)
    {
        return err;
    }
#endif

    smc_control_status = SMC_DEVICE_NOT_READY;
    return err;
}

void user_smci_callback(smci_callback_args_t *p_args)
{
    smci_event_t event = (smci_event_t)p_args->event;
    uint8_t length_update = 0;

    g_received_smci_event |= (uint8_t)event;
    if(event == SMCI_EVENT_RX_CHAR)
    {
        if(RxIsData == true)
        {
            if(smc_control_status == SMC_DEVICE_ATR)
            {
                smc_atr_table[smc_atr_table_pos] = (uint8_t)p_args->data;
                if(smc_atr_table_pos == 1)
                {
                    /* Decode T0 to get the number of coming data bytes
                     * historical data length + interface data length */
                    atr_historical_length = (smc_atr_table[smc_atr_table_pos] & 0x0f);
                    atr_interface_length = (uint8_t)(((smc_atr_table[smc_atr_table_pos] & 0x80)>>7)+
                                                     ((smc_atr_table[smc_atr_table_pos] & 0x40)>>6)+
                                                     ((smc_atr_table[smc_atr_table_pos] & 0x20)>>5)+
                                                     ((smc_atr_table[smc_atr_table_pos] & 0x10)>>4));
                    have_next_td = ((smc_atr_table[smc_atr_table_pos] & 0x80)>>7);
                    next_td_position = smc_atr_table_pos + atr_interface_length;
                    atr_length += (uint8_t)((have_next_td == 1) ? \
                                  (uint8_t)(atr_historical_length + atr_interface_length + 2 + 1) : \
                                  (uint8_t)(atr_historical_length + atr_interface_length + 2));
                }

                if((have_next_td) && (smc_atr_table_pos == next_td_position))
                {
                    length_update = (uint8_t)(((smc_atr_table[smc_atr_table_pos] & 0x80)>>7)+
                                              ((smc_atr_table[smc_atr_table_pos] & 0x40)>>6)+
                                              ((smc_atr_table[smc_atr_table_pos] & 0x20)>>5)+
                                              ((smc_atr_table[smc_atr_table_pos] & 0x10)>>4));
                    have_next_td = ((smc_atr_table[smc_atr_table_pos] & 0x80)>>7);
                    next_td_position = smc_atr_table_pos + length_update;
                    atr_interface_length += length_update;
                    atr_length += length_update;
                }

                smc_atr_table_pos++;
                if(smc_atr_table_pos == atr_length)
                {
                    /* ATR reception is completed */
                    RxIsData = false;
                    RxIndex = 0;

                    g_received_smci_event |= (uint8_t)SMCI_EVENT_RX_COMPLETE;
                    g_received_smci_event &= (uint8_t)(~SMCI_EVENT_RX_CHAR);
                }
                smc_atr_table_pos = smc_atr_table_pos % MAX_ATR_TABLE_SIZE;
            }
            else if(smc_control_status == SMC_DEVICE_NEGOTIATION)
            {
                if(PPS_receive_character((uint8_t)p_args->data))
                {

                    g_received_smci_event |= (uint8_t)SMCI_EVENT_RX_COMPLETE;
                    g_received_smci_event &= (uint8_t)(~SMCI_EVENT_RX_CHAR);
                }
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            rx_buffer[RxIndex] = (uint8_t)p_args->data;
            RxIndex++;
            RxIndex = RxIndex % SCI_SMCI_RX_SIZE;

            if(smc_control_status == SMC_DEVICE_ATR)
            {
                if((rx_buffer[RxIndex-1] == ATS_TS_DIRECT) || (rx_buffer[RxIndex-1] == ATS_TS_INDIRECT))
                {
                    smc_atr_table[0] = rx_buffer[RxIndex-1];
                    smc_atr_table_pos = 1;
                    RxIsData = true;
                }
            }
        }
    }
}

void receiveExpireCallback(timer_callback_args_t *p_args)
{
    if(p_args->event == TIMER_EVENT_CYCLE_END)
    {
        R_GPT_Stop(&g_smci_etu_count_timer_ctrl);
        wt_is_reached = true;
    }
}

smc_control_status_t Smart_Card_Interface_Handler(smc_control_status_t set_smc_state)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t   event_flag = 0;

    do
    {
        switch (smc_control_status)
        {
            case SMC_DEVICE_NOT_READY:
            {
               Smart_Card_Interface_Card_Detection();
               break;
            }

            case SMC_DEVICE_COLD_RESET:
            {
                /* Enable VCC */
                R_IOPORT_PinWrite(&g_ioport_ctrl, SMCI_VCC, BSP_IO_LEVEL_HIGH);

                /* Output clock and enable receiver */
                R_SCI_SMCI_ClockControl(&g_smci0_ctrl, true);

                if(e_smci_state == SMCI_STATE_IDLE_CLOCK_OFF)
                {
                    /* Tb: wait at least 400 clock cycles. */
                    /* Calculate the desired period based on the current clock. Note that this calculation could
                     * overflow if the desired period is larger than UINT32_MAX / pclkd_freq_hz. A cast to uint64_t
                     * is used to prevent this */
                    waiting_timer_enable(MIN_COLD_RESET_WAIT_TIME);
                    while(!wt_is_reached);

                    R_IOPORT_PinWrite(&g_ioport_ctrl, SMCI_RST, BSP_IO_LEVEL_HIGH);
                    smc_control_status = SMC_DEVICE_ATR;
                }
                break;
            }

            case SMC_DEVICE_ATR:
            {
                APP_PRINT("\r\nStart ATR reception\r\n");
                memset(smc_atr_table, 0x00, sizeof(smc_atr_table));
                memset(&smc_atr_message, 0x00, sizeof(smc_atr_message_t));
                atr_interface_length = 0;
                atr_historical_length = 0;
                atr_length = 0;
                smc_atr_table_pos = 0;
                RxIsData = false;
                RxIndex = 0;

                /* Waiting for ATR message
                 * Because there is no specified read buffer provided via SCI_SMCI_Read(),
                 * the moving received byte data into user's buffer will be handled by user's callback */
                event_flag = smart_card_interface_event_notify(SMCI_EVENT_RX_COMPLETE, DEFAULT_ATR_WAIT_TIME);
                assert(0 != event_flag);

                /* Decode ATR message */
                smc_atr_message.TS = smc_atr_table[0];
                APP_PRINT("TS:0x%x\r\n", smc_atr_message.TS);
                smc_atr_message.T0 = smc_atr_table[1];
                APP_PRINT("T0:0x%x\r\n", smc_atr_message.T0);
                smc_atr_message.TCK = smc_atr_table[atr_length - 1];
                APP_PRINT("TCK:0x%x\r\n", smc_atr_message.TCK);
                smc_atr_message.Tlength = atr_interface_length;
                APP_PRINT("Historical Byte Length: %d\r\n", smc_atr_message.Tlength);
                smc_atr_message.Hlength = atr_historical_length;
                APP_PRINT("Interface Byte Length: %d\r\n", smc_atr_message.Hlength);

                /* Decode interface byte area
                 * starting position of ATR table array: 2
                 * ending position of ATR table array: atr_interface_length+2 */
                uint8_t start_post = 2;
                uint8_t i = 0;
                uint8_t TDi = smc_atr_message.T0;
                do
                {
                    if(TDi & 0x10)
                    {
                        smc_atr_message.Ta[i] = smc_atr_table[start_post++];
                        APP_PRINT("TA%d:0x%x, ", (i+1), smc_atr_message.Ta[i]);
                        atr_interface_length--;
                    }

                    if(TDi & 0x20)
                    {
                        smc_atr_message.Tb[i] = smc_atr_table[start_post++];
                        APP_PRINT("TB%d:0x%x, ", (i+1), smc_atr_message.Tb[i]);
                        atr_interface_length--;
                    }

                    if(TDi & 0x40)
                    {
                        smc_atr_message.Tc[i] = smc_atr_table[start_post++];
                        APP_PRINT("TC%d:0x%x, ", (i+1), smc_atr_message.Tc[i]);
                        atr_interface_length--;
                    }

                    if(TDi & 0x80)
                    {
                        smc_atr_message.Td[i] = smc_atr_table[start_post++];
                        APP_PRINT("TD%d:0x%x", (i+1), smc_atr_message.Td[i]);
                        TDi = smc_atr_message.Td[i];
                        atr_interface_length--;
                    }
                    else
                    {
                        TDi = 0;
                    }

                    APP_PRINT("\r\n");
                    i++;
                }while((atr_interface_length>0) || (TDi != 0));

                APP_PRINT("Historical Bytes: ");
                for (uint8_t j = 0; j < smc_atr_message.Hlength; j++)
                {
                    smc_atr_message.H[j] = smc_atr_table[j + (uint8_t)2 + smc_atr_message.Tlength];
                    APP_PRINT("0x%x ", smc_atr_message.H[j]);
                }

                APP_PRINT("\r\n");

                if(smc_atr_message.Td[0] != 0)
                {
                    smc_atr_message.protocol_type = (smc_atr_message.Td[0] & 0x0f);
                }
                else
                {
                    smc_atr_message.protocol_type = SMCI_PROTOCOL_TYPE_T0;
                }
                APP_PRINT("Smart Card Selected Protocol Type:%d\r\n", smc_atr_message.protocol_type);

                if(smc_atr_message.TS == ATS_TS_DIRECT)
                {
                    smc_atr_message.data_convention_type = SMCI_CONVENTION_TYPE_DIRECT;
                }
                else if(smc_atr_message.TS == ATS_TS_INDIRECT)
                {
                    smc_atr_message.data_convention_type = SMCI_CONVENTION_TYPE_INVERSE;
                }
                else
                {
                    /* Do nothing */
                }

                /* Update the SCI SMCI transfer mode and data convention type */
                R_SCI_SMCI_TransferModeSet(&g_smci0_ctrl, &smc_transfer_mode);

                smc_control_status = SMC_DEVICE_NEGOTIATION;
                break;
            }

            case SMC_DEVICE_NEGOTIATION:
            {
                RxIsData = true;
                RxIndex = 0;
                err = Smart_Card_Interface_F_D_confirmation(&smc_atr_message);
                assert(FSP_SUCCESS == err);
                if(smc_atr_message.protocol_type == SMCI_PROTOCOL_TYPE_T0)
                {
                    ISO7816_T0_SET_PARAMETERS(&t0_parameters, &smc_atr_message, etu_in_us);
                }
                else if(smc_atr_message.protocol_type == SMCI_PROTOCOL_TYPE_T1)
                {
                    /* Area reserved */
                }
                smc_control_status = SMC_DEVICE_ACTIVE;
                break;
            }

            case SMC_DEVICE_ACTIVE:
            {
                if(set_smc_state != REGULAR_POOLING)
                {
                    smc_control_status = set_smc_state;
                }
                break;
            }

            case SMC_DEVICE_DEACTIVE:
            {
                if(!smc_is_inserted)
                {
                    APP_PRINT("\r\nSmart Card device is removed.\r\n");
                }
				else
                {
                    APP_PRINT("Proceed de-activation\r\n");
                }
                
                /* Pull RST to low state */
                R_IOPORT_PinWrite(&g_ioport_ctrl, SMCI_RST, BSP_IO_LEVEL_LOW);

                /* Stop clock output */
                R_SCI_SMCI_ClockControl(&g_smci0_ctrl, false);

                /* Deactivate the VCC */
                R_IOPORT_PinWrite(&g_ioport_ctrl, SMCI_VCC, BSP_IO_LEVEL_LOW);

                smc_control_status = SMC_DEVICE_NOT_READY;
                break;
            }

            default:
                break;
        }
    }while((smc_control_status != set_smc_state) &&
           (smc_control_status != SMC_DEVICE_ACTIVE) &&
           (set_smc_state != REGULAR_POOLING));

    return smc_control_status;
}

void waiting_timer_enable(uint32_t etu_num)
{
    fsp_err_t err = FSP_SUCCESS;

    wt_is_reached = false;
    wait_period_count = (uint32_t) (((uint64_t) pclkd_freq_hz * etu_in_us * etu_num) / MICRO_SEC_PER_SEC);
    R_GPT_Reset(&g_smci_etu_count_timer_ctrl);
    /* Set the calculated period. */
    err = R_GPT_PeriodSet(&g_smci_etu_count_timer_ctrl, wait_period_count);
    assert(FSP_SUCCESS == err);
    err = R_GPT_Start(&g_smci_etu_count_timer_ctrl);
    assert(FSP_SUCCESS == err);
}

void smart_card_detection_cb(external_irq_callback_args_t *p_args)
{
    bsp_io_level_t current_state;
    (void) p_args;

    R_IOPORT_PinRead(&g_ioport_ctrl, SMCI_DETECT, &current_state);
    if(current_state == BSP_IO_LEVEL_LOW)
    {
        smc_is_inserted = false;
        smc_control_status = SMC_DEVICE_DEACTIVE;
        /* Disable VCC */
        R_IOPORT_PinWrite(&g_ioport_ctrl, SMCI_VCC, BSP_IO_LEVEL_LOW);
    }
}

fsp_err_t Smart_Card_Interface_Card_Detection(void)
{
#if CARD_DETECTION_PIN_IS_USED
    uint32_t counter = 0;
    bsp_io_level_t current_state;

    do{
        current_state = \
                (bsp_io_level_t) R_PFS->PORT[SMCI_DETECT >> 8].PIN[SMCI_DETECT & BSP_IO_PRV_8BIT_MASK].PmnPFS_b.PIDR;
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS);
    }while((current_state != BSP_IO_LEVEL_HIGH) && (++counter < 0xffffffff));

    if(current_state == BSP_IO_LEVEL_HIGH)
    {
        smc_is_inserted = true;
        smc_control_status = SMC_DEVICE_COLD_RESET;
        APP_PRINT("\r\nSmart Card device is ready.\r\n");
        return FSP_SUCCESS;
    }
    else
    {
        return FSP_ERR_TIMEOUT;
    }
#else
    smc_control_status = SMC_DEVICE_COLD_RESET;
    APP_PRINT("\r\nSmart Card device is ready.\r\n");
    return FSP_SUCCESS;
#endif
}

uint8_t smart_card_interface_event_notify(uint8_t set_event_flag_value, uint32_t timout_etu)
{
    uint8_t        get_event_flag_value = 0;

    if(timout_etu != 0)
    {
        waiting_timer_enable(timout_etu);
    }

    do
    {
        get_event_flag_value = (set_event_flag_value & g_received_smci_event);
        if(get_event_flag_value)
        {
            g_received_smci_event &= (uint8_t)(~set_event_flag_value);
            waiting_timer_disable();
            return get_event_flag_value;
        }
    }while ((!wt_is_reached)&&(timout_etu));

    return 0;
}

void waiting_timer_disable(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_GPT_Stop(&g_smci_etu_count_timer_ctrl);
    assert(err == FSP_SUCCESS);
}

/***********************************************************************************************************************
 * @brief  Set the smart card baud rate function of F and D parameters and apply PPS procedure.
 * @param  SC_atr: a pointer to the ATR structure.
 * @param  SC_clk: the smart card clock frequency in Hz.
 * @retval The etu value in us.
 **********************************************************************************************************************/
fsp_err_t Smart_Card_Interface_F_D_confirmation(smc_atr_message_t * sc_returned_atr)
{

    smci_speed_params_t smci_speed_params = {
                                             .baudrate  = 10000,
                                             .di = SMCI_BAUDRATE_ADJUSTMENT_INTEGER_1,
                                             .fi = SMCI_CLOCK_CONVERSION_INTEGER_512_5,
                                            };

    fsp_err_t  err = FSP_SUCCESS;
    uint8_t    Fi = 6U;
    uint8_t    Di = 1;
    uint8_t    PPS_buffer[4] = {0xFF, 0x11, 0x00, 0xEE};
    uint8_t    PPS_Response_length = 0, pps1 = 0;
    uint8_t    TA2=0;

    /* Check TD1 to look for TA2 */
    if(sc_returned_atr->Td[0] & 0x10)
    {
        /* When TA2 is present, the SmartCard is in specific mode */
        TA2 = sc_returned_atr->Ta[1];
        /* TA2 bit5 = 0 : the integers Fi and Di defined by TA1 should apply */
        if((TA2 & 0x10) == 0)
        {
            Fi = Ta1_Fi_lookup_table[ (sc_returned_atr->Ta[0] & 0xf0) >> 4 ];
            Di = Ta1_Di_lookup_table[ ( sc_returned_atr->Ta[0] & 0x0f ) ];
            /* Calculates baud rate register settings. */
            err = R_SCI_SMCI_BaudCalculate(&smci_speed_params, SCI_SMCI_BAUDRATE_ERROR_PERCENT_MAX,
                                           &updated_smci_baud_setting);
            assert(err == FSP_SUCCESS);

            err = R_SCI_SMCI_BaudSet(&g_smci0_ctrl, &updated_smci_baud_setting);
            assert(err == FSP_SUCCESS);
        }
    }
    else
    {
        /* When TA2 is absent, the SmartCard is in negotiable mode */
        /* Then, check TS0 to look for TA1 */
        if(sc_returned_atr->T0 & 0x10)
        {
            /* Extract the FI and DI */
            Fi = Ta1_Fi_lookup_table[ (sc_returned_atr->Ta[0] & 0xf0) >> 4 ];
            Di = Ta1_Di_lookup_table[ ( sc_returned_atr->Ta[0] & 0x0f ) ];
            if((Fi != smci_speed_params.fi)||(Di != smci_speed_params.di))
            {
                APP_PRINT("TA1 presents and the card supported asynchronous parameters are different to "\
                          "the interface used\r\n");
                /* If the card supports values different from default values */
                /* Fill up PPS1 encoded in the same way as in TA1 */
                PPS_buffer[PPS0] = (0x10 | sc_returned_atr->protocol_type); /* 0x10 : only PPS1 present */
                PPS_buffer[PPS1] = sc_returned_atr->Ta[0];
                PPS_buffer[3] = PPS_GetPCK(PPS_buffer, 3);

                /* Apply PPS (Protocol Parameters Selection) */
                err = PPS_Exchange (PPS_buffer, &PPS_Response_length, &pps1);
                assert(err == FSP_SUCCESS);

                APP_PRINT("Complete PPS exchange\r\n");

                if(pps1 != 0)
                {
                    APP_PRINT("Use updated asynchronous parameters to update the baud rate setting\r\n");
                    Fi = Ta1_Fi_lookup_table[ (pps1 & 0xf0) >> 4 ];
                    Di = Ta1_Di_lookup_table[ ( pps1 & 0x0f ) ];
                }

                /* Calculates baud rate register settings. */
                err = R_SCI_SMCI_BaudCalculate(&smci_speed_params, SCI_SMCI_BAUDRATE_ERROR_PERCENT_MAX,
                                               &updated_smci_baud_setting);
                assert(err == FSP_SUCCESS);

                err = R_SCI_SMCI_BaudSet(&g_smci0_ctrl, &updated_smci_baud_setting);
                assert(err == FSP_SUCCESS);
            }
        }
        else
        {
        /* ---TA1 is absent (use of default values F=372 and D=1)--- */
        /* ---SmartCard still having the same baud rate--- */
        }
    }

    return err;
}
