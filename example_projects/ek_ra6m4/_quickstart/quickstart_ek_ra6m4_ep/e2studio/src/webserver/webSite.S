/***********************************************************************************************************************
* Copyright (c) 2014 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*******************************************************************************
* File Name     : webData.S
* Version       : 0.00
* Device(s)     : Renesas
* Tool-Chain    : GNU
* H/W Platform  : RSK2+RZA1H
* Description   : Inclusion of the web site data files from the WebSite folder
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 18.06.2013 0.01
*******************************************************************************/

	/* This file is updated by the build counter and is included to force this file
	   to be built every time */
//	.include "version.h"
    .global gFsWebSite
    .align 4
gFsWebSite:
	.incbin "fsWebSite.bin"
    .end
