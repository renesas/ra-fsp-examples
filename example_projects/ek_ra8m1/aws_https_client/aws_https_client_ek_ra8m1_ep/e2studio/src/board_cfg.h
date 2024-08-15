/***********************************************************************************************************************
 * File Name    : board_cfg.h
 * Description  : Board specific/configuration data.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include "hal_data.h"

#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define KIT_NAME                "EK-RA6M3"
#elif defined (BOARD_RA8M1_EK)
#define KIT_NAME                "EK-RA8M1"
#else
#define KIT_NAME                "EK-RA6M5"
#endif

/* MACRO definitions */
#define ADCTEMP_AS_C(a)             ((((float)a) * 0.196551f) - 277.439f)
#define ADCTEMP_AS_F(a)             ((((float)a) * 0.353793f) - 467.39f)


#define ETH_PREINIT             "\r\n \r\n--------------------------------------------------------------------------------"\
                                "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Pre IP Init       "\
                                "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#define ETH_POSTINIT             "\r\n \r\n--------------------------------------------------------------------------------"\
                                "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Post IP Init       "\
                                "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#endif /* BOARD_CONFIG_H_ */
