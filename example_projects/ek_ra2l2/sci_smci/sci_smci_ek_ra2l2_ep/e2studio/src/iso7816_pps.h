/***********************************************************************************************************************
File Name    : iso7816_pps.h
Description  : As required for the file such as Contains macros, data structures and functions used common to the EP
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ISO7816_PPS_H_
#define ISO7816_PPS_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define PPSS                                    (0)     /* PPS initial character */
#define PPS0                                    (1)     /* PPS format character */
#define PPS1                                    (2)     /* PPS parameter character 1 */
#define PPS2                                    (3)     /* PPS parameter character 2 */
#define PPS3                                    (4)     /* PPS parameter character 3 */
#define PCK                                     (5)     /* PPS check */
#define PPS_OK                                  (0)     /* Negotiation OK */
#define PPS_HANDSAKE_ERROR                      (-1)    /* Agreement not reached */
#define PPS_COMMUNICATION_ERROR                 (-2)    /* Communication error */
#define PPS_MAX_LENGTH                          (6)     /* Max length of the PPS frame */
#define PPS_HAS_PPS1(pps_buffer)                (((pps_buffer)[1] & 0x10) == 0x10)
#define PPS_HAS_PPS2(pps_buffer)                (((pps_buffer)[1] & 0x20) == 0x20)
#define PPS_HAS_PPS3(pps_buffer)                (((pps_buffer)[1] & 0x40) == 0x40)

/***********************************************************************************************************************
 * Function Prototypes
 **********************************************************************************************************************/
fsp_err_t PPS_Exchange(uint8_t * request, uint8_t * resp_length, uint8_t* pps1);
bool PPS_receive_character(uint8_t received_data);
uint8_t PPS_Match(uint8_t * request, uint8_t len_request, uint8_t * response, uint8_t len_response);
uint8_t PPS_GetLength(uint8_t* pps_buffer);
uint8_t PPS_GetPCK(uint8_t* pps_buffer, uint8_t length);
#endif /* ISO7816_PPS_H_ */
