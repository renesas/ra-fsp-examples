#include "phid_main_thread.h"
#include "common_utils.h"
#include "usb_phid_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup usb_phid_ep
 * @{
 **********************************************************************************************************************/
/* external variables*/
extern uint8_t g_apl_configuration[];
extern uint8_t g_apl_report[];
extern bool b_write_flag;

/* private function declarations */
static void usb_enumeration(void);
static void usb_status_update(void);
static void usb_write_operation(void);
static void deinit_usb(void);

/* private variables */
static uint8_t send_data[BUFF_SIZE] BSP_ALIGN_VARIABLE(ALIGN);

static uint8_t * p_idle_value = NULL;
static usb_event_info_t * p_usb_phid_event = NULL;

static uint8_t g_idle = RESET_VALUE;
static uint8_t g_buf[DATA_LEN]  = {RESET_VALUE}; /* HID NULL data */
static uint8_t g_data[DATA_LEN] = {RESET_VALUE};
static uint16_t g_numlock = RESET_VALUE;

/* Phid Thread entry function */
/* pvParameters contains TaskHandle_t */
void phid_main_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    static uint8_t idle_value = RESET_VALUE;
    static usb_event_info_t g_usb_phid_event  = {.module_number = RESET_VALUE};
    /* Make sure the pointers as assigned to valid memories before accessing */
    p_idle_value = &idle_value;
    p_usb_phid_event = &g_usb_phid_event;

    fsp_err_t err = FSP_SUCCESS;
    /* Open USB instance */
    err = R_USB_Open(&g_basic_ctrl, &g_basic_cfg);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Open failed\r\n");
        APP_ERR_TRAP(err);
    }

    while (true)
    {
        /* check for usb event */
        switch (p_usb_phid_event->event)
        {
            /* Configured event */
            case USB_STATUS_CONFIGURED:
            {
                APP_PRINT("\r\nUSB Status Configured Successful\r\n");
                break;
            }
            /* Receive Class Request */
            case USB_STATUS_REQUEST:
            {
                usb_enumeration();
                break;
            }
            /* Complete Class Request */
            case USB_STATUS_REQUEST_COMPLETE:
            {
                usb_status_update();
                break;
            }
            /* Write complete event */
            case USB_STATUS_WRITE_COMPLETE:
            {
                vTaskDelay(DELAY/portTICK_PERIOD_MS);

                usb_write_operation();
                break;
            }
            /* Suspend event */
            case USB_STATUS_SUSPEND:
            {
                APP_PRINT("\r\nUSB STATUS : SUSPEND\r\n");
                break;
            }
            /* detach event */
            case USB_STATUS_DETACH:
            {
                APP_PRINT("\r\nUSB STATUS : DETACH\r\n");
                break;
            }
            default:
            {
                break;
            }
        }
        vTaskDelay (TASK_DELAY);
    }
}

/*******************************************************************************************************************//**
 * @brief       usb phid callback
 * @param[IN]   usb_event_info_t    usb event
 * @param[IN]   usb_hdl_t           task
 * @param[IN]   usb_onoff_t         state
 * @retval      None
 **********************************************************************************************************************/
void usb_phid_callback(usb_event_info_t * p_usb_event , usb_hdl_t task, usb_onoff_t state)
{
    FSP_PARAMETER_NOT_USED (task);
    FSP_PARAMETER_NOT_USED (state);
    /* capture the usb event info */
    p_usb_phid_event = p_usb_event;

}

/*******************************************************************************************************************//**
 * @brief       usb enumeration
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_enumeration(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* check for request type */
    if (USB_SET_REPORT == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Get the NumLock data */
        err = R_USB_Read(&g_basic_ctrl, (uint8_t *) &g_numlock, SIZE_NUM, USB_CLASS_PHID);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Read failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
    else if (USB_GET_DESCRIPTOR == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* check for request value */
        if (USB_GET_REPORT_DESCRIPTOR == p_usb_phid_event->setup.request_value)
        {
            err = R_USB_PeriControlDataSet (&g_basic_ctrl,(uint8_t *)g_apl_report, USB_RECEIVE_REPORT_DESCRIPTOR);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
        else if (USB_GET_HID_DESCRIPTOR == p_usb_phid_event->setup.request_value)
        {
            for (uint8_t hid_des = RESET_VALUE; hid_des < USB_RECEIVE_HID_DESCRIPTOR; hid_des++)
            {
                send_data[hid_des] = g_apl_configuration[CD_LENGTH + hid_des];
            }

            /* Configuration Descriptor address set. */
            err = R_USB_PeriControlDataSet(&g_basic_ctrl, send_data, USB_RECEIVE_HID_DESCRIPTOR);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_PeriControlDataSet failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
        else
        {
            /* None */
        }
    }
    else if (USB_SET_IDLE == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* Get SetIdle value */
        p_idle_value = (uint8_t *) &p_usb_phid_event->setup.request_value;

        g_idle = p_idle_value[IDLE_VAL_INDEX];

        err = R_USB_PeriControlStatusSet(p_usb_phid_event, USB_SETUP_STATUS_ACK);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_PeriControlStatusSet failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
    else
    {
        /* None */
    }
}

/*******************************************************************************************************************//**
 * @brief       usb status update
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_status_update(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* check for request type */
    if (USB_SET_IDLE == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        p_idle_value = (uint8_t *) &p_usb_phid_event->setup.request_value;

        g_idle = p_idle_value[IDLE_VAL_INDEX];
    }
    else if (USB_SET_PROTOCOL == (p_usb_phid_event->setup.request_type & USB_BREQUEST))
    {
        /* None */
    }
    else
    {
        /* Sending the zero data (8 bytes) */
        err = R_USB_Write(&g_basic_ctrl, g_buf, DATA_LEN, USB_CLASS_PHID);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       usb write operation
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void usb_write_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* check flag from user input */
    if (true == b_write_flag)
    {
        /* Usage ID of A */
        static uint8_t data = USAGE_ID_A;

        if (USAGE_ID_0 >= data)
        {
            /* copy data to 3rd Index of buffer*/
            g_data[INDEX_THIRD] = data;
            /* increment the data */
            data++;
            /* Sending the 8 bytes data */
            err = R_USB_Write(&g_basic_ctrl, (uint8_t *) g_data, DATA_LEN, USB_CLASS_PHID);
            if (FSP_SUCCESS != err)
            {
                APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
                deinit_usb();
                APP_ERR_TRAP(err);
            }
        }
        else
        {
            data = USAGE_ID_A;
            b_write_flag = false;
        }
    }
    else
    {
        /* Sending the zero data (8 bytes) */
        err = R_USB_Write(&g_basic_ctrl, (uint8_t *) g_buf, DATA_LEN, USB_CLASS_PHID);
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nR_USB_Write failed\r\n");
            deinit_usb();
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       de-initialize USB
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void deinit_usb(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* close usb instance */
    err = R_USB_Close(&g_basic_ctrl);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_USB_Close\r\n ");
    }
}
