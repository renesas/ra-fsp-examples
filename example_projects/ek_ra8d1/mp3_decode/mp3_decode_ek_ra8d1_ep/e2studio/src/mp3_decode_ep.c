/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include <app_thread.h>
#include "common_utils.h"
#include "mp3_decode_ep.h"
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include "minimp3_ex.h"
#include "ff_stdio.h"

/*--------------------------------------------------------------------------------------------------
 * Macros and Constants
 *-------------------------------------------------------------------------------------------------*/
#define FRAME_MAX_SAMPLES       (1152U)    /* Maximum MP3 frame samples per channel */
#define CHANNELS                (2U)       /* Stereo (duplicate mono if needed) */
#define BUFFER_SAMPLES          (FRAME_MAX_SAMPLES * CHANNELS)
#define BUFFER_WORDS            (BUFFER_SAMPLES)
#define PCM_BUFFER_BYTES        (BUFFER_WORDS * sizeof(int16_t))

/*--------------------------------------------------------------------------------------------------
 * Global and Static Variables
 *-------------------------------------------------------------------------------------------------*/
static rm_freertos_plus_fat_device_t  device;        /* FAT device structure */
FF_Disk_t                             my_disk;       /* FAT disk structure */
static FF_FILE                        * g_mp3_file;    /* Pointer to the open MP3 file */
static mp3dec_ex_t                    g_decoder;      /* MP3 decoder context */

static int16_t                        pcm_buf_a[BUFFER_WORDS];
static uint16_t                       pcm_dac_buf[2][BUFFER_SAMPLES];

volatile int                          dac_dma_active = 0;
volatile bool                         g_rm_freertos_plus_fat_insertion_events = false;
bool                                  g_usb_safely_ejected_flag = false;
extern transfer_info_t                g_transfer0_info;



static mp3_file_entry_t g_mp3_files[MAX_MP3_PATH_LEN];
static uint32_t         g_mp3_file_count = 0;

/*--------------------------------------------------------------------------------------------------
 * Function Prototypes
 *-------------------------------------------------------------------------------------------------*/
fsp_err_t               usb_init(void);
void                    init_freertos_plus_fat(void);
bool                    wait_for_usb_connection(void);
void                    get_mp3_filename(uint8_t * p_out_filename, size_t max_len);
void                    seek_audio(mp3dec_ex_t * p_decoder, int direction);

static void             fat_clean_up(void);
static void             fat_read(void * buf, size_t size, void * user_data);
static void             fat_seek(uint64_t position, void * user_data);
static fsp_err_t        set_audio_frequency(int sample_rate_hz);
static fsp_err_t        audio_hw_init(void);
static fsp_err_t        audio_hw_deinit(void);
static fsp_err_t        audio_playback_start(void);
static inline uint16_t  pcm_to_dac(int16_t s);
static FF_Error_t       format_usb_device(void);

void                    dac_dma_start_transfer(const uint16_t * data, size_t samples);
void                    dac_dma_wait_for_done(void);
static void             print_mp3_info(mp3dec_ex_t * p_decoder, uint32_t file_size_bytes);
static fsp_err_t        open_mp3_file(uint8_t * p_filename, size_t filename_len);
static void             wait_for_input(char * buf, size_t len);

/***********************************************************************************************************************
 * @brief      Decodes and plays back an MP3 file using the minimp3 library.
 *
 * @param[in]
 * @retval     FSP_SUCCESS on successful playback, or an error code if any step fails.
 **********************************************************************************************************************/
fsp_err_t minimp3_decode_operation(void)
{
    /* Initialize variables */
    fsp_err_t status = FSP_SUCCESS;
    bool playback_started = false;
    int active_buffer = 0;
    size_t samples_per_channel = 0;
    uint8_t mp3_filename[MAX_FILENAME_LEN] = {0};
    int mp3_status    = 0;

    g_transfer0_info.p_dest = (void * volatile)&R_DAC->DADR[0];

    /* Initialize audio hardware */
    status = audio_hw_init();
    APP_ERR_RET(status != FSP_SUCCESS, status, "Hardware initialization failed\r\n");

    status = open_mp3_file(mp3_filename, sizeof(mp3_filename));
    APP_ERR_RET(FSP_SUCCESS != status, status, "Failed to open MP3 file\r\n");

    /* Configure MP3 decoder I/O */
    mp3dec_io_t io = {
        .read = (void *)fat_read,
        .read_data = g_mp3_file,
        .seek = (void *)fat_seek,
        .seek_data = g_mp3_file
    };

    /* Initialize MP3 decoder */
    mp3_status = mp3dec_ex_open_cb(&g_decoder, &io, MP3D_DO_NOT_SCAN);
    APP_ERR_RET(mp3_status, FSP_ERR_NOT_OPEN, "MP3 decoder initialization failed.\r\n");

    print_mp3_info(&g_decoder, g_mp3_file->ulFileSize);

    /* Configure audio hardware sample rate for MP3 playback */
    status = set_audio_frequency(g_decoder.info.hz);
    APP_ERR_RET(status != FSP_SUCCESS, status, "Failed to configure audio sample rate for MP3 playback\r\n");

    APP_PRINT("\r\nStarting MP3 playback... Press any key to stop.\r\n");

    /* Main playback loop */
    while (true)
    {
        /* Check for user input to stop playback */
        if (APP_CHECK_KEY || !g_rm_freertos_plus_fat_insertion_events)
        {
            if (APP_CHECK_KEY)
            {
                uint8_t user_input[TERM_BUFFER_SIZE + 1] = {0};
                TERM_READ(user_input, sizeof(user_input));
                APP_PRINT("\r\nStopping playback...\r\n");
            }
            break;
        }

        /* Decode next MP3 frame */
        samples_per_channel = mp3dec_ex_read(&g_decoder, pcm_buf_a, BUFFER_SAMPLES);
        if (0 == samples_per_channel)
        {
            APP_PRINT("\rMP3 playback complete\r\n");
            break;
        }

        /* Convert PCM samples to DAC format */
        int next_buffer = active_buffer ^ 1;
        for (size_t i = 0; i < samples_per_channel; i++)
        {
            pcm_dac_buf[next_buffer][i] = pcm_to_dac(pcm_buf_a[i]);
        }

        /* Wait for previous buffer to complete */
        if (playback_started)
        {
            while (!dac_dma_active)
            {
                __WFI();
            }

            dac_dma_active = false;
        }

        /* Setup next DMA transfer */
        active_buffer = next_buffer;
        status = R_DMAC_Reset(&g_transfer1_ctrl, 
                             pcm_dac_buf[active_buffer],
                             (void * volatile)&R_DAC->DADR[0],
                             FRAME_MAX_SAMPLES);
        APP_ERR_RET(status != FSP_SUCCESS, status, "DMA reset failed\r\n");

        /* Start audio playback when the first buffer is ready */
        if (!playback_started)
        {
            status = audio_playback_start();
            APP_ERR_RET(status != FSP_SUCCESS, status, "Failed to start audio playback\r\n");

            playback_started = true;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
    status = audio_hw_deinit();
    APP_ERR_RET(status != FSP_SUCCESS, status, "audio_hw_deinit() failed\r\n");

    mp3dec_ex_close(&g_decoder);
    ff_fclose(g_mp3_file);

    return status;
}

/***********************************************************************************************************************
 * @brief  Check whether a file name has an ".mp3" extension (case-insensitive).
 *
 * @param[in] p_name   Pointer to a null-terminated file name string.
 *
 * @retval true        The file name has an ".mp3" extension.
 * @retval false       The file name does not have an ".mp3" extension or the input is invalid.
 **********************************************************************************************************************/
static bool has_mp3_extension(const char * p_name)
{
    const char * p_ext = strrchr(p_name, '.');
    if (NULL == p_ext)
    {
        return false;
    }

    p_ext++;

    if ((0 == strcasecmp(p_ext, "mp3")))
    {
        return true;
    }

    return false;
}

/*******************************************************************************************************************//**
 * @brief  Recursively scan a directory for MP3 files and add them to the global list.
 *
 * @param[in] p_dir   Pointer to a null-terminated directory path to scan (e.g. "/", "/music").
 **********************************************************************************************************************/
static void scan_mp3_in_directory(const char * p_dir)
{
    FF_FindData_t find_data;
    int32_t       result;

    result = ff_findfirst(p_dir, &find_data);
    if (0 != result)
    {
        return;
    }

    do
    {
        const char * p_name = find_data.pcFileName;

        /* Skip "." and ".." entries */
        if ((0 == strcmp(p_name, ".")) || (0 == strcmp(p_name, "..")))
        {
            result = ff_findnext(&find_data);
            continue;
        }

        /* Build full path: dir + "/" + name */
        char full_path[MAX_MP3_PATH_LEN] = {0};

        if (0 == strcmp(p_dir, "/"))
        {
            /* Root: avoid path like "//file" */
            snprintf(full_path, sizeof(full_path), "/%s", p_name);
        }
        else
        {
            snprintf(full_path, sizeof(full_path), "%s/%s", p_dir, p_name);
        }

        if (find_data.ucAttributes & FF_FAT_ATTR_DIR)
        {
            /* Recursive scan for subdirectories */
            scan_mp3_in_directory(full_path);
        }
        else
        {
            /* Regular file: check for .mp3 extension */
            if (has_mp3_extension(p_name))
            {
                if (g_mp3_file_count < MAX_MP3_FILES)
                {
                    strncpy(g_mp3_files[g_mp3_file_count].path,
                            full_path,
                            MAX_MP3_PATH_LEN - 1);
                    g_mp3_files[g_mp3_file_count].path[MAX_MP3_PATH_LEN - 1] = '\0';
                    g_mp3_file_count++;
                }
            }
        }

        result = ff_findnext(&find_data);
    } while ((0 == result) && (0 == find_data.xDirectoryHandler.u.bits.bEndOfDir));
}

/***********************************************************************************************************************
 * @brief           Open an MP3 file for playback.
 * @param[in,out]   p_filename      Pointer to a buffer used to store the selected file name.
 * @param[in]       filename_len    Size of the filename buffer in bytes.
 * @retval          FSP_SUCCESS     File was opened successfully.
 **********************************************************************************************************************/
static fsp_err_t open_mp3_file(uint8_t * p_filename, size_t filename_len)
{
    int32_t fat_err = pdFREERTOS_ERRNO_NONE;
    char user_input[32] = {0};
    int chosen = 0;

    /* Reset the MP3 file list */
    memset(g_mp3_files, 0, sizeof(g_mp3_files));
    g_mp3_file_count = 0U;

    scan_mp3_in_directory("/");

    if (0U == g_mp3_file_count)
    {
        APP_PRINT("\r\nNo MP3 files found on the media.\r\n");
        return false;
    }

    APP_PRINT("\r\n[MP3 File List]\r\n");
    for (uint32_t i = 0; i < g_mp3_file_count; i++)
    {
        APP_PRINT("  %2u) %s\r\n", (unsigned) (i + 1U), g_mp3_files[i].path);
    }

    APP_PRINT("Enter the file index: ");

    do
    {
        APP_PRINT("\r\nSelect a file index (1-%u): ", (unsigned) g_mp3_file_count);

        /* Wait for user input and strip \r\n */
        wait_for_input(user_input, sizeof(user_input));
        chosen = atoi(user_input);

        if ((chosen <= 0) || ((uint32_t) chosen > g_mp3_file_count))
        {
            APP_PRINT("Invalid selection. Please try again.\n");
        }

    } while ((chosen <= 0) || ((uint32_t) chosen > g_mp3_file_count));

    strncpy((char *)p_filename, g_mp3_files[chosen - 1].path, filename_len - 1);
    p_filename[filename_len - 1] = '\0';

    /* Open the selected file */
    g_mp3_file = ff_fopen((char *)p_filename, "rb");
    if (NULL == g_mp3_file)
    {
        fat_err = stdioGET_ERRNO();
        APP_PRINT("\r\nFailed to open %s (errno: %ld)\r\n", p_filename, fat_err);
        return FSP_ERR_NOT_OPEN;
    }

    APP_PRINT("\r\nSelected: %s\r\n", p_filename);

    return FSP_SUCCESS;
}

/***********************************************************************************************************************
 * @brief   Print basic MP3 track information.
 *
 * @param[in] p_decoder        Pointer to an initialized mp3dec_ex_t decoder instance.
 * @param[in] file_size_bytes  MP3 file size in bytes.
 **********************************************************************************************************************/
static void print_mp3_info(mp3dec_ex_t * p_decoder, uint32_t file_size_bytes)
{
    /* Duration estimate assuming CBR based on file size and bitrate */
    double   duration_sec      = (double)(file_size_bytes * 8) /
                                 ((double) p_decoder->info.bitrate_kbps * 1000.0);
    uint32_t duration_minutes  = (uint32_t) (duration_sec / 60.0);
    uint32_t duration_seconds  = (uint32_t) (duration_sec - ((double) duration_minutes * 60.0));

    APP_PRINT(
        "\r\n[MP3 Track Information]\r\n"
        "  Duration      : %u:%02u\r\n"
        "  Sample rate   : %u Hz\r\n"
        "  Channels      : %u\r\n"
        "  Frame bitrate : %u kbps\r\n",
        (unsigned) duration_minutes,
        (unsigned) duration_seconds,
        (unsigned) p_decoder->info.hz,
        (unsigned) p_decoder->info.channels,
        (unsigned) p_decoder->info.bitrate_kbps);
}

/***********************************************************************************************************************
 * @brief      Convert a signed 16-bit PCM sample to a 12-bit DAC value (0..4095).
 *
 * @param[in]  s    Signed 16-bit PCM sample.
 * @retval     12-bit unsigned value suitable for the DAC.
 **********************************************************************************************************************/
static inline uint16_t pcm_to_dac(int16_t s)
{
    /* Ensure DEFAULT_VOLUME is bounded to [0.0, 1.0] */
    float vol = DEFAULT_VOLUME;
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;

    /* Apply volume scaling */
    int32_t scaled = (int32_t)((float)s * vol);

    /* Map from signed 16-bit (-32768..32767) to unsigned 0..65535 and clamp */
    int32_t unsigned16 = scaled + 32768;
    if (unsigned16 < 0) unsigned16 = 0;
    if (unsigned16 > 65535) unsigned16 = 65535;

    /* Convert to 12-bit range 0..4095 using 64-bit math to avoid overflow */
    uint16_t dac = (uint16_t)(((uint64_t)unsigned16 * 4095U) / 65535U);

    return dac;
}

/***********************************************************************************************************************
 * @brief      Configure the AGT timer to produce the requested audio sample frequency.
 *
 * @param[in]  sample_rate_hz  Desired audio sampling frequency in Hertz.
 * @retval     Status code from R_AGT_PeriodSet.
 **********************************************************************************************************************/
static fsp_err_t set_audio_frequency(int sample_rate_hz)
{
    uint32_t timer_freq_hz = R_FSP_SystemClockHzGet(FSP_PRIV_CLOCK_PCLKB) 
                            >> g_timer0_cfg.source_div;
    uint32_t period_counts = timer_freq_hz / (uint32_t)sample_rate_hz;
    
    return R_AGT_PeriodSet(&g_timer0_ctrl, period_counts);
}

/***********************************************************************************************************************
 * Function Name: wait_for_key_and_reinit
 * Description  : Waits for key press and reinitializes FreeRTOS+FAT
 ***********************************************************************************************************************/
void wait_for_key_and_reinit(void)
{
    APP_PRINT("\r\nPress any key (except 'T' or 't') to reinitialize FreeRTOS+FAT, "
              "\r\nor press 'T' or 't' to terminate the application.\r\n");

    while (true)
    {
        if (APP_CHECK_KEY)
        {
            uint8_t user_input[TERM_BUFFER_SIZE + 1] = {NULL_CHAR};
            TERM_READ(user_input, sizeof(user_input));

            if ((user_input[0] == 'T') || (user_input[0] == 't'))
            {
                APP_PRINT("\r\nApplication terminated.\r\n");
                vTaskSuspend(NULL);
                break;
            }
            else
            {
                init_freertos_plus_fat();
                break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*******************************************************************************************************************//**
 * @brief   Waits for user input from terminal/UART and reads it into a buffer.
 *          Trailing carriage return ('\r') and newline ('\n') characters are automatically removed.
 *
 * This function polls APP_CHECK_DATA and sleeps briefly to reduce CPU usage, making it suitable
 * for FreeRTOS tasks. It ensures the output buffer is always null-terminated.
 *
 * @param[out] buf  Pointer to the buffer to store user input.
 * @param[in]  len  Size of the buffer in bytes. Must be > 0.
 **********************************************************************************************************************/
static void wait_for_input(char * buf, size_t len)
{
    if ((buf == NULL) || (len == 0))
    {
        return;
    }

    /* Wait until terminal/UART has data */
    while (!APP_CHECK_DATA)
    {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    /* Read user input into buffer, leaving space for null terminator */
    TERM_READ((uint8_t *)buf, len - 1);

    /* Ensure buffer is null-terminated */
    buf[len - 1] = '\0';

    /* Strip trailing carriage return '\r' and newline '\n' characters */
    size_t str_len = strlen(buf);
    while ((str_len > 0) && ((buf[str_len - 1] == '\r') || (buf[str_len - 1] == '\n')))
    {
        buf[str_len - 1] = '\0';
        str_len--;
    }
}

/***********************************************************************************************************************
 * @brief      Prompt the user and obtain an MP3 file path from the terminal.
 *
 * @param[in]  p_out_filename  Pointer to the buffer containing the filename
 * @param[in]  max_len         Size of the output buffer in bytes
 * @retval     None.
 **********************************************************************************************************************/
void get_mp3_filename(uint8_t *p_out_filename, size_t max_len)
{
    uint8_t user_input[TERM_BUFFER_SIZE + 1] = { NULL_CHAR };  /* Local buffer for terminal input */
    size_t len = 0;

    APP_PRINT("\r\nEnter MP3 file path (e.g. song.mp3): ");

    /* Wait for user input */
    while (true)
    {
        if (APP_CHECK_DATA)
        {
            TERM_READ(user_input, sizeof(user_input));
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    /* Remove any newline or carriage return characters */
    len = strlen((char *) user_input);
    while ((len > 0) && ((user_input[len - 1] == '\n') || (user_input[len - 1] == '\r')))
    {
        user_input[--len] = NULL_CHAR;
    }

    /* Copy cleaned input to output buffer */
    strncpy((char *) p_out_filename, (char *) user_input, max_len - 1U);
    p_out_filename[max_len - 1U] = NULL_CHAR;

    APP_PRINT("\r\nSelected file: %s\r\n", p_out_filename);
}

/***********************************************************************************************************************
 * Function Name: handle_user_input
 * Description  : Handles terminal menu input from user
 ***********************************************************************************************************************/
void handle_user_input(void)
{
    fsp_err_t err = FSP_SUCCESS;
    FF_Error_t fat_err = FF_ERR_NONE;
    uint8_t user_input[TERM_BUFFER_SIZE + 1] = {NULL_CHAR};

    /* Read user input */
    TERM_READ(user_input, sizeof(user_input));

    switch (user_input[0])
    {
        case MP3_PLAY:
            err = minimp3_decode_operation();
            handle_fsp_error(err, "minimp3_decode_operation() failed\r\n");
            APP_PRINT(MINI_MP3_MENU);
            break;

        case USB_FORMAT:
            fat_err = format_usb_device();
            handle_fsp_error((fsp_err_t)fat_err, "format_usb_device() failed\r\n");
            break;

        case USB_SAFELY_EJECT:
            err = usb_safely_eject();
            handle_fsp_error(err, "usb_safely_eject() failed\r\n");
            wait_for_key_and_reinit();
            break;

        default:
            APP_PRINT("Invalid selection.\r\n");
            break;
    }
}

/*******************************************************************************************************************//**
 * @brief   Reads data from an open FAT file.
 *
 * @param[out]  buf         Pointer to the buffer where the read data will be stored.
 * @param[in]   size        Number of bytes to read from the file.
 * @param[in]   user_data   Pointer to the file handle.
 *
 * @retval      size_t      Number of bytes successfully read from the file.
 **********************************************************************************************************************/
static void fat_read(void *buf, size_t size, void *user_data)
{
    FF_FILE *fp = (FF_FILE *)user_data;
    ff_fread(buf, 1, size, fp);
}

/*******************************************************************************************************************
 * This function performs USB HMSC format operation.
 ******************************************************************************************************************/
FF_Error_t format_usb_device(void)
{
    APP_PRINT("\r\nUSB Device Formatting will be initiated. Formatting will take time "
              "\r\ndepending on USB Device capacity.\r\n");
    APP_PRINT("\r\nDo not disconnect the USB device while formatting is in progress."
              " \r\nPlease Wait ...\r\n");

    /* Formatting time varies with USB Device capacity. Might take longer time for higher capacity USB Device */
    FF_Error_t ff_error = FF_Format (&my_disk, my_disk.xStatus.bPartitionNumber, pdFALSE, pdFALSE);

    if (FF_ERR_NONE != ff_error)
    {

        APP_ERR_PRINT("\r\n FF_Format API failed  %d. Check the USB Device.\r\n", FF_GetErrMessage(ff_error));
    }
    else
    {
        APP_PRINT("\r\nUSB Device Formatted successfully \r\n");
    }

    return ff_error;
}

/***********************************************************************************************************************
 * @brief  Set file pointer to the specified position.
 *
 * @param[in]  position    Offset from the file start.
 * @param[in]  user_data   Pointer to the file handle (FF_FILE *).
 **********************************************************************************************************************/
static void fat_seek(uint64_t position, void * user_data)
{
    FF_FILE * fp = (FF_FILE *) user_data;
    ff_fseek(fp, (long) position, FF_SEEK_SET);
}

/**********************************************************************************************************************
 * @brief       This function Initializes the FreeRTOS+FAT instance..
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
void init_freertos_plus_fat(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    /* Open FreeRTOS PLUS FAT  */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Open (&g_rm_freertos_plus_fat_ctrl, &g_rm_freertos_plus_fat_cfg);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nFREERTOS PLUS FAT OPEN API failed\r\n");
        handle_fsp_error (freertos_fat_error, "\r\nInitialize the media device failed\r\n");
    }

    APP_PRINT("\r\nFreeRTOS+FAT Open successful\r\n");
    APP_PRINT("\r\nConnect USB Device...\r\n");

}

/**********************************************************************************************************************
 * @brief       This function Initializes the FreeRTOS+FAT instance..
 * @param[IN]   None
 * @retval      Any Other Error code apart from FSP_SUCCESS on Unsuccessful operation.
 *********************************************************************************************************************/
fsp_err_t usb_init(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;
    int32_t file_error = SUCCESS;

    memset (&device, RESET_VALUE, sizeof(device));
    memset (&my_disk, RESET_VALUE, sizeof(my_disk));

    /* Initialize the mass storage device.  This should not be done until the device is plugged in and initialized. */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_MediaInit (&g_rm_freertos_plus_fat_ctrl, &device);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nFreeRTOS Plus FAT Media Init API failed\r\n");
        fat_clean_up ();

        return freertos_fat_error;
    }

    /* Initialize one disk for each partition used in the application. */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskInit (&g_rm_freertos_plus_fat_ctrl,
                                                        &g_rm_freertos_plus_fat_disk_cfg, &my_disk);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nFreeRTOS Plus FAT Disk Init API failed\r\n");
        fat_clean_up ();

        return freertos_fat_error;
    }

    /* Mount each disk.  This assumes the disk is already partitioned and formatted. */
    FF_Error_t ff_err = FF_Mount (&my_disk, my_disk.xStatus.bPartitionNumber);
    if (FSP_SUCCESS != ff_err)
    {
        APP_ERR_PRINT("\r\nFF_Mount API failed\r\n");

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        fat_clean_up ();

        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(ff_err);
    }

    /* Add the disk to the file system. */
    file_error = FF_FS_Add ("/", &my_disk);
    if (pdFALSE == file_error)
    {
        APP_ERR_PRINT("\r\nFF_Mount API failed\r\n");

        /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
        fat_clean_up ();

        /* As function returns fsp_err_t, ff_err cannot be returned. Hence trapping the error here */
        APP_ERR_TRAP(file_error);
    }

    return freertos_fat_error;
}

/*******************************************************************************************************************
 * This function closes the FreeRTOS+FAT instance.
 *******************************************************************************************************************/
static void fat_clean_up(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    /* Close the FREERTOS_PLUS_FAT_Close instance on any failure */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close (&g_rm_freertos_plus_fat_ctrl);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_PRINT("\r\nFREERTOS PLUS FAT CLOSE API failed\r\n");
    }
    else
    {
        APP_PRINT("\r\nFREERTOS PLUS FAT instance Closed successfully.\r\n");
    }
}

/***********************************************************************************************************************
 * @brief  Safely ejects the connected USB device.
 *
 * @retval  FSP_SUCCESS           USB was safely ejected.
 * @retval  Any other error code  If deinitialization or closing the FAT instance failed.
 **********************************************************************************************************************/
fsp_err_t usb_safely_eject(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Perform safe eject once reconnected */
    APP_PRINT("\r\nPerforming safe eject...\r\n");

    /* Deinitialize disk */
    err = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat_ctrl, &my_disk);
    APP_ERR_RET(err != FSP_SUCCESS, err, "DiskDeinit failed\r\n");

    /* Close FAT instance */
    err = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat_ctrl);
    APP_ERR_RET(err != FSP_SUCCESS, err, "Close failed\r\n");

    /* Clear event flag */
    xEventGroupClearBitsFromISR(g_usb_event_group, USB_INSERT_BIT | USB_REMOVE_BIT);

    g_usb_safely_ejected_flag = true;
    APP_PRINT("USB safely ejected — it is now safe to remove\r\n");

    return err;
}


/***********************************************************************************************************************
 *  Function Name: handle_fsp_error
 *  Description  : If an error occurred, this function handles an error by closing all opened modules, prints the error,
 *                 and traps on the error.
 *  Arguments    : err             error status
 *                 err_str         error string
 *  Return Value : None
 **********************************************************************************************************************/
void handle_fsp_error(fsp_err_t err, char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print error information and error code */
        APP_ERR_PRINT(err_str);

        /* Close the FREERTOS_PLUS_FAT instance on any failure */
        if (MODULE_CLOSE != g_rm_freertos_plus_fat_ctrl.open)
        {
            if (FSP_SUCCESS != RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat_ctrl))
            {
                APP_ERR_PRINT ("\r\nFREERTOS PLUS FAT CLOSE API failed\r\n");
            }
        }

        /* De-initialize DAC if initialized */
        if (MODULE_CLOSE == g_dac0_ctrl.channel_opened)
        {
            if (FSP_SUCCESS != R_DAC_Close(&g_dac0_ctrl))
            {
                APP_ERR_PRINT("\r\nDAC Close API failed\r\n");
            }
        }

        /* De-initialize DMAC if initialized */
        if (MODULE_CLOSE == g_transfer1_ctrl.open)
        {
            if (FSP_SUCCESS != R_DMAC_Close(&g_transfer1_ctrl))
            {
                APP_ERR_PRINT("\r\nDMAC Close API failed\r\n");
            }
        }

        /* De-initialize GPT if initialized */
        if (MODULE_CLOSE == g_timer0_ctrl.open)
        {
            if (FSP_SUCCESS != R_AGT_Close(&g_timer0_ctrl))
            {
                APP_ERR_PRINT("\r\nGPT Close API failed\r\n");
            }
        }

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}

/***********************************************************************************************************************
 * @brief      Initializes all necessary hardware peripherals for the application.
 *
 * @param[in]
 * @retval     FSP_SUCCESS on successful initialization of all peripherals, or an error code if any step fails.
 **********************************************************************************************************************/
static fsp_err_t audio_hw_init(void) {
    fsp_err_t status = FSP_SUCCESS;

    /* Initialize the AGT */
    status = R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    APP_ERR_RET(status != FSP_SUCCESS, status, "R_AGT_Open API failed\r\n");

    /* Initialize the DMAC */
    status = R_DMAC_Open(&g_transfer1_ctrl, &g_transfer1_cfg);
    APP_ERR_RET(status != FSP_SUCCESS, status, "R_DMAC_Open API failed\r\n");

    /* Initialize the DAC */
    status = R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    APP_ERR_RET(status != FSP_SUCCESS, status, "R_DAC_Open API failed\r\n");

    return status;
}

/***********************************************************************************************************************
 * @brief      De-initializes all hardware peripherals used by the audio application.
 *
 * @retval     FSP_SUCCESS if all peripherals are successfully de-initialized,
 *             or an error code indicating which step failed.
 **********************************************************************************************************************/
static fsp_err_t audio_hw_deinit(void)
{
    fsp_err_t status = FSP_SUCCESS;

    /* De-initialize the AGT module */
    status = R_AGT_Close(&g_timer0_ctrl);
    APP_ERR_RET(status != FSP_SUCCESS, status, "R_AGT_Close API failed\r\n");

    /* De-initialize the DMAC module */
    status = R_DMAC_Close(&g_transfer1_ctrl);
    APP_ERR_RET(status != FSP_SUCCESS, status, "R_DMAC_Close API failed\r\n");

    /* De-initialize the DAC module */
    status = R_DAC_Close(&g_dac0_ctrl);
    APP_ERR_RET(status != FSP_SUCCESS, status, "R_DAC_Close API failed\r\n");

    return status;
}

/***********************************************************************************************************************
 * @brief  Starts audio playback by enabling the DAC and AGT timer.
 *
 * @retval  FSP_SUCCESS           Playback started successfully.
 * @retval  Any other error code  If DAC or AGT start operation fails.
 **********************************************************************************************************************/
static fsp_err_t audio_playback_start(void)
{

    fsp_err_t status = FSP_SUCCESS;

    /* Start DAC for audio output */
    status = R_DAC_Start(&g_dac0_ctrl);
    APP_ERR_RET(status != FSP_SUCCESS, status, "R_DAC_Start API failed\r\n");

    /* Start AGT timer to generate DMA trigger at the sample rate */
    status = R_AGT_Start(&g_timer0_ctrl);
    APP_ERR_RET(status != FSP_SUCCESS, status, "R_AGT_Start API failed\r\n");

    return FSP_SUCCESS;
}

/***********************************************************************************************************************
 * @brief  DMA transfer completion callback.
 *
 * @param[in]  p_args   Pointer to the callback arguments (unused).
 **********************************************************************************************************************/
void transfer_callback(transfer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Transfer done */
    dac_dma_active = true;
}

/***********************************************************************************************************************
 * @brief     Callback function for FreeRTOS+FAT triggered on USB flash drive insertion or removal events.
 * @param[IN] p_args   Pointer to callback arguments containing the event type.
 * @retval    None.
 **********************************************************************************************************************/
void free_rtos_fat_callback(rm_freertos_plus_fat_callback_args_t *p_args)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_INSERTED)
    {
        APP_PRINT("\r\nUSB flash drive attached.\r\n");

        g_rm_freertos_plus_fat_insertion_events = true;
        g_usb_safely_ejected_flag              = false;

        /* Clear REMOVE bit and set INSERT bit from ISR */
        xEventGroupClearBitsFromISR(g_usb_event_group, USB_REMOVE_BIT);
        xEventGroupSetBitsFromISR(g_usb_event_group, USB_INSERT_BIT, &xHigherPriorityTaskWoken);
    }
    else if (p_args->event & RM_FREERTOS_PLUS_FAT_EVENT_MEDIA_REMOVED)
    {
        APP_PRINT("\r\nUSB flash drive removed.\r\n");

        g_rm_freertos_plus_fat_insertion_events = false;

        /* Clear INSERT bit and set REMOVE bit from ISR */
        xEventGroupClearBitsFromISR(g_usb_event_group, USB_INSERT_BIT);
        xEventGroupSetBitsFromISR(g_usb_event_group, USB_REMOVE_BIT, &xHigherPriorityTaskWoken);
    }

    /* Perform a context switch if a higher-priority task was woken by the event */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
