/***********************************************************************************************************************
* File Name    : common_app.h
* Description  : Contains macros, data structures, and functions commonly used in the EP
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_APP_H_
#define COMMON_APP_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macros for the EP information. */
#define EP_VERSION              ("1.0")
#define MODULE_NAME             "USBX_ethernet_bridge"

#define BANNER_INFO	            "\r\n******************************************************************"\
								"\r\n*   Renesas FSP Example Project for "MODULE_NAME"         *"\
								"\r\n*   Example Project Version %s                                  *"\
								"\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
								"\r\n******************************************************************"\
								"\r\nRefer to readme.txt file for more details on Example Project and" \
        						"\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                 "\r\nThis example project demonstrates the usage of the Azure NetX Duo"\
                                "\r\nnetwork stack over the USBX RNDIS module on an RA MCU. It performs"\
                                "\r\nvarious network operations leveraging NetX Duo. The RA board creates"\
                                "\r\na DHCP server and assigns the dynamic IP address for the host PC. The"\
                                "\r\nRA board and the host PC are directly linked via a point-to-point"\
                                "\r\nEthernet connection over a USB cable. After obtaining the IP address,"\
                                "\r\nthe project enables the user to interact with a menu through the"\
                                "\r\nJ-Link RTT Viewer or a terminal application on the host PC.\r\n"



#define MENU                    "\r\nSelect the network operation:"\
                                "\r\n   Type '1 <x.x.x.x>:<port>' to perform TCP client operation."\
                                "\r\n   Type '2 <x.x.x.x>:<port>' to perform UDP client operation."\
                                "\r\n   Type '3' to start the web HTTP server."\
                                "\r\n   User input: \r\n"

#if (BSP_CFG_RTOS == 1U)
#define LED_CONTROL_THREAD_SLEEP_TICK   (1U)
#define TERMINAL_THREAD_SLEEP_TICK      (1U)
/* Macros for printing info, handling errors, and trapping errors. */
#define PRINT_EP_BANNER                 (term_send_output_queue(TERMINAL_OUTPUT_EP_BANNER, NULL, RESET_VALUE))
#define PRINT_EP_INFO                   (term_send_output_queue(TERMINAL_OUTPUT_EP_INFO, NULL, RESET_VALUE))
#define PRINT_EP_MENU                   (term_send_output_queue(TERMINAL_OUTPUT_EP_MENU, NULL, RESET_VALUE))
#define PRINT_INFO_STR(str)             (term_send_output_queue(TERMINAL_OUTPUT_INFO_STR, (str), strlen(str)))
#define PRINT_ERR_STR(str)              (term_send_output_queue(TERMINAL_OUTPUT_ERR_STR, (str), strlen(str)))
#define PRINT_ID_STR(id, str)           (term_send_output_queue((id), (str), strlen(str)))
#define PRINT_ID_DATA(id, data)         (term_send_output_queue((id), &(data), sizeof(data)))
#define TERM_INFO_RET(cond, str)             ({\
    if (cond)\
    {\
        term_send_output_queue(TERMINAL_OUTPUT_INFO_STR, (str), strlen(str));\
        return (0U);\
    }\
})
#define TERM_ERROR_RET(cond, err, str)       ({\
    if (cond)\
    {\
        term_send_output_queue(TERMINAL_OUTPUT_ERR_STR, (str), strlen(str));\
        return (err);\
    }\
})
#define TERM_ERROR_TRAP(cond, err, str)      ({\
    if (cond)\
    {\
        term_send_output_queue(TERMINAL_OUTPUT_ERR_STR, (str), strlen(str));\
        term_send_output_queue(TERMINAL_OUTPUT_ERR_TRAP, &(err), sizeof(uint32_t));\
    }\
})

#define TERM_ERR_RET(con, err)               \
({\
    if (con)\
    {\
        return (err);\
    }\
})
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Enumeration for terminal event IDs */
typedef enum terminal_event_id
{
    /* Enumeration IDs for general use */
    TERMINAL_INPUT_MESSAGE,
    TERMINAL_OUTPUT_EP_BANNER,
    TERMINAL_OUTPUT_EP_INFO,
    TERMINAL_OUTPUT_EP_MENU,
    TERMINAL_OUTPUT_INFO_STR,
    TERMINAL_OUTPUT_ERR_STR,
    TERMINAL_OUTPUT_ERR_TRAP,
    /* Enumeration IDs for this example project */
    TERMINAL_OUTPUT_NET_INFO,
    TERMINAL_OUTPUT_HTTP_ADDR_INFO,
    TERMINAL_OUTPUT_TCP_SEND_DATA,
    TERMINAL_OUTPUT_TCP_RECV_DATA,
    TERMINAL_OUTPUT_UDP_SEND_DATA,
    TERMINAL_OUTPUT_UDP_RECV_DATA,
    TERMINAL_OUTPUT_DHCP_CLIENT_ADDR,
    TERMINAL_OUTPUT_DHCP_CLIENT_NAME,
}terminal_event_id_t;

/* Enumeration for LED control events */
typedef enum e_led_event
{
    LED_1_TOGGLE_EVENT          = 1U << 0,
    LED_2_TOGGLE_EVENT          = 1U << 1,
    LED_ANY_EVENT               = 0x03U,
} led_event_t;

/* Enumeration for usb status events */
typedef enum e_usb_status_event
{
    USB_REMOVE_FLAG             = 1U << 0,
    USB_ATTACH_FLAG             = 1U << 1,
    USB_CONNECTION_ANY_EVENT    = 0x03U,
} usb_status_event_t;

/* Structure for IPv4 address */
typedef union u_ipv4_addr
{
    ULONG full;
    struct st_block_ip
    {
        UCHAR d;
        UCHAR c;
        UCHAR b;
        UCHAR a;
    }block;
}ipv4_addr_t;

/* Structure for MAC address */
typedef union u_mac_addr
{
    ULONG64 full;
    struct st_group_mac_addr
    {
        ULONG msb;
        ULONG lsb;
    }group;
    struct st_block_mac_addr
    {
        UCHAR b;
        UCHAR a;
        UCHAR rev1;
        UCHAR rev2;
        UCHAR f;
        UCHAR e;
        UCHAR d;
        UCHAR c;
    }block;
}mac_addr_t;

/* Structure for the network system configuration */
typedef struct st_net_info
{
    mac_addr_t phy_addr;
    ipv4_addr_t ipv4_addr;
    ipv4_addr_t subnet_mask;
}net_info_t;

#endif /* COMMON_APP_H_ */
