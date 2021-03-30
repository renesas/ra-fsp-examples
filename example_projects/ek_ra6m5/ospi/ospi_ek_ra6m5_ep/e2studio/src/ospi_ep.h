/***********************************************************************************************************************
 * File Name    : ospi_ep.h
 * Description  : Contains data structures and functions used in ospi_ep.h.
 **********************************************************************************************************************/
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

#ifndef OSPI_EP_H_
#define OSPI_EP_H_

#define SOPI_OPERATION_MODE                             (3U)
#define DOPI_OPERATION_MODE                             (4U)
#define SPI_MODE                                        (1U)
#define SOPI_MODE                                       (2U)
#define DOPI_MODE                                       (3U)
#define WRITE                                           (1U)
#define READ                                            (2U)
#define ERASE                                           (3U)
#define EXIT                                            (4U)
#define OSPI_DATA_SIZE                                  (24U)
#define TWO                                             (2U)
#define INITIAL_INDEX                                   (0U)
#define LENGTH_ZERO                                     (0U)
#define LENGTH_ONE                                      (1U)
#define LENGTH_TWO                                      (2U)
#define LENGTH_THREE                                    (3U)
#define LENGTH_FOUR                                     (4U)
#define DELAY_TIME                                      (100U)
#define ERASE_INITIAL_VAL                               (0xffU)
#define DEVICE_ID_MASK                                  (0xFFFFFF)
#define INITIAL_VALUE                                   '\0'

#define OSPI_CR2_DATA_HEX_02                            (0x2U)
#define OSPI_CR2_DATA_HEX_00                            (0x0U)
#define OSPI_CR2_DATA_HEX_05                            (0x5U)
#define OSPI_CR2_DATA_HEX_07                            (0x7U)
#define OSPI_CR2_ADDRESS_HEX_300                        (0x300U)
#define OSPI_CR2_ADDRESS_HEX_200                        (0x200U)
#define OSPI_CR2_ADDRESS_HEX_0                          (0x0U)

#if defined(BOARD_RA6M5_EK)
#define RESET_PIN                                       BSP_IO_PORT_06_PIN_15
#define SPI_REFERENCE_ADDRESS                           (0x70001000)
#define SOPI_REFERNCE_ADDRESS                           (0x70002000)
#define DOPI_REFERNECE_ADDRESS                          (0x70003000)
#else
#define RESET_PIN                                       BSP_IO_PORT_00_PIN_04
#define SPI_REFERENCE_ADDRESS                           (0x68001000)
#define SOPI_REFERNCE_ADDRESS                           (0x68002000)
#define DOPI_REFERNECE_ADDRESS                          (0x68003000)
#endif

#define OSPI_MENU          "\r\nOSPI Main Menu options:"\
                                "\r\n1. Enter 1 for SPI  Mode."\
                                "\r\n2. Enter 2 for SOPI Mode."\
                                "\r\n3. Enter 3 for DOPI Mode."\
                                "\r\nUser Input:  "

#define OSPI_SUB_MENU          "\r\nOSPI Sub Menu options:"\
                               "\r\n1. Write Operation "\
                               "\r\n2. Read Operation "\
                               "\r\n3. Erase Operation "\
                               "\r\n4. Exit Operation "\

#define EP_INFO        "\r\nThis example project demonstrates basic functionalities of OSPI driver\r\n"\
                       "on Renesas RA MCUs based on Renesas FSP. Based on the User input, EP performs\r\n"\
                       "read/write/erase operation in the following modes:\r\n"\
                       "1. SPI Mode\r\n"\
                       "2. SOPI Mode\r\n"\
                       "3. DOPI Mode\r\n"\
                       "On successful completion of each operation, success message will be printed\r\n"\
                       "on RTT viewer. Error and info messages will be printed on JlinkRTTViewer.\r\n\n"


/* function declarations*/
fsp_err_t ospi_init(void);
fsp_err_t ospi_write_enable_and_verify (ospi_instance_ctrl_t * p_instance_ctrl, uint8_t mode);
fsp_err_t spi_operation(void);
fsp_err_t opi_operation(uint8_t mode);
fsp_err_t read_device_id (uint32_t *dev_id);
fsp_err_t reconfigure_device(void);
void handle_error(fsp_err_t err, char * err_str);
uint8_t process_input_data(void);

#endif /* OSPI_EP_H_ */
