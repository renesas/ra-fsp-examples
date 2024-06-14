/***********************************************************************************************************************
 * File Name    : usbx_paud_ep.h
 * Description  : Contains macros, data structures and functions used usbx_paud_ep.h
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef USBX_PAUD_EP_H_
#define USBX_PAUD_EP_H_

/*******************************************************************************************************************//**
 * @addtogroup usbx_paud_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* To display EP info on the RTT viewer */
#define EP_INFO        "\r\nThis example project demonstrates the basic functionalities of USBX "\
            "\r\nPeripheral Audio device driver on Renesas RA MCUs based on Renesas FSP using Azure RTOS. "\
            "\r\nRA MCU will be connected as USBX peripheral audio to the host PC. An audio wav file will "\
            "\r\nbe played on the host PC using audio player(Audacity). The wav audio data will be "\
            "\r\ntransferred from host PC to the MCU using the isochronous OUT, through USBX PAUD module. Once"\
            "\r\nthe audio data is received on the MCU, the received audio data will be stored on the MCU SRAM. "\
            "\r\nWhen host PC starts recording the audio using the audio recorder(Audacity), MCU will transmit the "\
            "\r\naudio data stored in the MCU SRAM to the host PC using the isochronous IN, through USBX PAUD module. "\
            "\r\nThe user will be able to play, listen and verify the recorded audio file data.\r\n"\

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* USBx device configuration settings */
#define DEVICE_FRAME_LENGTH_HIGH_SPEED              (103U)
#define DEVICE_FRAME_LENGTH_FULL_SPEED              (93U)
#define STRING_FRAMEWORK_LENGTH                     (93U)
#define LANGUAGE_ID_FRAME_WORK_LENGTH               (2U)
#define CONFIG_NUMB                                 (1U)
#define INTERFACE_NUMB0                             (0x00)
#define INTERFACE_NUMB1                             (0x01)
#define BYTE_SIZE                                   (4U)
#define DATA_LEN                                    (2048U)
#define WRITE_DATA_LEN                              (62U)
#define MAX_PACKET_SIZE_HS                          (512U)
#define MAX_PACKET_SIZE_FS                          (64U)

/******************************************************************************
    Macro definitions
 ******************************************************************************/
#define INDEX_0                     (0)
#define INDEX_1                     (1)
#define VALUE_0                     (0)
#define VALUE_1                     (1)
#define VALUE_2                     (2)
#define VALUE_4                     (4)
#define VALUE_93                    (93)
#define VALUE_208                   (208)
#define VALUE_226                   (226)
#define VALUE_247                   (247)
#define VALUE_255                   (0xFF)
#define VALUE_275                   (275)
#define GBUFF_SIZE                  (2000 * 192)

#define MEMPOOL_SIZE                (18432 * 2)
#define STACK_SIZE                  (1024U)
#define NUM_OF_FRAME                (8U)

#define USB_APL_ON                  (1U)
#define USB_APL_OFF                 (0U)
#define USB_MAX_PACKET_SIZE_IN      (200U)
#define USB_MAX_PACKET_SZIE_OUT     (192U)
#define USB_APL_DETACH              (0)
#define USB_APL_DEFAULT             (1)
#define USB_APL_CONFIGURED          (2)
#define USB_APL_SUSPEND             (3)
#define USB_APL_RESUME              (4)
#define USB_PLUG_IN                 (1UL << 0)
#define USB_PLUG_OUT                (1UL << 1)
#define USB_PLUG_IN_CLEAR           (~(1UL << 0))
#define USB_PLUG_OUT_CLEAR          (~(1UL << 1))

#define A20_CS_ID                       (0x10)
#define A20_SAMPLING_FREQUENCY          (48000)       /* hz */
#define A20_FU_ID_0                     (5)
#define A20_FU_ID_1                     (8)
#define A20_MUTE                        (0)
#define A20_VOL_MIN_0                   (0)
#define A20_VOL_MAX_0                   (100)
#define A20_SET_VOL_0                   (50)
#define A20_VOL_MIN_1                   (0)
#define A20_VOL_MAX_1                   (200)
#define A20_SET_VOL_1                   (100)

#define A10_FU_ID_0                     (5)
#define A10_FU_ID_1                     (8)
#define A10_EP_ADDR_OUT                 (0x02)
#define A10_EP_ADDR_IN                  (0x81)
#define A10_MUTE_0                      (0)
#define A10_MUTE_1                      (0x10)
#define A10_VOL_0                       (0)
#define A10_VOL_MIN_0                   (0)
#define A10_VOL_MAX_0                   (0x80)
#define A10_VOL_RES_0                   (0x40)
#define A10_VOL_1                       (0)
#define A10_VOL_MIN_1                   (0)
#define A10_VOL_MAX_1                   (0xF0)
#define A10_VOL_RES_1                   (0x80)

/* APL_AUDIO_20
 * macro when enabled uses the HIGH SPEED Interface
 * on EK-RA6M3 and EK-RA6M5
 *
 * macro when disabled uses the FULL SPEED Interface
 * on EK-RA4M2, EK-RA4M3, EK-RA6M1, EK-RA6M2, EK-RA6M3, EK-RA6M4, EK-RA6M5 */
#define APL_AUDIO_20

#endif /* USBX_PAUD_EP_H_ */
/*******************************************************************************************************************//**
 * @} (end defgroup usbx_paud_ep)
 **********************************************************************************************************************/
