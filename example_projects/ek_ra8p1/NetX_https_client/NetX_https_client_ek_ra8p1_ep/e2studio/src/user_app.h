/***********************************************************************************************************************
 * File Name    : user_app.h
 * Description  : Contains details of the Adafruit IO server.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USER_APP_H_
#define USER_APP_H_

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/

/* Web address of the Server (Adafruit IO) */
#define ADAFRUIT_IO_SERVER  "io.adafruit.com"

/* URL requirement for Adafruit server */
#define API_V2              "/api/v2/"

/* For the specific username, feed id can be distinguished by this */
#define FEEDS               "/feeds/"

/* To get the data from the specific feed */
#define DATA                "/data"

/* Separator for the HTTP address */
#define SEPARATOR           "/"

/* To get only the last data from the specific feed */
#define LAST                "last"

/* The response key where data is saved in the feed */
#define REQ_RESPONSE_KEY    "value"

/* When making HTTP requests to Adafruit IO, you can include the API key
 * as a query parameter named x-aio-key or as a request header named X-AIO-Key.
 * In both cases, "X-AIO-Key" is case insensitive.
 * NOTE: If you regenerate your AIO key, your old key will be immediately invalidated,
 * so you'll have to replace it in any scripts or sketches where it is in use */
#define X_AIO_KEY           "X-AIO-Key"
/* Adafruit IO content type */
#define X_CONTENT_TYPE      "Content-Type"

/* The content on the server is in JSON format */
#define CONTENT_TYPE        "application/json"

#endif /* USER_APP_H_ */
