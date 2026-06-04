/***********************************************************************************************************************
 * File Name    : camera_thread_entry.c
 * Description  : The functionality of this file is handle for Camera operation of the application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "camera_thread.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "ov3640.h"
#include "common_utils.h"
#include "usr_app.h"
#include "JPEGENC.h"
#include "jpeg.h"

/* Frame buffer */
#define FRAME_SIZE_BUFFER               (1024U * 64U)

/* VGA resolution definition */
#define IMAGE_WIDTH                     (640U)
#define IMAGE_HEIGHT                    (480U)
#define BYTE_PER_PIXEL                  (2U)

/***********************************************************************************************************************
 * Exported global variables
 ***********************************************************************************************************************/
extern const sensor_reg_t g_ov3640_fmt_YUV422[];
extern const sensor_reg_t g_ov3640_fmt_JPEG[];
extern const sensor_reg_t g_ov3640_VGA[];

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
/* Current method status variable */
static uint8_t g_current_method;

/* Variables for OV3640 sensor encoding method (using sensor encoding engine) */
static size_t g_last_jpeg_size = FRAME_SIZE_BUFFER;
static uint8_t jpeg_image_buffer[FRAME_SIZE_BUFFER] BSP_ALIGN_VARIABLE(8);

/* Variables for MCU encoding method (using third-party software) */
static JPEGE_IMAGE jpg;
static JPEGENCODE jpe;
static uint8_t yuv_buffer[IMAGE_WIDTH * IMAGE_HEIGHT * BYTE_PER_PIXEL] BSP_ALIGN_VARIABLE(8);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static volatile bool g_camera_running = true;
static volatile bool g_camera_initialized = false;
static volatile bool g_stream_active = false;
static capture_instance_t const *gp_ceu_instance = NULL;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void encode_method_selection(void);
static void handle_error(fsp_err_t err, const char *err_str);
static fsp_err_t capture_image(capture_instance_t const *p_instance, uint8_t *const p_buffer);
static size_t get_jpeg_image_size(uint8_t *jpeg_data);
static fsp_err_t camera_configuration(uint8_t current_method);
static int encode_yuv_to_jpeg(uint32_t image_width, uint32_t image_height, uint8_t *pixel, uint8_t *encode_jpeg_buffer);
static void cleanup_camera_resources(void);

/*******************************************************************************************************************//**
 *  @brief      g_ceu_mcu_encode_callback callback function, used to notify that a frame capture has completed.
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void g_ceu_mcu_encode_callback (capture_callback_args_t * p_args)
{
    if (CEU_EVENT_FRAME_END == p_args->event )
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(g_ceu_semaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*******************************************************************************************************************//**
 *  @brief      g_ceu_jpeg_callback callback function, used to notify that a frame capture has completed.
 *  @param[in]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void g_ceu_sensor_encode_callback (capture_callback_args_t * p_args)
{
    if (CEU_EVENT_FRAME_END == p_args->event )
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(g_ceu_semaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*******************************************************************************************************************//**
 *  @brief      This is User Camera Thread
 *  @param[in]  Thread specific parameters
 *  @retval     None
 **********************************************************************************************************************/
void camera_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_err_t           err             = FSP_SUCCESS;
    bool                demo_running    = true;
    jpeg_transfer_t     jpeg_transfer;

    /* Wait for network establish */
    xEventGroupWaitBits(g_system_event,
                        EVT_NET_READY,
                        pdFALSE,
                        pdTRUE,
                        portMAX_DELAY);
    APP_PRINT("\r\nNetwork ready, Start initialize camera...\r\n");

    while (demo_running)
    {
        /* Ask user to select the encoding method */
        encode_method_selection();

        /* Initialize necessary configuration for camera */
        err = camera_configuration(g_current_method);
        if (err != FSP_SUCCESS)
        {
            APP_ERR_PRINT(" ** Camera configuration failed ** \r\n");
            break;
        }

        /* Assign camera status */
        g_camera_initialized = true;
        g_stream_active = false;

        /* Notify HTTP Server that camera is ready */
        xEventGroupSetBits(g_system_event, EVT_CAM_READY);
        vTaskDelay(pdMS_TO_TICKS(500));

        /* Waiting for start capture request */
        xEventGroupWaitBits(g_system_event,
                            EVT_CAM_START,
                            pdTRUE,
                            pdTRUE,
                            portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));

        /* Print Control menu
         * 1: Stop current stream to change encode method
         * 2: End demo */
        APP_PRINT(CONTROL_MENU);

        /* Active stream */
        g_stream_active = true;

        /* Streaming loop */
        while (demo_running && g_stream_active)
        {
            /* Check for user commands */
            if (APP_CHECK_DATA)
            {
                char user_input[32] = {0};
                APP_READ(user_input);

                if (user_input[0] == OPTION_STOP_STREAM)
                {
                    APP_PRINT("\r\nStream stopped...\r\n");
                    APP_PRINT("\r\nPlease select Encoding method then refresh the browser !!!\r\n");
                    jpeg_transfer_t dummy;

                    /* Stop current stream  */
                    g_stream_active = false;

                    /* Signal HTTP server to disconnect all clients */
                    xEventGroupSetBits(g_system_event, EVT_CAM_STOP);

                    /* Wait a moment for clients to disconnect */
                    vTaskDelay(pdMS_TO_TICKS(500));

                    /* Clear JPEG queue */
                    while (xQueueReceive(g_jpeg_queue, &dummy, 0) == pdTRUE)
                    {
                        /* Drain queue */
                    }

                    /* Cleanup current camera configuration */
                    cleanup_camera_resources();

                    break; /* Exit streaming loop, return to method selection */
                }
                else if (user_input[0] == OPTION_END_DEMO)
                {
                    APP_PRINT("\r\nDemo Ending...\r\n");
                    g_stream_active = false;
                    demo_running = false;

                    /* Cleanup current camera configuration */
                    cleanup_camera_resources();

                    /* Signal HTTP server to shutdown */
                    xEventGroupSetBits(g_system_event, EVT_DEMO_END);

                    break; /* Exit streaming loop and demo loop to end the current demo */
                }
                else
                {
                    APP_PRINT("\r\nInvalid option. Try again.\r\n");
                    APP_PRINT(CONTROL_MENU);
                }
            }

            /* Start capture and send JPEG image to HTTP Thread depending on current encode method selection */
            /* MCU Encode method selected */
            if (g_current_method == SELECT_MCU_ENCODE)
            {
                /* Capture YUV image and save to yuv_buffer */
                err = capture_image(gp_ceu_instance, yuv_buffer);
                if (err != FSP_SUCCESS)
                {
                    APP_ERR_PRINT(" ** Capture failed ** \r\n");
                    vTaskDelay(pdMS_TO_TICKS(10)); /* Brief delay before retry */
                    continue;
                }

                /* Encode YUV to JPEG */
                int jpeg_size = encode_yuv_to_jpeg(IMAGE_WIDTH, IMAGE_HEIGHT, yuv_buffer, jpeg_image_buffer);
                if (jpeg_size <= 0)
                {
                    APP_ERR_PRINT(" ** JPEG encoding failed ** \r\n");
                    continue;
                }

                /* Assign buffer to transfer to HTTP Server Thread */
                jpeg_transfer.jpeg_address = jpeg_image_buffer;
                jpeg_transfer.size = (size_t)jpeg_size;

                /* Send to HTTP server queue */
                if (xQueueSend(g_jpeg_queue, &jpeg_transfer, pdMS_TO_TICKS(100)) != pdTRUE)
                {
                    continue;
                }
            }
            /* Sensor Encode method selected */
            else if (g_current_method == SELECT_SENSOR_ENCODE)
            {
                /* Capture JPEG image directly from sensor */
                err = capture_image(gp_ceu_instance, jpeg_image_buffer);
                if (err != FSP_SUCCESS)
                {
                    APP_ERR_PRINT(" ** Capture failed ** \r\n");
                    vTaskDelay(pdMS_TO_TICKS(10)); /* Brief delay before retry */
                    continue;
                }

                /* Get JPEG size with optimized scan */
                size_t jpeg_size = get_jpeg_image_size(jpeg_image_buffer);
                if (jpeg_size == 0)
                {
                    APP_ERR_PRINT(" ** Invalid JPEG size ** \r\n");
                    continue;
                }

                /* Assign buffer to transfer to HTTP Server Thread */
                jpeg_transfer.jpeg_address = jpeg_image_buffer;
                jpeg_transfer.size = jpeg_size;

                /* Send to HTTP server queue */
                if (xQueueSend(g_jpeg_queue, &jpeg_transfer, pdMS_TO_TICKS(100)) != pdTRUE)
                {
                    continue;
                }
            }

            /* End thread time slice to optimize stream pipeline */
            taskYIELD();
        }

        /* If demo is ending, exit the main loop */
        if (!demo_running)
        {
            break;
        }
    }
    /* Thread cleanup */
    cleanup_camera_resources();

    /* Clear camera status */
    g_camera_running = false;
    g_camera_initialized = false;

    /* Remove Thread */
    vTaskDelete(NULL);
}

/*******************************************************************************************************************//**
 *  @brief      This function ask user to select the encode method. There are 2 methods in this application project
 *              is MCU encode method and SENSOR encode method.
 *  @param[in]  None
 *  @retval     None
 **********************************************************************************************************************/
static void encode_method_selection(void)
{
    uint8_t rtt_read[TERMINAL_BUFFER] = {RESET_VALUE};

    /* Select type of encode the image */
    APP_PRINT(METHOD_MENU);

    while(true)
    {
        if(APP_CHECK_DATA)
        {
            /* Clean terminal input buffer */
            memset(rtt_read, NULL_CHAR, TERMINAL_BUFFER);

            /* Read terminal input data */
            APP_READ(rtt_read);

            /* Selected MCU as the method to encode image */
            if(SELECT_MCU_ENCODE == rtt_read[ZERO_INDEX])
            {
                g_current_method = SELECT_MCU_ENCODE;
                gp_ceu_instance = &g_ceu_mcu_encode;
                APP_PRINT("\r\nSelected encode method: MCU Encode Method.\r\n");
                return;
            }
            /* Selected Sensor as the method to encode image */
            else if (SELECT_SENSOR_ENCODE == rtt_read[ZERO_INDEX])
            {
                g_current_method = SELECT_SENSOR_ENCODE;
                gp_ceu_instance = &g_ceu_sensor_encode;
                APP_PRINT("\r\nSelected encode method: Sensor Encode Method.\r\n");
                return;
            }
            else
            {
                APP_PRINT("\r\nSelection is invalid !\r\n");
                APP_PRINT(METHOD_MENU);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function sets up GPT for camera clock, initializes I2C for sensor communication,
 *               configures the OV3640 sensor for capture format, and assigns the CEU instance depending on
 *               selected encode method.
 *  @param[in]   current_method     user selected encode method
 *  @retval      FSP_SUCCESS        Camera configure successfully
 **********************************************************************************************************************/
static fsp_err_t camera_configuration(uint8_t current_method)
{
    fsp_err_t err = FSP_SUCCESS;

    if(g_timer_periodic_ctrl.open == RESET_VALUE)
    {
        /* Initialize GPT module */
        err = R_GPT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
        handle_error(err, " ** R_GPT_Open API failed ** \r\n");

        /* Start GPT module to provide the 24MHz clock frequency output for the camera clock source */
        err = R_GPT_Start(&g_timer_periodic_ctrl);
        handle_error(err," ** R_GPT_Start API failed ** \r\n");
    }

    if(g_i2c_master_for_ov3640_ctrl.open == RESET_VALUE)
    {
        /* Initialize IIC module */
        err = R_IIC_MASTER_Open(&g_i2c_master_for_ov3640_ctrl, &g_i2c_master_for_ov3640_cfg);
        handle_error(err," ** R_IIC_MASTER_Open API failed ** \r\n");
    }

    /* Assign CEU instance pointer */
    if (current_method == SELECT_MCU_ENCODE)
    {
        /* Initialize OV3640 sensor with output format as YUV */
        err = ov3640_open(g_ov3640_fmt_YUV422);
        handle_error(err," ** ov3640 open operation failed ** \r\n");

        /* Set camera resolution specified by ov3640 array pointer */
        err = ov3640_set_resolution(g_ov3640_VGA);
        handle_error(err," ** ov3640 set resolution operation failed ** \r\n");
    }
    else if (current_method == SELECT_SENSOR_ENCODE)
    {
        /* Initialize OV3640 sensor with output format as JPEG */
        err = ov3640_open(g_ov3640_fmt_JPEG);
        handle_error(err," ** ov3640 open operation failed ** \r\n");

        /* Set camera resolution specified by ov3640 array pointer */
        err = ov3640_set_resolution(g_ov3640_VGA);
        handle_error(err," ** ov3640 set resolution operation failed ** \r\n");
    }

    /* Initialize CEU module with the configuration specified by CEU instance pointer */
    err = R_CEU_Open(gp_ceu_instance->p_ctrl, gp_ceu_instance->p_cfg);
    handle_error(err, " ** R_CEU_Open API failed ** \r\n");

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function handle error if error occurred, closes all opened modules, print and traps error.
 *  @param[in]   err        error status
 *  @param[in]   err_str    error string
 *  @retval      None
 **********************************************************************************************************************/
static void handle_error (fsp_err_t err,  const char * err_str)
{
    if(FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* Close sensor encode opened CEU module*/
        if(0U != g_ceu_sensor_encode_ctrl.open)
        {
            if(FSP_SUCCESS != R_CEU_Close(&g_ceu_sensor_encode_ctrl))
            {
                APP_ERR_PRINT(" ** R_CEU_Close API failed ** \r\n");
            }
        }

        /* Close MCU encode opened CEU module*/
        if(0U != g_ceu_mcu_encode_ctrl.open)
        {
            if(FSP_SUCCESS != R_CEU_Close(&g_ceu_mcu_encode_ctrl))
            {
                APP_ERR_PRINT(" ** R_CEU_Close API failed ** \r\n");
            }
        }

        /* Close opened I2C Master module*/
        if(0U != g_i2c_master_for_ov3640_ctrl.open)
        {
            if(FSP_SUCCESS != R_IIC_MASTER_Close(&g_i2c_master_for_ov3640_ctrl))
            {
                APP_ERR_PRINT(" ** R_IIC_MASTER_Close API failed ** \r\n");
            }
        }

        /* Close opened GPT module*/
        if(0U != g_timer_periodic_ctrl.open)
        {
            if(FSP_SUCCESS != R_GPT_Close(&g_timer_periodic_ctrl))
            {
                APP_ERR_PRINT(" ** R_GPT_Close API failed ** \r\n");
            }
        }

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function used to encode image from YUV format to JPEG format
 *  @param[in]   image_width        image width
 *  @param[in]   image_height       image height
 *  @param[in]   yuv_input_address  pointer to the input YUV image
 *  @retval      jpeg_size          pointer to the output JPEG image
 **********************************************************************************************************************/
static int encode_yuv_to_jpeg(uint32_t image_width,  uint32_t image_height, uint8_t *yuv_input_address, uint8_t *jpeg_output_address)
{
    int rc;
    int jpeg_size;

    /* Initialize JPEG encoder and output buffer in RAM */
    rc = JPEGOpenRAM(&jpg, jpeg_output_address, FRAME_SIZE_BUFFER);
    if (rc != JPEGE_SUCCESS)
    {
       /* Failed to initialize JPEG encoder */
       return -1;
    }

    /* Configure JPEG encoding parameters */
    rc = encodeBegin(&jpg,
                     &jpe,
                     (int)image_width,
                     (int)image_height,
                     JPEGE_PIXEL_YUV422,    /* Input pixel format */
                     JPEGE_SUBSAMPLE_420,   /* Output JPEG sub-sampling */
                     JPEGE_Q_HIGH);          /* JPEG quality */
    if (rc != JPEGE_SUCCESS)
    {
        /* Invalid image parameters or encoder failure */
        return -1;
    }

    /* Add raw YUV422 frame data to the encoder
     * YUV422 uses 2 bytes per pixel */
    addFrame(&jpg, &jpe, yuv_input_address, (int)image_width*2);

    /* Finalize JPEG encoding and get output size */
    jpeg_size = close(&jpg);

    return jpeg_size;
}

/*******************************************************************************************************************//**
 *  @brief       This function opens the CEU instance, starts image capture into the specified buffer.
 *  @param[in]   p_instance         CEU instance (MCU encode instance or Sensor encode instance)
 *  @param[in]   p_buffer           Specified buffer
 *  @retval      FSP_SUCCESS        Image captured successfully
 **********************************************************************************************************************/
static fsp_err_t capture_image (capture_instance_t const * p_instance, uint8_t * const p_buffer)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start capture image and store it in the buffer specified by image buffer pointer */
    err = R_CEU_CaptureStart(p_instance->p_ctrl, p_buffer);

    APP_ERR_RET(err, " ** R_CEU_CaptureStart API failed ** \r\n");

    BaseType_t status = xSemaphoreTake(g_ceu_semaphore, pdMS_TO_TICKS(1000));
    if (status != pdTRUE)
    {
        APP_ERR_RET(FSP_ERR_TIMEOUT, " ** CEU Callback event not received ** \r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function scans the JPEG data to find the End Of Image (EOI) marker (0xFFD9).
 *  @param[in]   jpeg_data      Pointer to the JPEG data
 *  @retval      Return size of the JPEG image in bytes, or 0 if invalid or marker not found.
 **********************************************************************************************************************/
static size_t get_jpeg_image_size(uint8_t *jpeg_data)
{
    if (jpeg_data == NULL) {
        return 0;
    }

    /* Validate SOI */
    if (jpeg_data[0] != 0xFF || jpeg_data[1] != 0xD8) {
        return 0;
    }

    /* Start from estimated position and scan backwards */
    size_t start_pos = (g_last_jpeg_size > 0) ? g_last_jpeg_size : 30000;

    /* Scan forward first (EOI likely near previous position) */
    for (size_t i = start_pos; i < start_pos + 10000 && i < FRAME_SIZE_BUFFER - 1; i++) {
        if (jpeg_data[i] == 0xFF && jpeg_data[i + 1] == 0xD9) {
            g_last_jpeg_size = i + 2;
            return i + 2;
        }
    }

    /* Scan backward if not found forward */
    if (start_pos > 10000) {
        for (size_t i = start_pos - 10000; i < start_pos; i++) {
            if (jpeg_data[i] == 0xFF && jpeg_data[i + 1] == 0xD9) {
                g_last_jpeg_size = i + 2;
                return i + 2;
            }
        }
    }

    /* Last resort: full scan */
    for (size_t i = 0; i < FRAME_SIZE_BUFFER - 1; i++) {
        if (jpeg_data[i] == 0xFF && jpeg_data[i + 1] == 0xD9) {
            g_last_jpeg_size = i + 2;
            return i + 2;
        }
    }

    return 0;
}

/*******************************************************************************************************************//**
 *  @brief      This function clean the camera configured resources.
 *  @param[in]  None
 *  @retval     None
 **********************************************************************************************************************/
static void cleanup_camera_resources(void)
{
    fsp_err_t err;

    /* Close CEU instance if initialized */
    if (g_camera_initialized && gp_ceu_instance != NULL)
    {
        err = R_CEU_Close(gp_ceu_instance->p_ctrl);
        if (err != FSP_SUCCESS)
        {
            APP_ERR_PRINT(" ** R_CEU_Close failed ** \r\n");
        }
    }

    /* Reset camera status */
    g_camera_initialized = false;
    gp_ceu_instance = NULL;
}
