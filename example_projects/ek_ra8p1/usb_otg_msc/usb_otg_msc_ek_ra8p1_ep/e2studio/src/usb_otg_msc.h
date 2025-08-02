/***********************************************************************************************************************
* File Name    : usb_otg_msc.h
* Description  : Contains macros and definitions for USB OTG MSC functionality and event handling
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_OTG_MSC_H_
#define USB_OTG_MSC_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8P1_EK)
#define USB_OTG_ID_PIN                  (BSP_IO_PORT_04_PIN_11)
#endif

#if defined(BOARD_RA8P1_EK)
    #if USB_IP0
#define USB_VBUSEN                      (BSP_IO_PORT_05_PIN_00) /* USB_FS_VBUSEN */
    #else
#define USB_VBUSEN                      (BSP_IO_PORT_13_PIN_07) /* USB_HS_VBUSEN */
    #endif
#endif

/***********************************************************************************************************************
 * Typedefs
 **********************************************************************************************************************/
typedef enum e_usb_otg_event
{
    USB_OTG_CHANGE_ID_STATE                 = 1U << 0,
    USB_OTG_CHANGE_TO_HOST                  = 1U << 1,
    USB_OTG_CHANGE_TO_DEVICE                = 1U << 2,
    USB_OTG_HOST_INSERTED                   = 1U << 3,
    USB_OTG_HOST_REMOVED                    = 1U << 4,
    USB_OTG_DEVICE_ATTACH                   = 1U << 5,
    USB_OTG_DEVICE_DETACH                   = 1U << 6,
} usb_otg_event_t;

typedef enum e_usb_otg_mode
{
    OTG_HOST                  ,
    OTG_DEVICE                ,
} usb_otg_mode_t;

typedef struct st_usb_otg_msc {
    usb_instance_ctrl_t * ctrl;
    const usb_cfg_t * cfg;
    usb_otg_mode_t mode;
} usb_otg_msc_t;

#endif /* USB_OTG_MSC_H_ */
