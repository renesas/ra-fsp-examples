/***********************************************************************************************************************
 * File Name    : pdc_ep.h
 * Description  : Contains declarations of data structures used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PDC_EP_H_
#define PDC_EP_H_

#define EP_INFO  "\r\nThe Example Project demonstrates the basic functionality of PDC driver on Renesas \r\n"\
                  "RA MCUs based on FSP. On successful initialization of PDC and camera setup,\r\n"\
                  "user is requested to enter any key through RTTViewer to start capturing the image.\r\n"\
                  "The captured image can be viewed in e2studio using the image viewer.\r\n\n"\

typedef enum e_module_name
{
    PDC_MODULE,
    IIC_MODULE,
    ALL
}module_name_t;

#endif /* PDC_EP_H_ */
