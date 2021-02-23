
#include "hal_data.h"
#include "app_common.h"

volatile bool g_button_s1_pushed;
volatile bool g_button_s2_pushed;

volatile bool g_sci0_uart_transmit_complete_flag;
volatile bool g_sci0_uart_recv_flag;
uint16_t recv_count = 0;


static volatile bool wakeup_request_flag = CLEAR_FLAG;

static uint8_t data_request_event = 0; // 0: none, 1:user switch s1, 2: user switch s2,
                                       // 3: data full, 4: level judgment (ch00), 5: level judgment (ch01)

static uint8_t send_data;
static uint8_t recv_data;
static uint8_t recv_buff[3000];

const uint8_t recv_data_correct_wakeup[] = "^40001 $";

static bsp_io_level_t pin_data;
static uint8_t pin_pullup_check;

static uint16_t rtt_read_point;

static uint8_t rtt_data_size_ASCII[5] = {0};
static uint8_t rtt_nData_ASCII[3] = {0};
static uint8_t rtt_time_ASCII[11] = {0};
static uint8_t rtt_light_ASCII[5] = {0};
static uint8_t rtt_temp_ASCII[3] = {0};

static uint8_t rtt_attribute;
static uint16_t rtt_data_size;
static uint32_t rtt_time;
static uint16_t rtt_light_ave, rtt_light_min, rtt_light_max;
static uint8_t rtt_temp_ave_u, rtt_temp_ave_l;
static uint8_t rtt_temp_min_u, rtt_temp_min_l;
static uint8_t rtt_temp_max_u, rtt_temp_max_l;

void app_main(void);
void pin_state_change(bsp_io_port_pin_t pin, bsp_io_level_t state);
void pin_state_check(void);
void wakeup_pin_state_change(void);

void app_main(void){
    fsp_err_t err = FSP_SUCCESS;

    /* Power on message */
    APP_PRINT("\n\n\n\n\n");
    APP_PRINT("/**********************************************************/\n");
    APP_PRINT("/******* APN : Low Power Data Logger (Main Module) ********/\n");
    APP_PRINT("/**********************************************************/\n\n");

    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

    /* Initialize module */
    err |= R_ICU_ExternalIrqOpen(&g_external_irq13_button_s1_ctrl, &g_external_irq13_button_s1_cfg);
    err |= R_ICU_ExternalIrqOpen(&g_external_irq12_button_s2_ctrl, &g_external_irq12_button_s2_cfg);
    err |= R_ICU_ExternalIrqEnable(&g_external_irq13_button_s1_ctrl);
    err |= R_ICU_ExternalIrqEnable(&g_external_irq12_button_s2_ctrl);
    if(FSP_SUCCESS != err) __BKPT(0);

    err |= R_SCI_UART_Open(&g_sci8_uart_ctrl, &g_sci8_uart_cfg);
    if(FSP_SUCCESS != err) __BKPT(0);

    while(1){
        /* User button s1 event */
        if(g_button_s1_pushed){
            g_button_s1_pushed = false;

            /* Check the S1 state again for chattering elimination */
            R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
            R_IOPORT_PinRead(&g_ioport_ctrl, USER_BUTTON_S1, &pin_data);
            if(pin_data == PIN_LOW__LED_OFF){
                wakeup_request_flag = SET_FLAG;
                data_request_event = 1;
            }
        }

        /* User button s2 event */
        if(g_button_s2_pushed){
            g_button_s2_pushed = false;

            /* Check the S2 state again for chattering elimination */
            R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
            R_IOPORT_PinRead(&g_ioport_ctrl, USER_BUTTON_S2, &pin_data);
            if(pin_data == PIN_LOW__LED_OFF){
                wakeup_request_flag = SET_FLAG;
                data_request_event = 2;
            }
        }

        /* Data full event */
        R_IOPORT_PinRead(&g_ioport_ctrl, DATAFULL_PIN, &pin_data);
        if(pin_data == PIN_HIGH__LED_ON){
            wakeup_request_flag = SET_FLAG;

            pin_pullup_check++;
            data_request_event = 3;
        }

        /* Level judgment result (ch00) event */
        R_IOPORT_PinRead(&g_ioport_ctrl, ANALOG_LEVEL_CH00_PIN, &pin_data);
        if(pin_data == PIN_HIGH__LED_ON){
            wakeup_request_flag = SET_FLAG;

            pin_pullup_check++;
            data_request_event = 4;
        }

        /* Level judgment result (ch01) event */
        R_IOPORT_PinRead(&g_ioport_ctrl, ANALOG_LEVEL_CH01_PIN, &pin_data);
        if(pin_data == PIN_HIGH__LED_ON){
            wakeup_request_flag = SET_FLAG;

            pin_pullup_check++;
            data_request_event = 5;
        }

        /* Check of Sub module power on */
        if(pin_pullup_check == 3){
            wakeup_request_flag = CLEAR_FLAG;
        }else{
            /* Output an information of the data request event to RTT Viewer */
            switch(data_request_event){
                case 1:
                    APP_PRINT("\nEvent: button S1\n");
                    break;
                case 2:
                    APP_PRINT("\nEvent: button S2\n");
                    break;
                case 3:
                    APP_PRINT("\nEvent: Data full\n");
                    break;
                case 4:
#ifndef DISABLE_SENSOR_DEPENDENCE
                    APP_PRINT("\nEvent: Analog level Channel 0 (Light)\n");
#else
                    APP_PRINT("\nEvent: Analog level Channel 0\n");
#endif
                    break;
                case 5:
#ifndef DISABLE_SENSOR_DEPENDENCE
                    APP_PRINT("\nEvent: Analog level Channel 1 (Temp)\n");
#else
                    APP_PRINT("\nEvent: Analog level Channel 1\n");
#endif
                    break;
                default:
                    break;
            }
        }
        pin_pullup_check = 0;

        /* Wait for RTC Alarm setting complication of sub module */
        R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

        /* Change the wakeup pin state */
        wakeup_pin_state_change();

        /* Check the pin status */
        pin_state_check();

        if(wakeup_request_flag){

            /* Receive Wake up complete message */
            /* Read setting */
            recv_count = 0;
            R_SCI_UART_Read(&g_sci8_uart_ctrl, &recv_data, 1);
            /* Wait for data end code */
            while(g_sci0_uart_recv_flag == CLEAR_FLAG){
                pin_state_check();
                if(recv_count == 0){
                    if(g_button_s1_pushed){
                        g_button_s1_pushed = false;

                        /* Check the S1 state again for chattering elimination */
                        R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
                        R_IOPORT_PinRead(&g_ioport_ctrl, USER_BUTTON_S1, &pin_data);
                        if(pin_data == PIN_LOW__LED_OFF){
                            wakeup_request_flag = CLEAR_FLAG;
                            wakeup_pin_state_change();
                            R_SCI_UART_Abort(&g_sci8_uart_ctrl, UART_DIR_RX);
                            break;
                        }
                    }
                    if(g_button_s2_pushed){
                        g_button_s2_pushed = false;

                        /* Check the S1 state again for chattering elimination */
                        R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
                        R_IOPORT_PinRead(&g_ioport_ctrl, USER_BUTTON_S2, &pin_data);
                        if(pin_data == PIN_LOW__LED_OFF){
                            wakeup_request_flag = CLEAR_FLAG;
                            wakeup_pin_state_change();
                            R_SCI_UART_Abort(&g_sci8_uart_ctrl, UART_DIR_RX);
                            break;
                        }
                    }
                }
            }

            if(g_sci0_uart_recv_flag == SET_FLAG){
                g_sci0_uart_recv_flag = CLEAR_FLAG;

                if(memcmp(recv_buff, recv_data_correct_wakeup, 8) == 0){
                    APP_PRINT("    Wakeup message check : OK\n");

                    /* Create a command data */
                    switch(data_request_event){
                        case 1:
                            send_data = '1';
                            break;
                        case 2:
                        case 3:
                            send_data = '2';
                            break;
                        case 4:
                        case 5:
                            send_data = '3';
                            break;
                        default:
                            break;
                    }

                    /* Send command */
                    err = R_SCI_UART_Write(&g_sci8_uart_ctrl, &send_data, 1);
                    while(!g_sci0_uart_transmit_complete_flag);
                    g_sci0_uart_transmit_complete_flag = CLEAR_FLAG;

                    /* Receive the data */
                    /* Read setting */
                    recv_count = 0;
                    err = R_SCI_UART_Read(&g_sci8_uart_ctrl, &recv_data, 1);

                    /* Wait for data end code */
                    while(g_sci0_uart_recv_flag == CLEAR_FLAG){
                        pin_state_check();
                        if(recv_count == 0){
                            if(g_button_s1_pushed){
                                g_button_s1_pushed = false;

                                /* Check the S1 state again for chattering elimination */
                                R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
                                R_IOPORT_PinRead(&g_ioport_ctrl, USER_BUTTON_S1, &pin_data);
                                if(pin_data == PIN_LOW__LED_OFF){
                                    wakeup_request_flag = CLEAR_FLAG;
                                    wakeup_pin_state_change();
                                    R_SCI_UART_Abort(&g_sci8_uart_ctrl, UART_DIR_RX);
                                    break;
                                }
                            }
                            if(g_button_s2_pushed){
                                g_button_s2_pushed = false;

                                /* Check the S1 state again for chattering elimination */
                                R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
                                R_IOPORT_PinRead(&g_ioport_ctrl, USER_BUTTON_S2, &pin_data);
                                if(pin_data == PIN_LOW__LED_OFF){
                                    wakeup_request_flag = CLEAR_FLAG;
                                    wakeup_pin_state_change();
                                    R_SCI_UART_Abort(&g_sci8_uart_ctrl, UART_DIR_RX);
                                    break;
                                }
                            }
                        }
                    }

                    if(g_sci0_uart_recv_flag == SET_FLAG){
                        g_sci0_uart_recv_flag = CLEAR_FLAG;

                        /* Change the wakeup pin state */
                        wakeup_request_flag = CLEAR_FLAG;
                        wakeup_pin_state_change();

                        /* Check the pin status */
                        pin_state_check();

                        // attribute
                        APP_PRINT("    Attribute            : %c\n", recv_buff[1]);
                        rtt_attribute = (uint8_t)(recv_buff[1] - 0x30);

                        // data size
                        memcpy(rtt_data_size_ASCII, (recv_buff + 2), 4);
                        rtt_data_size = (uint16_t)atoi((char *)rtt_data_size_ASCII);
                        APP_PRINT("    Data size            : %d [bytes]\n", rtt_data_size);

                        if(recv_count == (rtt_data_size + PACKET_FORMAT_SIZE_START_CODE + PACKET_FORMAT_SIZE_ATTRIBUTE_CODE
                                + PACKET_FORMAT_SIZE_DATA_LENGTH_CODE + PACKET_FORMAT_SIZE_END_CODE)){
                            APP_PRINT("    Data size check      : OK\n");
                        }

                        rtt_read_point = 0;
                        switch(rtt_attribute){
                            case 1:
                            case 2:

                                // Number of data
                                memcpy(rtt_nData_ASCII, (recv_buff + 6), 2);
                                uint8_t rtt_nData = (uint8_t)atoi((char *)rtt_nData_ASCII);
                                APP_PRINT("    Data num             : %d\n", rtt_nData);

                                APP_PRINT("    Data : ");
                                if(rtt_nData == 0){
                                    APP_PRINT("None\n");
                                }else{
#ifndef DISABLE_SENSOR_DEPENDENCE
                                    APP_PRINT("| Time [sec] |   Light [Lux]  |      Temp [C]     |\n");
#else
                                    APP_PRINT("| Time [sec] |    Channel 0   |     Channel 1     |\n");
#endif
                                    APP_PRINT("           |            |  ave  min  max |  ave   min   max  |\n");
                                }
                                rtt_read_point = 8;

                                // data
                                while(1){
                                    if(recv_buff[rtt_read_point] == '/'){
                                        rtt_read_point++;

                                        if(rtt_attribute == 1){
                                            // time
                                            memcpy(rtt_time_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TIME);
                                            rtt_time = (uint32_t)atoi((char *)rtt_time_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_TIME + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                            // light average
                                            memcpy(rtt_light_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_LIGHT);
                                            rtt_light_ave = (uint16_t)atoi((char *)rtt_light_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_LIGHT + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                            // temp average upper
                                            memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                            rtt_temp_ave_u = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + PACKET_FORMAT_SIZE_DATA_TEMP_A_U);

                                            // temp average lower
                                            memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                            rtt_temp_ave_l = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
#ifndef DISABLE_SENSOR_DEPENDENCE
                                            APP_PRINT("           | %10d | %4d           | %2d.%02d             |\n", rtt_time, rtt_light_ave, rtt_temp_ave_u, rtt_temp_ave_l);
#else
                                            APP_PRINT("           | %10d | %4d           | %2d%02d              |\n", rtt_time, rtt_light_ave, rtt_temp_ave_u, rtt_temp_ave_l);
#endif

                                        }else if(rtt_attribute == 2){
                                            // time
                                            memcpy(rtt_time_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TIME);
                                            rtt_time = (uint32_t)atoi((char *)rtt_time_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_TIME + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                            // light average
                                            memcpy(rtt_light_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_LIGHT);
                                            rtt_light_ave = (uint16_t)atoi((char *)rtt_light_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_LIGHT + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                            // light minimum
                                            memcpy(rtt_light_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_LIGHT);
                                            rtt_light_min = (uint16_t)atoi((char *)rtt_light_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_LIGHT + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                            // light maximum
                                            memcpy(rtt_light_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_LIGHT);
                                            rtt_light_max = (uint16_t)atoi((char *)rtt_light_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_LIGHT + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                            // temp average upper
                                            memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                            rtt_temp_ave_u = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + PACKET_FORMAT_SIZE_DATA_TEMP_A_U);

                                            // temp average lower
                                            memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                            rtt_temp_ave_l = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_TEMP_A_U + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                            // temp minimum upper
                                            memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                            rtt_temp_min_u = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + PACKET_FORMAT_SIZE_DATA_TEMP_A_U);

                                            // temp minimum lower
                                            memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                            rtt_temp_min_l = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_TEMP_A_U + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                            // temp maximum upper
                                            memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                            rtt_temp_max_u = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + PACKET_FORMAT_SIZE_DATA_TEMP_A_U);

                                            // temp maximum lower
                                            memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                            rtt_temp_max_l = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                            rtt_read_point = (uint16_t)(rtt_read_point + PACKET_FORMAT_SIZE_DATA_TEMP_A_U);

#ifndef DISABLE_SENSOR_DEPENDENCE
                                            APP_PRINT("           | %10d | %4d %4d %4d | %2d.%02d %2d.%02d %2d.%02d |\n", rtt_time,
                                                    rtt_light_ave, rtt_light_min, rtt_light_max, rtt_temp_ave_u, rtt_temp_ave_l, rtt_temp_min_u, rtt_temp_min_l, rtt_temp_max_u, rtt_temp_max_l);
#else
                                            APP_PRINT("           | %10d | %4d %4d %4d | %2d%02d  %2d%02d  %2d%02d  |\n", rtt_time,
                                                    rtt_light_ave, rtt_light_min, rtt_light_max, rtt_temp_ave_u, rtt_temp_ave_l, rtt_temp_min_u, rtt_temp_min_l, rtt_temp_max_u, rtt_temp_max_l);
#endif

                                            R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);
                                        }else{
                                        }

                                    }
                                    else if(recv_buff[rtt_read_point] == '$'){
                                        break;
                                    }
                                    else{
                                        APP_PRINT("!!! PACKET ERROR !!!");
                                        break;
                                    }
                                }

                                break;

                            case 3:
                                rtt_read_point = 6;

                                // time
                                memcpy(rtt_time_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TIME);
                                rtt_time = (uint32_t)atoi((char *)rtt_time_ASCII);
                                rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_TIME + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                // light average
                                memcpy(rtt_light_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_LIGHT);
                                rtt_light_ave = (uint16_t)atoi((char *)rtt_light_ASCII);
                                rtt_read_point = (uint16_t)(rtt_read_point + (PACKET_FORMAT_SIZE_DATA_LIGHT + PACKET_FORMAT_SIZE_DELIMITER_CODE));

                                // temp average upper
                                memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                rtt_temp_ave_u = (uint8_t)atoi((char *)rtt_temp_ASCII);
                                rtt_read_point = (uint16_t)(rtt_read_point + PACKET_FORMAT_SIZE_DATA_TEMP_A_U);

                                // temp average lower
                                memcpy(rtt_temp_ASCII, (recv_buff + rtt_read_point), PACKET_FORMAT_SIZE_DATA_TEMP_A_U);
                                rtt_temp_ave_l = (uint8_t)atoi((char *)rtt_temp_ASCII);

#ifndef DISABLE_SENSOR_DEPENDENCE
                                APP_PRINT("    Data :  Time   %d [sec]\n", rtt_time);
                                APP_PRINT("            Light  %d [Lux]\n", rtt_light_ave);
                                APP_PRINT("            Temp   %d.%02d [C]\n", rtt_temp_ave_u, rtt_temp_ave_l);
#else
                                APP_PRINT("    Data :  Time       %d \n", rtt_time);
                                APP_PRINT("            Channel 0  %d \n", rtt_light_ave);
                                APP_PRINT("            Channel 1  %d%02d \n", rtt_temp_ave_u, rtt_temp_ave_l);
#endif

                                break;

                            case 4:
                                APP_PRINT("\n !!! SEQUENCE ERROR : Received wake up message again. !!! \n");
                                break;

                            case 5:
                                APP_PRINT("\n *** ERROR MESSAGE : Received command error. *** \n");
                                break;

                            default:
                                APP_PRINT("\n !!! PACKET  ERROR !!! \n");
                                break;

                        }
                    }
                }else{
                    APP_PRINT("\n !!! WAKEUP MESSAGE ERROR !!! \n");
                }
            }
        }
        data_request_event = 0;
    }
}

void wakeup_pin_state_change(void){
    /* Update the wakeup pin state */
    if(wakeup_request_flag){
        pin_state_change(WAKE_UP_PIN, PIN_HIGH__LED_ON);
        pin_state_change(LED1_BLUE, PIN_HIGH__LED_ON);

    }else{
        pin_state_change(WAKE_UP_PIN, PIN_LOW__LED_OFF);
        pin_state_change(LED1_BLUE, PIN_LOW__LED_OFF);
    }
}

void pin_state_change(bsp_io_port_pin_t pin, bsp_io_level_t state)
{
    /* Enable pin access */
    R_BSP_PinAccessEnable();

    /* Change the state */
    if(state == PIN_HIGH__LED_ON)
    {
        R_BSP_PinWrite(pin, BSP_IO_LEVEL_HIGH);
    }
    else
    {
        R_BSP_PinWrite(pin, BSP_IO_LEVEL_LOW);
    }

    /* Disable pin access */
    R_BSP_PinAccessDisable();
}

void pin_state_check(void){
    bsp_io_level_t pin_data_datafull, pin_data_level_ch00, pin_data_level_ch01;

    // pin read
    R_IOPORT_PinRead(&g_ioport_ctrl, DATAFULL_PIN, &pin_data_datafull);
    R_IOPORT_PinRead(&g_ioport_ctrl, ANALOG_LEVEL_CH00_PIN, &pin_data_level_ch00);
    R_IOPORT_PinRead(&g_ioport_ctrl, ANALOG_LEVEL_CH01_PIN, &pin_data_level_ch01);

    if(pin_data_datafull == PIN_HIGH__LED_ON){
        pin_state_change(LED3_RED, PIN_HIGH__LED_ON);
    }else{
        pin_state_change(LED3_RED, PIN_LOW__LED_OFF);
    }

    if(pin_data_level_ch00 == PIN_HIGH__LED_ON || pin_data_level_ch01 == PIN_HIGH__LED_ON){
        pin_state_change(LED2_GREEN, PIN_HIGH__LED_ON);
    }else{
        pin_state_change(LED2_GREEN, PIN_LOW__LED_OFF);
    }
}

void irq13_s1_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    // Set a flag
    g_button_s1_pushed = true;
}

void irq12_s2_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);

    // Set a flag
    g_button_s2_pushed = true;
}

void sci0_uart_callback(uart_callback_args_t *p_args)
{
    if(p_args->event == UART_EVENT_RX_COMPLETE)
    {
        recv_buff[recv_count] = recv_data;
        recv_count++;

        if(recv_data == '$'){
            g_sci0_uart_recv_flag = SET_FLAG;
        }else{
            R_SCI_UART_Read(&g_sci8_uart_ctrl, &recv_data, 1);
        }
    }
    else if(p_args->event == UART_EVENT_TX_COMPLETE)
    {
        g_sci0_uart_transmit_complete_flag = SET_FLAG;
    }
}
