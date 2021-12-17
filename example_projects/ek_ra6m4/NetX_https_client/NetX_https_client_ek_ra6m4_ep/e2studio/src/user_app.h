/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains details of the Adafruit IO server.
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

#ifndef USER_APP_H_
#define USER_APP_H_

/******************************************************************************
 Macro definitions.
 ******************************************************************************/
/* Adafruit User configurable settings. */

/**
 * The following macros together help the user to read/ write data to the Adafruit IO Server
 * Each macro is a part of the Server URL from where the user will read/write data
 *
 *  Sample URL : https://ioadafruit.com/{IO_USERNAME}/{feeds}/{FEED_NAME}/data/
 *  User has to be update these fields {IO_USERNAME},{FEED_NAME} and {IO_KEY}
 *  according to their credentials, in the respective macros.
 *  For more information on obtaining user credentials and updating them in this file,
 *  Follow the steps provided in mark down file.
 **/

/* Web address of the Server (Adafruit IO). */
#define ADAFRUIT_IO_SERVER  "io.adafruit.com"

/* Adafruit IO user specific key. *Needs to be changed by the user.*
 * Find it by logging into adafruit.io, under the section 'My Key'
 * Copy the key from 'Active Key', and paste here. */
#define IO_KEY              "aio_YYDz98TfzduyHvSblgkCNMgkxSmZ"

/* Adafruit IO Username. *Needs to be changed by the user.*/
/* Adafruit Server details to create and update, data as per GET, PUT, POST functions.
 * On the Adafruit IO server, usernames can be distinguished by this. */
#define IO_USERNAME         "dnyaneshwarh"
/* URL requirement for Adafruit server. */
#define API_V2              "/api/v2/"
/* For the specific username, feed id can be distinguished by this. */
#define FEEDS               "/feeds/"
/* To get the data from the specific feed. */
#define DATA                "/data"
/* Separator for the HTTP address. */
#define SEPERATOR           "/"
/* The response key where data is saved in the feed. */
#define REQ_RESPONSE_KEY    "value"
/* Name of the user feed created on Adafruit.IO server.
 * *Needs to be changed by the user.*/
#define FEED_NAME           "test"

/* When making HTTP requests to Adafruit IO, you can include the API key
 * as a query parameter named x-aio-key or as a request header named X-AIO-Key.
 * In both cases, "X-AIO-Key" is case insensitive.
 * NOTE: If you regenerate your AIO key, your old key will be immediately invalidated,
 * so you'll have to replace it in any scripts or sketches where it is in use.*/
#define X_AIO_KEY           "X-AIO-Key"
/* Adafruit IO content type. */
#define X_CONTENT_TYPE      "Content-Type"
/* The content on server is like a json app.  */
#define CONTENT_TYPE        "application/json"

#endif /* USER_APP_H_ */
