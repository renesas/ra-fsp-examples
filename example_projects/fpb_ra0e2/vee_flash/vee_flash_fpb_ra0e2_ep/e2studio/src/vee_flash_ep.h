/***********************************************************************************************************************
 * File Name    : vee_flash_ep.h
 * Description  : Contains data structures and functions used in vee_flash_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef VEE_FLASH_EP_H_
#define VEE_FLASH_EP_H_

#define REC_ID_1              (1U)          /* ID of Record 1 to write/read. */
#define REC_ID_2              (2U)          /* ID of Record 2 to write/read. */
#define REC_ID_3              (3U)          /* ID of Record 3 to write/read. */
#define REC_ID_4              (4U)          /* ID of Record 4 to write/read. */
#define DATA_SIZE             (12U)         /* Length of data to write. */
#define WRITE                 (1U)          /* RTT Input selection for write operation. */
#define READ                  (2U)          /* RTT Input selection for read operation. */
#define FORMAT                (3U)          /* RTT Input selection for format operation. */
#define STATUS                (4U)          /* RTT Input selection for statusGet. */
#define REFRESH               (5U)          /* RTT Input selection for Refresh. */
#define WRITE_READ_ID_1       (1U)          /* RTT Input selection for write/read integer data to/from RECORD_ID1. */
#define WRITE_READ_ID_2       (2U)          /* RTT Input selection for write/read char data to/from RECORD_ID2. */
#define WRITE_READ_ID_3       (3U)          /* RTT Input selection for write/read float data to/from RECORD_ID3. */
#define WRITE_READ_ID_4       (4U)          /* RTT Input selection for write/read structure data to/from RECORD_ID4. */
#define DEFAULT_ID            (65535U)
#define ONE                   (1U)
#define BUF_SIZE              (64U)
#define INITIAL_VALUE         ('\0')

/* vee flash Menu options */
#define VEE_FLASH_MENU          "\r\nVee flash Main Menu options:"\
                                "\r\n1. Enter 1 for Write operation."\
                                "\r\n2. Enter 2 for Read operation."\
                                "\r\n3. Enter 3 for Format operation (Format should only be"\
                                "\r\n   used to erase the entire data flash used by VEEPROM)."\
                                "\r\n4. Enter 4 for StatusGet operation."\
                                "\r\n5. Enter 5 for Refresh operation."\
                                "\r\nUser Input:  "

/* vee flash Sub-Menu options for write operation*/
#define VEE_FLASH_WRITE_MENU    "\r\nVee flash Menu options for Write operation"\
                                "\r\n1. Enter 1 to write fixed integer data to RECORD_ID1."\
                                "\r\n2. Enter 2 to write fixed char data to RECORD_ID2."\
                                "\r\n3. Enter 3 to write fixed float data to RECORD_ID3."\
                                "\r\n4. Enter 4 to write fixed structure data to RECORD_ID4."

/* vee flash Sub-Menu options for Read operation*/
#define VEE_FLASH_READ_MENU     "\r\nVee flash Menu options for Read operation"\
                                "\r\n1. Enter 1 to read most recent data associated with RECORD_ID1."\
                                "\r\n2. Enter 2 to read most recent data associated with RECORD_ID2."\
                                "\r\n3. Enter 3 to read most recent data associated with RECORD_ID3."\
                                "\r\n4. Enter 4 to read most recent data associated with RECORD_ID4."

#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of vee flash driver\r\n"\
                       "on Renesas RA MCUs based on Renesas FSP. Based on User input, EP performs:\r\n"\
                       "1. Write and Read operations on different Record_ID's for different data types.\r\n"\
                       "2. Format, GetStatus, Refresh operations can also be performed on vee flash.\r\n"\
                       "On successful completion of each operation, success message will be printed\r\n"\
                       "on J-Link RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.\r\n\n\n"

/* Structure variables.*/
typedef struct st_adxlsensor
{
    uint8_t x_data;
    uint8_t y_data;
    uint8_t z_data;
    uint8_t xy_data;
} adxl_t;

/* Function declarations*/
fsp_err_t vee_flash_init(void);
fsp_err_t vee_write_operation(uint32_t const rec_id, void * p_data, uint32_t bytes);
fsp_err_t vee_read_operation(uint32_t const rec_id);
fsp_err_t vee_format_operation(void);
fsp_err_t vee_status_get_operation(void);
fsp_err_t vee_refresh_operation(void);
void handle_error(fsp_err_t err, char * err_str);
uint8_t process_input_data(void);

#endif /* VEE_FLASH_EP_H_ */
