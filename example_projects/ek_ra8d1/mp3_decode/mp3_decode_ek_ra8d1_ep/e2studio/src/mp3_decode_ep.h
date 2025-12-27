/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef MP3_DECODE_EP_H_
#define MP3_DECODE_EP_H_

#include "stdio.h"

/***********************************************************************************************************************
 * Function Prototypes
 **********************************************************************************************************************/
void        handle_fsp_error(fsp_err_t err, char * err_str);
fsp_err_t   usb_safely_eject(void);
fsp_err_t   minimp3_decode_operation(void);
void        wait_for_key_and_reinit(void);
void        init_freertos_plus_fat(void);
fsp_err_t   usb_init(void);
void        handle_user_input(void);

/***********************************************************************************************************************
 * Macro Definitions
 **********************************************************************************************************************/
#define USB_INSERT_BIT          (1U << 0)
#define USB_REMOVE_BIT          (1U << 1)
#define SUCCESS                 (0U)
#define DEFAULT_VOLUME          (0.5f)
#define MAX_FILENAME_LEN        (64)
#define ROOT_PATH               ("/")
#define MAX_MP3_FILES           (64U)
#define MAX_MP3_PATH_LEN        (128U)


#define EP_VERSION              ("1.0")
#define MODULE_NAME             "minimp3"
#define BANNER_INFO             "\r\n********************************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                             *"\
                                "\r\n*   Example Project Version %s                                                *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                                "\r\n********************************************************************************"\
                                "\r\nRefer to the accompanying .md file for Example Project details." \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO        "\r\nThis Example Project demonstrates real-time MP3 audio playback on a Renesas RA MCU\r\n"\
                       "using the Mini MP3 Decoder. The application reads an MP3 file from a USB flash drive\r\n"\
                       "and decodes it on the fly using the lightweight minimp3 library, streaming the\r\n"\
                       "resulting PCM audio samples to the DAC peripheral for continuous playback.\r\n"\

typedef struct st_mp3_file_entry
{
    char path[MAX_MP3_PATH_LEN];
} mp3_file_entry_t;

#define MINI_MP3_MENU   "\r\n================= USB MP3 Player Menu ==================\r\n"\
		                " 1. Play MP3 Audio File\r\n"\
                        " 2. Format USB Drive\r\n"\
                        " 3. Safely Eject USB Drive\r\n"\
                        "========================================================\r\n"

#define MP3_PLAY                ('1')   /* MP3 playback */
#define USB_FORMAT              ('2')   /* Format operation */
#define USB_SAFELY_EJECT        ('3')   /* Safely eject operation */
#define USB_INIT                ('4')   /* USB reinitialization */

#endif /* MP3_DECODE_EP_H_ */
