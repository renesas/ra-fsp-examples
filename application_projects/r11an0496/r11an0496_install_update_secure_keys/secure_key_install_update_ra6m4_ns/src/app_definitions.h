/***********************************************************************************************************************
 * File Name    : app_definitions.h
 * Description  : Contains macros specific to this application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2020-2021] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/
#ifndef APP_DEFINITIONS_H_
#define APP_DEFINITIONS_H_

#include <stdint.h>
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"
#include "common_utils.h"

#define _unused(x) ((void)(x))


/* Macros for menu options to be displayed */

#define MENUOPTION_MENU             "\r\n MENU to Select \r\n"
#define MENUOPTION1                 "Press 1 exercise the cryptographic operation with directly installed user key\r\n"
#define MENUOPTION2                 "Press 2 create updated new user key using FSP\r\n"
#define MENUOPTION3                 "Press 3 exercise the cryptographic operation with newly updated user key\r\n"

#define DECRYPT_WITH_INSTALLED_KEY          (1)
#define UPDATE_USER_KEY                     (2)
#define DECRYPT_WITH_UPDATED_KEY            (3)


#define MENUEVENT1       " Cryptographic operation with directly installed user key, which is installed via the serial interface.\r\n"
#define MENUEVENT2       " Update the new user key encrypted with key update key and store the new wrapped key in data flash.\r\n"
#define MENUEVENT3       " Cryptographic operation with new wrapped user key stored in data flash\r\n\r\n"

#define RESULT1_SUCCESS " \r\nResult: Cryptographic operation is successful with directly installed user key\r\n\r\n"
#define RESULT2_SUCCESS " \r\nResult: User Key is updated and stored to Data Flash\r\n\r\n"
#define RESULT3_SUCCESS " \r\nResult: Cryptographic operation is successful with Updated User Key\r\n\r\n"

#define RESULT1_FAILURE " \r\nResult: Cryptographic operation failed with directly installed user key\r\n\r\n"
#define RESULT2_FAILURE " \r\nResult: User Key Update Failed\r\n\r\n"
#define RESULT3_FAILURE " \r\nResult: Cryptographic operation failed with Updated User Key\r\n\r\n"
#define RESULT4_FAILURE " \r\n Unsupported Command\r\n"

#endif /* APP_DEFINITIONS_H_ */
