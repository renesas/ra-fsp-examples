/***********************************************************************************************************************
 * File Name    : usbx_pprn_thread_entry.c
 * Description  : Contains definitions and functions used for usbx_pprn functionality.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "usbx_pprn_thread.h"
#include "r_usb_basic.h"
#include "common_utils.h"
#include "usbx_pprn_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usbx_pprn_ep
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
  Exported global variables and functions (to be accessed by other files)
 **********************************************************************************************************************/
extern TX_THREAD usbx_pprn_read_thread;
extern uint8_t g_device_framework_full_speed[];
extern uint8_t g_device_framework_hi_speed[];
extern uint8_t g_string_framework[];
extern uint8_t g_language_id_framework[];
extern uint8_t printer_device_id[];
extern uint32_t usb_peri_usbx_initialize(uint32_t dcd_io);

/*******************************************************************************************************************//**
 Private global variables and functions
 **********************************************************************************************************************/
static void memory_usage_update(void);
static UINT usbx_status_callback(ULONG status);
static uint32_t time_diff(uint32_t v_origin, uint32_t v_minus, uint32_t wrap);
static void ux_printer_instance_activate(void *printer_instance);
static void ux_printer_instance_deactivate(void *printer_instance);
static void ux_printer_soft_reset(void *printer_instance);
static char print_buff[BUFFER_SIZE_MIN] = {RESET_VALUE};

static UX_DEVICE_CLASS_PRINTER_PARAMETER device_printer_parameter;

static union _PRINTER_PORT_STATUS {
    struct  {
        uint8_t           reserved:VALUE_THREE;
        uint8_t           not_error:VALUE_ONE;
        uint8_t           select:VALUE_ONE;
        uint8_t           paper_empty:VALUE_ONE;
    } bm;
    uint8_t value;
} device_printer_port_status;

static struct MEM_USAGE_STRUCT {
    uint32_t           mem_usage_max;
    uint32_t           mem_usage;
} mem_usage = {RESET_VALUE, RESET_VALUE};

/* Memory pool size of 18k is required for USBX device class pre-built libraries
 * and it is valid only if it with default USBX configurations */
static uint32_t g_ux_pool_memory[(MEMPOOL_SIZE + DEMO_STACK_SIZE*VALUE_THREE) / BYTE_SIZE];

uint8_t _ux_system_slave_class_prn_name[] = "ux_slave_class_prn";
UX_DEVICE_CLASS_PRINTER *device_printer = UX_NULL;

/* To check the printer port status */
uint8_t port_status = RESET_VALUE;

/* PPRN Thread entry function */
void usbx_pprn_thread_entry(void)
{
    /* To check UX API return status */
    UINT status = UX_SUCCESS;
    UINT state  = TX_SUSPENDED;

    /* To check FSP API return status */
    uint32_t tick0           = RESET_VALUE;
    uint32_t tick1           = RESET_VALUE;
    uint32_t diff            = RESET_VALUE;
    uint32_t pmem            = RESET_VALUE;
    uint8_t  *stack_pointer  = UX_NULL;
    uint8_t  *memory_pointer = UX_NULL;

    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print the EP banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* ux_system_initialization */
    stack_pointer  = (uint8_t *)g_ux_pool_memory;
    memory_pointer = stack_pointer + DEMO_STACK_SIZE * VALUE_THREE;

    status = ux_system_initialize(memory_pointer, MEMPOOL_SIZE, UX_NULL, RESET_VALUE);
    /* Error handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_system_initialize API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_system initialized successfully");

    /* ux_device stack initialization */
    status = ux_device_stack_initialize(g_device_framework_hi_speed, DEVICE_FRAME_LENGTH_HIGH_SPEED,\
                                        g_device_framework_full_speed, DEVICE_FRAME_LENGTH_FULL_SPEED,\
                                        g_string_framework, STRING_FRAMEWORK_LENGTH, g_language_id_framework,\
                                        LANGUAGE_ID_FRAME_WORK_LENGTH, usbx_status_callback);
    /* Error handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_initialize API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device stack initialized successfully");

    /* Set the parameters for the callback upon insertion or removal of a printer device */
    _ux_utility_memory_set(&device_printer_parameter, RESET_VALUE, sizeof(device_printer_parameter));
    _ux_utility_short_put_big_endian(printer_device_id, PRINTER_DEVICE_ID_LENGTH);
    device_printer_port_status.value = UX_DEVICE_CLASS_PRINTER_SELECT | UX_DEVICE_CLASS_PRINTER_NOT_ERROR;
    device_printer_parameter.ux_device_class_printer_device_id           = printer_device_id;
    device_printer_parameter.ux_device_class_printer_instance_activate   = ux_printer_instance_activate;
    device_printer_parameter.ux_device_class_printer_instance_deactivate = ux_printer_instance_deactivate;
    device_printer_parameter.ux_device_class_printer_soft_reset          = ux_printer_soft_reset;

    /* ux_device stack class registration */
    status = ux_device_stack_class_register(_ux_system_slave_class_prn_name, _ux_device_class_printer_entry,\
                                            CONFIG_NUMB,\
                                            INTERFACE_NUMB0, (void*) &device_printer_parameter);
    /* Error handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("ux_device_stack_class_register API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("ux_device_stack_class registered successfully");

    if (USB_SPEED_FS == g_basic0_cfg.usb_speed)
    {
        /* Initialize peripheral mode with Full Speed */
        status = usb_peri_usbx_initialize(R_USB_FS0_BASE);
        /* Error handle */
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("usb_peri_usbx_initialize with Full Speed API failed");
            ERROR_TRAP(status);
        }
    }
    else if (USB_SPEED_HS == g_basic0_cfg.usb_speed)
    {
        /* Initialize peripheral mode with High Speed */
        status = usb_peri_usbx_initialize(R_USB_HS0_BASE);
        /* Error handle */
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("usb_peri_usbx_initialize with High Speed API failed");
            ERROR_TRAP(status);
        }
    }
    else
    {
        PRINT_ERR_STR("usb_peri_usbx_initialize API failed to initialize USB Full Speed/High Speed Mode");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("usb_peri_usbx initialized successfully");

    /* Open USB driver */
    status = R_USB_Open(&g_basic0_ctrl, &g_basic0_cfg);
    /* Error handle */
    if (FSP_SUCCESS != status)
    {
        PRINT_ERR_STR("R_USB_Open API failed");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("USB driver opened successfully");

    /* Resume the usbx_pprn_read_thread */
    status = tx_thread_info_get(&usbx_pprn_read_thread, TX_NULL, &state, TX_NULL, TX_NULL, TX_NULL, TX_NULL,\
                                TX_NULL, TX_NULL);
    /* Error handle */
    if (UX_SUCCESS != status)
    {
        PRINT_ERR_STR("Error in getting thread info\n");
        ERROR_TRAP(status);
    }

    /* Wait for initialization to complete */
    if (TX_SUSPENDED == state)
    {
        tx_thread_resume(&usbx_pprn_read_thread);
    }

    while (true)
    {
        /* Update memory usage */
        memory_usage_update();

        /* Check if printer port status has changed */
        if (device_printer && port_status != device_printer_port_status.value)
        {
            status = ux_device_class_printer_ioctl(device_printer, UX_DEVICE_CLASS_PRINTER_IOCTL_PORT_STATUS_SET,\
                                                   (void *)&device_printer_port_status.value);
            /* Error handle */
            if (UX_SUCCESS != status)
            {
                PRINT_ERR_STR("ux_device_class_printer_ioctl API failed.");
                ERROR_TRAP(status);
            }
            port_status = device_printer_port_status.value;
            sprintf(print_buff, "Memory usage: %d", (int)port_status);
            PRINT_INFO_STR(print_buff);
            memset(print_buff, RESET_VALUE, sizeof(print_buff));
        }

        tx_thread_sleep(1);

        /* Check elapsed time and update speed every 1 second */
        tick1 = tx_time_get();
        diff  = time_diff(tick1, tick0, TIME_DELAY);
        if (diff < TX_TIMER_TICKS_PER_SECOND)
        {
            continue;
        }
        tick0 = tick1;

        /* Print results */
        if ((pmem != mem_usage.mem_usage_max))
        {
            sprintf(print_buff, "Memory usage: %ld", (ULONG) pmem);
            PRINT_INFO_STR(print_buff);
            memset(print_buff, RESET_VALUE, sizeof(print_buff));
            pmem = mem_usage.mem_usage_max;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     Displays the memory used while printing a specific file.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void memory_usage_update(void)
{
    uint32_t mem_total  = RESET_VALUE;

    /* Update memory usage */
    mem_total = _ux_system -> ux_system_memory_byte_pool[UX_MEMORY_BYTE_POOL_REGULAR] -> ux_byte_pool_size + (uint32_t)(
            (uint8_t *)_ux_system -> ux_system_memory_byte_pool[UX_MEMORY_BYTE_POOL_REGULAR] -> ux_byte_pool_start -
            (uint8_t *)_ux_system);
    mem_usage.mem_usage = mem_total - _ux_system -> \
                          ux_system_memory_byte_pool[UX_MEMORY_BYTE_POOL_REGULAR] -> ux_byte_pool_available;

#ifdef UX_ENABLE_MEMORY_STATISTICS
    mem_usage.mem_usage_max = mem_total - _ux_system -> \
                              ux_system_memory_byte_pool[UX_MEMORY_BYTE_POOL_REGULAR] -> ux_byte_pool_min_free;
#else   /* Not accurate — memory may be allocated or freed between checks */
    if (mem_usage.mem_usage > mem_usage.mem_usage_max)
        mem_usage.mem_usage_max = mem_usage.mem_usage;
#endif
}

/*******************************************************************************************************************//**
 * @brief     Calculates the time difference to generate 1 second delay.
 * @param[in] v_origin.
 * @param[in] v_minus.
 * @param[in] wrap.
 * @retval    Time difference.
 **********************************************************************************************************************/
static uint32_t time_diff(uint32_t v_origin, uint32_t v_minus, uint32_t wrap)
{
    if (v_origin >= v_minus)
    {
        return(v_origin - v_minus);
    }
    else
    {
        return(v_origin + (wrap - v_minus));
    }
}

/*******************************************************************************************************************//**
 * @brief     Activates the printer instance.
 * @param[in] printer_instance    Pointer to the structure that stores the instance information.
 * @retval    None.
 **********************************************************************************************************************/
static void ux_printer_instance_activate(void *printer_instance)
{
    UINT status        = UX_SUCCESS;
    if (device_printer == UX_NULL)
    {
        device_printer = (UX_DEVICE_CLASS_PRINTER *)printer_instance;
        status = ux_device_class_printer_ioctl(device_printer, UX_DEVICE_CLASS_PRINTER_IOCTL_PORT_STATUS_SET,\
                                               (void*)&device_printer_port_status.value);

        /* Error handle */
        if (UX_SUCCESS != status)
        {
            PRINT_ERR_STR("ux_device_class_printer_ioctl API failed");
            ERROR_TRAP(status);
        }
    }
    port_status = device_printer_port_status.value;
    PRINT_INFO_STR("ux_printer_instance is activated");
}

/*******************************************************************************************************************//**
 * @brief     Deactivates the printer instance.
 * @param[in] printer_instance    Pointer to the structure that stores the instance information.
 * @retval    None.
 **********************************************************************************************************************/
static void ux_printer_instance_deactivate(void *printer_instance)
{
    if ((void*)device_printer == printer_instance)
    {
        device_printer = UX_NULL;
    }
    PRINT_INFO_STR("ux_printer_instance is deactivated");
}

/*******************************************************************************************************************//**
 * @brief       This function does nothing in particular.
 *              (This function is called when a SOFT_RESET request is received)
 * @param[in]   printer_instance    Pointer to area store the instance pointer.
 * @retval      None.
 **********************************************************************************************************************/
static void ux_printer_soft_reset(void *printer_instance)
{
    FSP_PARAMETER_NOT_USED(printer_instance);
    PRINT_INFO_STR("ux_printer_soft_reset");
}

/*******************************************************************************************************************//**
 * @brief       USB callback function for USB status change.
 * @param[in]   ULONG status    USB status code.
 * @retval      UX_SUCCESS.
 **********************************************************************************************************************/
static UINT usbx_status_callback(ULONG status)
{
    switch (status)
    {
        case UX_DEVICE_ATTACHED:
        {
            PRINT_INFO_STR("UX_DEVICE_ATTACHED");
            break;
        }
        case UX_DEVICE_CONFIGURED:
        {
            PRINT_INFO_STR("UX_DEVICE_CONFIGURED");
            break;
        }
        case UX_DEVICE_REMOVED:
        {
            PRINT_INFO_STR("UX_DEVICE_REMOVED");
            break;
        }
        case UX_DEVICE_SUSPENDED:
        {
            PRINT_INFO_STR("UX_DEVICE_SUSPENDED");
            break;
        }
        case UX_DEVICE_RESUMED:
        {
            PRINT_INFO_STR("UX_DEVICE_RESUMED");
            break;
        }
        default:
        {
            PRINT_INFO_STR("UX_DEVICE CONFIGURATION FAILED");
            return UX_ERROR;
        }
    }
    return UX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup usbx_pprn_ep)
 **********************************************************************************************************************/
