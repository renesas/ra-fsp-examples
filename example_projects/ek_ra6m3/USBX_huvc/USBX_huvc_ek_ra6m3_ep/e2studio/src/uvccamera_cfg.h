/***********************************************************************************************************************
 * File Name    : uvccamera_cfg.h
 * Description  : Contains data structures and functions used in usbcamera_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef UVCCAMERA_CFG_H_
#define UVCCAMERA_CFG_H_

#include "ux_api.h"
#include "ux_system.h"
#include "ux_host_class_video.h"

#define UX_ZERO                             (0U)
#define BYTE_SIZE                           (4U)
/* Length of the header of each video data packet we send. */
#define JPEG_BUFFER_NUMBER                      (2)
#define JPEG_BUFFER_SIZE                        (65536/2)
#define VIDEO_BUFFER_SIZE                       (2*1024)
#define VIDEO_RESOLUTION_X                      (320)
#define VIDEO_RESOLUTION_Y                      (240)
#define VIDEO_FRAME_INTERVAL_30FPS              (333333) /* 10000000/30 */

#define UVC_VIDEO_PAYLOAD_HEADER_SIZE           (12)

#define EVENTFLAG_USB_DEVICE_INSERTED          (1U << 0)

#define UVC_STREAM_HEADER_HEADERINFO_EOH       (1U << 7)
#define UVC_STREAM_HEADER_HEADERINFO_ERR       (1U << 6)
#define UVC_STREAM_HEADER_HEADERINFO_STI       (1U << 5)
#define UVC_STREAM_HEADER_HEADERINFO_RES       (1U << 4)
#define UVC_STREAM_HEADER_HEADERINFO_SCR       (1U << 3)
#define UVC_STREAM_HEADER_HEADERINFO_PTS       (1U << 2)
#define UVC_STREAM_HEADER_HEADERINFO_EOF       (1U << 1)
#define UVC_STREAM_HEADER_HEADERINFO_FID       (1U << 0)

#define UVC_JPEG_DATA_FLAG_SOF                 (1U << 2)
#define UVC_JPEG_DATA_FLAG_MID                 (1U << 1)
#define UVC_JPEG_DATA_FLAG_EOF                 (1U << 0)
#define SOF_MARKER                             (0xC0FF)
#define SOS_MARKER                             (0xDAFF)
#define DHT_MARKER                             (0xC4FF)
#define DHT_TABLE_SIZE                         (420)

#define JPEG_BUFFER_ALIGN           (8u)
#define JPEG_IMG_WIDTH              (320)
#define JPEG_IMG_HEIGHT             (240)
#define JPEG_IMG_HSTRIDE            (320)
#define JPEG_IMG_BYTESPP            (2)
#define MEMPOOL_SIZE                (1024U*60)
#define array_count(array) (sizeof(array)/sizeof(array[0]))


#ifdef SEMI_HOSTING
#ifdef __GNUC__
#define DBG_MSG(x, ...)                  printf("%s", (char *)x);
#endif
#else
#define DBG_MSG(x, ...)
#endif

typedef struct uvc_camera_cfg
{
    UX_HOST_CLASS_VIDEO  * volatile video_host_class;
    UCHAR                *video_data_ptrs[JPEG_BUFFER_NUMBER];
    UINT                 completed_transfer_length[JPEG_BUFFER_NUMBER];
    ULONG                device_inserted_eventflag;
    UINT                 callback_index;
    void                 (*uvc_transfer_request_done_callback)(UX_TRANSFER *transfer_request);
} uvc_camera_cfg_t;
#if 1
typedef struct uvc_stream_header_struct
{
    uint8_t bHeaderLength;
    uint8_t bmHeaderInfo;
    uint8_t dwPresentationTime[4];
    uint8_t scrSourceClock_SourceTimeClock[4];
    uint8_t scrSourceClock_SOFCounter[2];
} uvc_stream_header_t;

typedef struct jpeg_stream_struct
{
    void    * p_pointer;
    uint32_t  size;
    uint32_t  flag;
} jpeg_stream_t;
#endif
typedef struct fb_data_struct
{
    UCHAR *p_pointer;
    uint32_t size;
} fb_data_t;

#endif /* UVCCAMERA_CFG_H_ */
