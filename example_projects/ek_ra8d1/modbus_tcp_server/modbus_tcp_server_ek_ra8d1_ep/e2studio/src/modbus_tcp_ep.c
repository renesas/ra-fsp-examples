/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "app_thread.h"
#include "modbus_tcp_ep.h"
#include "common_utils.h"

/* Network Configuration */
#define MAX_IP_ADDR_LEN           (16U)
#define NETWORK_INIT_TIMEOUT_MS   (500U)
#define NETWORK_INIT_MAX_ATTEMPTS (60U)

/* LED Blink Configuration */
#define DEFAULT_BLINK_DELAY_MS    (1500U)
#define BLINK_DELAY_MIN_MS        (100U)
#define BLINK_DELAY_MAX_MS        (3000U)

/* Modbus Addressing */
#define MODBUS_COIL_COUNT         (3U)
#define MODBUS_INPUT_COUNT        (2U)
#define MODBUS_MAX_REGISTERS      (1U)
#define MODBUS_MAX_COILS_READ     (125U)

static uint16_t temp_regs[2] = {0};

/* MAC Address Configuration (Test MAC) */
static const uint8_t g_mac_address[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

/* IP Configuration */
static const uint8_t g_ip_address[4]         = {192, 168, 0, 1};
static const uint8_t g_netmask[4]            = {255, 255, 255, 0};
static const uint8_t g_gateway_address[4]    = {192, 168, 0, 1};
static const uint8_t g_dns_server_address[4] = {10, 60, 1, 2};

static const char *g_modbus_client_addr = "192.168.0.2";

/* Coil-to-GPIO pin mapping for LED outputs */
static const bsp_io_port_pin_t g_coil_pin_map[] =
{
    [MODBUS_COIL_LED1_ADDR] = BSP_IO_PORT_06_PIN_00,
    [MODBUS_COIL_LED2_ADDR] = BSP_IO_PORT_04_PIN_14,
    [MODBUS_COIL_LED3_ADDR] = BSP_IO_PORT_01_PIN_07,
};

/* Discrete input-to-GPIO pin mapping for switch inputs */
static const bsp_io_port_pin_t g_input_pin_map[] =
{
    [MODBUS_INPUT_SW1_ADDR] = BSP_IO_PORT_00_PIN_09,
    [MODBUS_INPUT_SW2_ADDR] = BSP_IO_PORT_00_PIN_08,
};

static volatile uint16_t g_led_blink_delay_ms = DEFAULT_BLINK_DELAY_MS;

typedef uint16_t (*modbus_reg_reader_t)(uint16_t addr);
typedef bool (*modbus_reg_writer_t)(uint16_t addr, uint16_t value);

/* Forward declaration */
static float read_mcu_temp_celsius(void);
static bool network_stack_init(void);
static void network_config_display(void);
static bool modbus_server_init(void);
static void modbus_tcp_fc01_read_coils_handler(modbus_tcp_server_callback_args_t * const p_cb_args);
static void modbus_tcp_fc02_read_discrete_inputs_handler(modbus_tcp_server_callback_args_t * const p_cb_args);
static void modbus_tcp_fc03_read_holding_registers_handler(modbus_tcp_server_callback_args_t * const p_cb_args);
static void modbus_tcp_fc04_read_input_registers_handler(modbus_tcp_server_callback_args_t * const p_cb_args);
static void modbus_tcp_fc05_write_single_coil_handler(modbus_tcp_server_callback_args_t * const p_cb_args);
static void modbus_tcp_fc06_write_single_register_handler(modbus_tcp_server_callback_args_t * const p_cb_args);
static void modbus_tcp_fc0f_write_multiple_coils_handler(modbus_tcp_server_callback_args_t * const p_cb_args);
static void modbus_common_read(uint16_t function_code,
                               uint16_t start_addr, uint16_t count, void * out,
                               uint8_t * out_bytes,
                               modbus_tcp_server_user_function_error_t * code,
                               modbus_reg_reader_t reader,
                               uint16_t valid_start, uint16_t valid_end);
static void modbus_common_write(uint16_t function_code,
                                uint16_t start_addr, uint16_t count,
                                const void * in_data,
                                modbus_tcp_server_user_function_error_t * code,
                                modbus_reg_writer_t writer,
                                uint16_t valid_start, uint16_t valid_end,
                                uint16_t * resp_addr, uint16_t * resp_value);

/***********************************************************************************************************************
 * @brief      Modbus TCP Server Thread Entry Point.
 *
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
void modbus_tcp_entry(void)
{
    fsp_pack_version_t  version = {RESET_VALUE};
    modbus_err_t status = MODBUS_ERR_OK;

    TERM_INIT();

    /* Version get API for FSP pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the RTT */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(MODBUS_TCP_EP_DESCRIPTION);

    APP_PRINT(MODBUS_FUNCTION_TABLE);

    APP_PRINT("\r\n============================================\r\n");
    APP_PRINT("   Modbus TCP Server Initialization\r\n");
    APP_PRINT("============================================\r\n");

    /* Initialize network stack */
    if (!network_stack_init())
    {
        APP_ERR_PRINT("FreeRTOS_IPInit Failed");
        APP_ERR_TRAP(true);
    }

    /* Display the current network configuration */
    network_config_display();

    /* Initialize Modbus TCP server */
    status = modbus_server_init();
    if (MODBUS_ERR_OK != status)
    {
        APP_ERR_PRINT("Modbus server initialization failed (Error code: 0x%x)\r\n", status);
        modbus_err_t close_status = R_MODBUS_TCP_SERVER_Close(&g_modbus_tcp_server0_ctrl);
        if (MODBUS_ERR_OK != close_status)
        {
            APP_ERR_PRINT("R_MODBUS_TCP_SERVER_Close API failed (Error code: 0x%x)\r\n", close_status);
        }

        APP_ERR_TRAP(status);
    }

    /* Indicate server is running with LED status */
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_coil_pin_map[MODBUS_COIL_LED2_ADDR], BSP_IO_LEVEL_HIGH);
    R_IOPORT_PinWrite(&g_ioport_ctrl, g_coil_pin_map[MODBUS_COIL_LED3_ADDR], BSP_IO_LEVEL_HIGH);

    /* Main status blink loop */
    while (1)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_coil_pin_map[MODBUS_COIL_LED1_ADDR], BSP_IO_LEVEL_HIGH);
        vTaskDelay(pdMS_TO_TICKS(g_led_blink_delay_ms));

        R_IOPORT_PinWrite(&g_ioport_ctrl, g_coil_pin_map[MODBUS_COIL_LED1_ADDR], BSP_IO_LEVEL_LOW);
        vTaskDelay(pdMS_TO_TICKS(g_led_blink_delay_ms));
    }
}

/***********************************************************************************************************************
 * @brief      This callback is invoked by the Modbus TCP server stack whenever a valid Modbus request is received
 *             from a client.
 *
 * @param[in]  p_args   Pointer to callback arguments.
 * @retval     None.
 **********************************************************************************************************************/
void function_code_callback(modbus_tcp_server_callback_args_t * const p_args)
{
    switch (p_args->function_code)
    {
        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_COILS:
            modbus_tcp_fc01_read_coils_handler(p_args);
            break;

        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_DISCRETE_INPUTS:
            modbus_tcp_fc02_read_discrete_inputs_handler(p_args);
            break;

        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_HOLDING_REGISTERS:
            modbus_tcp_fc03_read_holding_registers_handler(p_args);
            break;

        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_INPUT_REGISTERS:
            modbus_tcp_fc04_read_input_registers_handler(p_args);
            break;

        case MODBUS_TCP_SERVER_FUNCTION_CODE_WRITE_SINGLE_COIL:
            modbus_tcp_fc05_write_single_coil_handler(p_args);
            break;

        case MODBUS_TCP_SERVER_FUNCTION_CODE_WRITE_SINGLE_REGISTER:
            modbus_tcp_fc06_write_single_register_handler(p_args);
            break;

        case MODBUS_TCP_SERVER_FUNCTION_CODE_WRITE_MULTIPLE_COILS:
            modbus_tcp_fc0f_write_multiple_coils_handler(p_args);
            break;

        case MODBUS_TCP_SERVER_FUNCTION_CODE_WRITE_MULTIPLE_REGISTERS:
            /* Not implemented in this example */
            break;

        case MODBUS_TCP_SERVER_FUNCTION_CODE_READ_WRITE_MULTIPLE_REGISTERS:
            /* Not implemented in this example */
            break;

        default:
            break;
    }
}

/***********************************************************************************************************************
 * @brief   Initialize the FreeRTOS+TCP network stack and wait for link up.
 *
 * @retval  true   Network stack initialized successfully and link is up.
 * @retval  false  Network stack failed to initialize or link did not come up
 *                 within the configured timeout.
 **********************************************************************************************************************/
static bool network_stack_init(void)
{
    /* Initialize the TCP/IP stack */
    FreeRTOS_IPInit((uint8_t *) g_ip_address,
                    (uint8_t *) g_netmask,
                    (uint8_t *) g_gateway_address,
                    (uint8_t *) g_dns_server_address,
                    (uint8_t *) g_mac_address);

    APP_PRINT("Waiting for network stack to initialize");

    for (uint32_t attempt = 0; attempt < NETWORK_INIT_MAX_ATTEMPTS; attempt++)
    {
        if (FreeRTOS_IsNetworkUp() == pdTRUE)
        {
            APP_PRINT(" [DONE]\r\n");
            return true;
        }

        vTaskDelay(pdMS_TO_TICKS(NETWORK_INIT_TIMEOUT_MS));
        APP_PRINT(".");
    }

    APP_PRINT("\r\nError: Network stack failed to initialize\r\n");
    return false;
}

/***********************************************************************************************************************
 * @brief   Reads the current state of an LED (coil).
 *
 * @param[in]  addr    Coil (LED) address index used to select the mapped GPIO pin.
 *
 * @retval     uint16_t   Current LED state:
 *                        - 0: LED is OFF
 *                        - 1: LED is ON
 **********************************************************************************************************************/
static uint16_t get_led_state(uint16_t addr)
{
    bsp_io_port_pin_t pin = g_coil_pin_map[addr];
    bsp_io_level_t level;

    R_IOPORT_PinRead(&g_ioport_ctrl, pin, &level);
    APP_PRINT("  LED addr %u: %s\r\n", addr, level ? "ON" : "OFF");

    return (uint16_t)level;
}

/***********************************************************************************************************************
 * @brief   Set the state of an LED (coil) based on the Modbus value.
 *
 * @param[in]  addr    Coil (LED) address index used to select the mapped GPIO pin.
 * @param[in]  value   Modbus coil value (raw request value or normalized 0/1).
 *
 * @retval     FSP_SUCCESS    The LED state was updated successfully.
 * @retval     Other          Failed to write to the GPIO pin.
 **********************************************************************************************************************/
static bool set_led_state(uint16_t addr, uint16_t value)
{
    fsp_err_t err;
    bsp_io_level_t level = (value == 1) ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW;

    err = R_IOPORT_PinWrite(&g_ioport_ctrl, g_coil_pin_map[addr], level);

    if (FSP_SUCCESS == err)
    {
        APP_PRINT("  LED addr %u set to: %s\r\n", addr, value ? "ON" : "OFF");
        return true;
    }
    else
    {
        APP_PRINT("  ERROR: Failed to set LED addr %u (err: %d)\r\n", addr, err);
        return false;
    }
}

/***********************************************************************************************************************
 * @brief   Read the current state of a switch (discrete input).
 *
 * @param[in]  addr    Discrete input (switch) address index used to select the mapped GPIO pin.
 *
 * @retval     uint16_t   Current switch state (0 = Pressed, 1 = Released).
 **********************************************************************************************************************/
static uint16_t get_switch_state(uint16_t addr)
{
    bsp_io_port_pin_t pin = g_input_pin_map[addr];
    bsp_io_level_t level;

    R_IOPORT_PinRead(&g_ioport_ctrl, pin, &level);
    APP_PRINT("  SW addr %u: %s\r\n", addr, level ? "Released" : "Pressed");

    return (uint16_t)level;
}

/***********************************************************************************************************************
 * @brief   Read the value of a Modbus holding register(LED blink interval in milliseconds).
 *
 * @param[in]  addr    Modbus holding register address.
 *
 * @retval     uint16_t   Value of the holding register or 0 if the address is invalid.
 **********************************************************************************************************************/
static uint16_t modbus_read_holding_register(uint16_t addr)
{
    switch (addr)
    {
        case MODBUS_HOLDING_REGISTER_ADDR:
            return g_led_blink_delay_ms;
        default:
            return 0;
    }
}

/***********************************************************************************************************************
 * @brief   Write a value to a Modbus holding register(LED blink interval in milliseconds).
 *
 * @param[in]  addr    Modbus holding register address.
 * @param[in]  value   Value to write to the holding register.
 *
 * @retval     true    The value was successfully written.
 * @retval     false   Invalid address or value out of range.
 **********************************************************************************************************************/
static bool modbus_write_holding_register(uint16_t addr, uint16_t value)
{
    switch (addr)
    {
        case MODBUS_HOLDING_REGISTER_ADDR:
            if (value >= BLINK_DELAY_MIN_MS && value <= BLINK_DELAY_MAX_MS)
            {
                g_led_blink_delay_ms = value;
                APP_PRINT("  Blink delay updated to %u ms\r\n", value);
                return true;
            }
            else
            {
                APP_PRINT("  Error: Blink delay out of range [%u-%u]\r\n",
                          BLINK_DELAY_MIN_MS, BLINK_DELAY_MAX_MS);
                return false;
            }

        default:
            return false;
    }
}

/***********************************************************************************************************************
 * @brief   Read input register value (temperature as float, 2 registers)
 *
 * @param[in]   addr    Register address
 * @return      Register value (16-bit)
 **********************************************************************************************************************/
static uint16_t modbus_read_input_register(uint16_t addr)
{
    /* Read sensor only once per request (when accessing first register) */
    if (addr == MODBUS_HOLDING_REGISTER_ADDR)
    {
        float temp_c = read_mcu_temp_celsius ();

        union
        {
            float f;
            uint32_t u32;
        } conv;
        conv.f = temp_c;

        temp_regs[1] = (uint16_t) ((conv.u32 >> 16) & 0xFFFF);  // High word first
        temp_regs[0] = (uint16_t) (conv.u32 & 0xFFFF);          // Low word second
    }

    return temp_regs[addr];
}

/***********************************************************************************************************************
 * @brief   Generic Modbus read handler for coils, discrete inputs, holding registers, and input registers.
 *
 * @param[in]   function_code   Modbus function code (FC01/02/03/04).
 * @param[in]   start_addr      First address to read.
 * @param[in]   count           Number of items to read.
 * @param[out]  out             Output buffer for Modbus response data.
 * @param[out]  out_bytes       Size of the output data (in bytes).
 * @param[out]  code            Pointer to error code result.
 * @param[in]   reader          Callback function used to read a single address.
 * @param[in]   valid_start     Minimum allowed address.
 * @param[in]   valid_end       Maximum allowed address.
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_common_read(uint16_t function_code,
                               uint16_t start_addr,
                               uint16_t count,
                               void *out,
                               uint8_t *out_bytes,
                               modbus_tcp_server_user_function_error_t *code,
                               modbus_reg_reader_t reader,
                               uint16_t valid_start,
                               uint16_t valid_end)
{
    *code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;
    memset(out, 0, count * 2);

    bool read_bit = (function_code == MODBUS_FC_READ_COILS || function_code == MODBUS_FC_READ_DISCRETE_INPUTS);

    for (uint16_t i = 0; i < count; i++)
    {
        uint16_t addr = start_addr + i;

        if (addr < valid_start || addr > valid_end)
        {
            *code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS;
            *out_bytes = 0;
            APP_PRINT("  Error: Address %u out of range [%u-%u]\r\n", addr, valid_start, valid_end);
            return;
        }

        uint16_t value = reader(addr);

        if (read_bit)
        {
            uint8_t *p_bits = (uint8_t *)out;
            p_bits[i / 8] |= (value & 1U) << (i % 8);
        }
        else
        {
            uint16_t *p_regs = (uint16_t *)out;
            p_regs[i] = value;
        }
    }

    *out_bytes = (uint8_t)(read_bit ? ((count + 7U) >> 3U) : (count * 2U));
    /* Log raw response data as bytes */
    uint8_t * p = (uint8_t *) out;
    APP_PRINT("  Response Data (%u bytes):", *out_bytes);
    for (uint8_t i = 0; i < *out_bytes; i++)
    {
        APP_PRINT(" %02X", p[i]);
    }
    APP_PRINT("\r\n");
}

/***********************************************************************************************************************
 * @brief   Common write operation for all Modbus write functions.
 *
 * @param[in]   function_code  Modbus function code.
 * @param[in]   start_addr     Starting Modbus address requested by the client.
 * @param[in]   count          Number of items (bits or registers) to write.
 * @param[in]   in_data        Pointer to input payload buffer containing values to be written.
 * @param[out]  code           Pointer to error code indicating success or Modbus user error status.
 * @param[in]   writer         Callback used to write a value to a given address.
 * @param[in]   valid_start    Minimum allowed address.
 * @param[in]   valid_end      Maximum allowed address.
 * @param[out]  resp_addr      Pointer to variable that receives the starting address for the response.
 * @param[out]  resp_value     Pointer to variable that receives the number of items written for the response.
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_common_write(uint16_t function_code,
                                uint16_t start_addr,
                                uint16_t count,
                                const void * in_data,
                                modbus_tcp_server_user_function_error_t * code,
                                modbus_reg_writer_t writer,
                                uint16_t valid_start,
                                uint16_t valid_end,
                                uint16_t * resp_addr,
                                uint16_t * resp_value)
{
    *code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_SUCCESS;

    const uint8_t *p_bytes = (const uint8_t *) in_data;

    for (uint16_t i = 0; i < count; i++)
    {
        uint16_t addr = start_addr + i;

        if ((addr < valid_start) || (addr > valid_end))
        {
            *code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_ADDRESS;
            APP_PRINT("  Error: Address %u out of range [%u-%u]\r\n", addr, valid_start, valid_end);
            return;
        }

        uint16_t value;

        if (function_code == MODBUS_FC_WRITE_SINGLE_COIL)
        {
            /* FC05: write single coil, 2 bytes, big-endian */
            value = (uint16_t)((uint16_t)p_bytes[1] << 8) | p_bytes[0];
            value = (value == 0xFF00) ? 1 : 0;
        }
        else if (function_code == MODBUS_FC_WRITE_MULTIPLE_COILS)
        {
            /* FC15: write multiple coils, 1 bit per coil */
            uint8_t bit = (p_bytes[i / 8] >> (i % 8)) & 0x01;
            value = bit ? 1 : 0;
        }
        else
        {
            /* Write single/multiple registers: combine 2 bytes big-endian */
            value = (uint16_t)((uint16_t)p_bytes[2 * i + 1] << 8) | p_bytes[2 * i];
        }

        /* Print request data for this address */
        APP_PRINT("  Write Request: addr=%u, value=0x%04X\r\n", addr, value);

        if (!writer(addr, value))
        {
            *code = MODBUS_TCP_SERVER_USER_FUNCTION_ERROR_ILLEGAL_DATA_VALUE;
            APP_PRINT("  Error: Write failed at address %u\r\n", addr);
            return;
        }
    }

    /* Prepare response */
    *resp_addr = start_addr;
    *resp_value = count;
}



/***********************************************************************************************************************
 * @brief   Modbus FC01 - Reads coil states (LED state)
 *
 * @param[in]   p_cb_args   Pointer to Modbus TCP server callback arguments containing request/response data
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_tcp_fc01_read_coils_handler(modbus_tcp_server_callback_args_t * const p_cb_args)
{
    modbus_tcp_server_read_coils_t * const req =
        (modbus_tcp_server_read_coils_t *) p_cb_args->request_and_response_data;

    APP_PRINT("[Modbus TCP] FC01 Read Coils\r\n");
    APP_PRINT("  Request: start_addr=%u, count=%u\r\n",
              req->request_start_address, req->request_num_of_coils);

    modbus_common_read(MODBUS_FC_READ_COILS,
                       req->request_start_address,
                       req->request_num_of_coils,
                       req->response_data,
                       &req->response_num_of_bytes,
                       &req->exception_code,
                       get_led_state,
                       MODBUS_COIL_LED1_ADDR,
                       MODBUS_COIL_LED3_ADDR);
}

/***********************************************************************************************************************
 * @brief   Modbus FC02 - Reads switch (discrete input) states
 *
 * @param[in]   p_cb_args   Pointer to Modbus TCP server callback arguments containing request/response data
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_tcp_fc02_read_discrete_inputs_handler(modbus_tcp_server_callback_args_t * const p_cb_args)
{
    modbus_tcp_server_read_inputs_t * const req =
        (modbus_tcp_server_read_inputs_t *) p_cb_args->request_and_response_data;

    APP_PRINT("[Modbus TCP] FC02 Read Discrete Inputs\r\n");
    APP_PRINT("  Request: start_addr=%u, count=%u\r\n",
              req->request_start_address, req->request_num_of_inputs);

    modbus_common_read(MODBUS_FC_READ_DISCRETE_INPUTS,
                       req->request_start_address,
                       req->request_num_of_inputs,
                       req->response_data,
                       &req->response_num_of_bytes,
                       &req->exception_code,
                       get_switch_state,
                       MODBUS_INPUT_SW1_ADDR,
                       MODBUS_INPUT_SW2_ADDR);
}

/***********************************************************************************************************************
 * @brief   Modbus FC03 - Reads holding registers (LED blink delay).
 *
 * @param[in]   p_cb_args   Pointer to Modbus TCP server callback arguments containing request/response data
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_tcp_fc03_read_holding_registers_handler(modbus_tcp_server_callback_args_t * const p_cb_args)
{
    modbus_tcp_server_read_holding_registers_t * const req =
        (modbus_tcp_server_read_holding_registers_t *) p_cb_args->request_and_response_data;

    APP_PRINT("[Modbus TCP] FC03 Read Holding Registers\r\n");
    APP_PRINT("  Request: start_addr=%u, count=%u\r\n",
              req->request_start_address, req->request_num_of_registers);

    modbus_common_read(MODBUS_FC_READ_HOLDING_REGISTERS,
                       req->request_start_address,
                       req->request_num_of_registers,
                       req->response_data,
                       &req->response_num_of_bytes,
                       &req->exception_code,
                       modbus_read_holding_register,
                       MODBUS_HOLDING_REGISTER_ADDR,
                       MODBUS_HOLDING_REGISTER_ADDR);
}

/***********************************************************************************************************************
 * @brief   Modbus FC04 - Reads input registers (MCU temperature sensor data).
 *
 * @param[in]   p_cb_args   Pointer to Modbus TCP server callback arguments containing request/response data
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_tcp_fc04_read_input_registers_handler(modbus_tcp_server_callback_args_t * const p_cb_args)
{
    modbus_tcp_server_read_input_registers_t * const req =
        (modbus_tcp_server_read_input_registers_t *) p_cb_args->request_and_response_data;

    APP_PRINT("[Modbus TCP] FC04 Read Input Registers\r\n");
    APP_PRINT("  Request: start_addr=%u, count=%u\r\n",
              req->request_start_address, req->request_num_of_regisgers);

    modbus_common_read(MODBUS_FC_READ_INPUT_REGISTERS,
                       req->request_start_address,
                       req->request_num_of_regisgers,
                       req->response_data,
                       &req->response_num_of_bytes,
                       &req->exception_code,
                       modbus_read_input_register,
                       MODBUS_INPUT_REGISTER_START_ADDR,
                       MODBUS_INPUT_REGISTER_END_ADDR);
}

/***********************************************************************************************************************
 * @brief   Modbus FC05 - Writes state to a single coil (LED output).
 *
 * @param[in]   p_cb_args   Pointer to Modbus TCP server callback arguments containing request/response data
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_tcp_fc05_write_single_coil_handler(modbus_tcp_server_callback_args_t * const p_cb_args)
{
    modbus_tcp_server_write_single_coil_t * const req =
        (modbus_tcp_server_write_single_coil_t *) p_cb_args->request_and_response_data;

    APP_PRINT("[Modbus TCP] FC05 Write Single Coil\r\n");
    APP_PRINT("  Request: addr=%u, value=0x%04X\r\n",
              req->request_output_address, req->request_output_value);

    modbus_common_write(MODBUS_FC_WRITE_SINGLE_COIL,
                        req->request_output_address,
                        1,
                        &req->request_output_value,
                        &req->exception_code,
                        set_led_state,
                        MODBUS_COIL_LED1_ADDR,
                        MODBUS_COIL_LED3_ADDR,
                        &req->response_output_address,
                        &req->response_output_value
                        );
}

/***********************************************************************************************************************
 * @brief   Modbus FC06 - Writes value to a single holding register (LED blink delay).
 *
 * @param[in]   p_cb_args   Pointer to Modbus TCP server callback arguments containing request/response data
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_tcp_fc06_write_single_register_handler(modbus_tcp_server_callback_args_t * const p_cb_args)
{
    modbus_tcp_server_write_single_register_t * const req =
        (modbus_tcp_server_write_single_register_t *) p_cb_args->request_and_response_data;

    APP_PRINT("[Modbus TCP] FC06 Write Single Register\r\n");
    APP_PRINT("  Request: addr=%u, value=0x%04X\r\n",
              req->request_register_address, req->request_register_value);

    modbus_common_write(MODBUS_FC_WRITE_SINGLE_REGISTER,
                        req->request_register_address,
                        1,
                        &req->request_register_value,
                        &req->exception_code,
                        modbus_write_holding_register,
                        MODBUS_HOLDING_REGISTER_ADDR,
                        MODBUS_HOLDING_REGISTER_ADDR,
                        &req->response_register_address,
                        &req->response_register_value
                        );
}

/***********************************************************************************************************************
 * @brief   Modbus FC0F - Writes states to multiple coils (LED outputs).
 *
 * @param[in]   p_cb_args   Pointer to Modbus TCP server callback arguments containing request/response data
 *
 * @retval      None.
 **********************************************************************************************************************/
static void modbus_tcp_fc0f_write_multiple_coils_handler(modbus_tcp_server_callback_args_t * const p_cb_args)
{
    modbus_tcp_server_write_multiple_coils_t * const req =
        (modbus_tcp_server_write_multiple_coils_t *) p_cb_args->request_and_response_data;

    APP_PRINT("[Modbus TCP] FC0F Write Multiple Coils\r\n");
    APP_PRINT("  Request: start_addr=%u, count=%u\r\n",
              req->request_start_address, req->request_num_of_outputs);

    modbus_common_write(MODBUS_FC_WRITE_MULTIPLE_COILS,
                        req->request_start_address,
                        req->request_num_of_outputs,
                        &req->request_data,
                        &req->exception_code,
                        set_led_state,
                        MODBUS_COIL_LED1_ADDR,
                        MODBUS_COIL_LED3_ADDR,
                        &req->response_start_address,
                        &req->response_num_of_outputs
                        );
}

/***********************************************************************************************************************
 * @brief   Read the MCU internal temperature in Celsius using the ADC.
 *
 * @retval     float   Temperature in degrees Celsius.
 **********************************************************************************************************************/
static float read_mcu_temp_celsius(void)
{
    fsp_err_t err;
    float mcu_temp_c = 0.0f;

    /* Open ADC */
    err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    APP_ERR_RET(err != FSP_SUCCESS, err, "R_ADC_Open API failed\r\n");

    /* Configure ADC scan */
    err = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    APP_ERR_RET(err != FSP_SUCCESS, err, "R_ADC_ScanCfg API failed\r\n");

    /* Start ADC scan */
    err = R_ADC_ScanStart(&g_adc0_ctrl);
    APP_ERR_RET(err != FSP_SUCCESS, err, "R_ADC_ScanStart API failed\r\n");

    /* Wait for scan to complete */
    adc_status_t status = {0};
    status.state = ADC_STATE_SCAN_IN_PROGRESS;
    while (ADC_STATE_SCAN_IN_PROGRESS == status.state)
    {
        err = R_ADC_StatusGet(&g_adc0_ctrl, &status);
        APP_ERR_RET(err != FSP_SUCCESS, err, "R_ADC_StatusGet API failed\r\n");
    }

    /* Read ADC value */
    uint16_t adc_value = 0;
    err = R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, &adc_value);
    APP_ERR_RET(err != FSP_SUCCESS, err, "R_ADC_Read API failed\r\n");

    /* Convert ADC to temperature */
    mcu_temp_c = ADCTEMP_AS_C(adc_value);

    APP_PRINT("  [Temperature1] %.2f°C (ADC: 0x%04X)\r\n", mcu_temp_c, adc_value);

    return mcu_temp_c;
}

/***********************************************************************************************************************
 * @brief   Print IP address in dotted decimal format.
 *
 * @retval  None.
 **********************************************************************************************************************/
static void print_ip_address(const char *label, uint32_t ip)
{
    APP_PRINT("  %-15s: %lu.%lu.%lu.%lu\r\n",
              label,
              (ip >> 0) & 0xFF,
              (ip >> 8) & 0xFF,
              (ip >> 16) & 0xFF,
              (ip >> 24) & 0xFF);
}



/***********************************************************************************************************************
 * @brief   Display the current network configuration.
 *
 * @retval  None.
 **********************************************************************************************************************/
static void network_config_display(void)
{
    uint32_t ip_address, netmask, gateway, dns;
    do
    {
        FreeRTOS_GetAddressConfiguration(&ip_address, &netmask, &gateway, &dns);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    while (ip_address == 0);

    APP_PRINT("Network Configuration:\r\n");
    print_ip_address("IP Address", ip_address);
    print_ip_address("Subnet Mask", netmask);
    print_ip_address("Gateway", gateway);
    print_ip_address("DNS Server", dns);
    APP_PRINT("============================================\r\n");
}

/***********************************************************************************************************************
 * @brief  Initialize and start Modbus TCP server.
 *
 * @return MODBUS_ERR_OK if successful, false otherwise
 **********************************************************************************************************************/
static bool modbus_server_init(void)
{
    modbus_err_t err;

    /* Open Modbus TCP server */
    err = R_MODBUS_TCP_SERVER_Open(&g_modbus_tcp_server0_ctrl, &g_modbus_tcp_server0_cfg);
    APP_ERR_RET(err != MODBUS_ERR_OK, false, "Modbus TCP Server Open failed\r\n");

    /* Register client IP list */
    err = R_MODBUS_TCP_SERVER_RegisterIpList(&g_modbus_tcp_server0_ctrl, (char *)g_modbus_client_addr);
    APP_ERR_RET(err != MODBUS_ERR_OK, false, "Modbus TCP Server IP registration failed\r\n");

    /* Start the server */
    err = R_MODBUS_TCP_SERVER_Start(&g_modbus_tcp_server0_ctrl);
    APP_ERR_RET(err != MODBUS_ERR_OK, false, "Modbus TCP Server Start failed\r\n");

    APP_PRINT("[Modbus TCP] Server started successfully\r\n");
    APP_PRINT("[Modbus TCP] Ready for client connections\r\n");
    APP_PRINT("============================================\r\n");

    return err;
}
