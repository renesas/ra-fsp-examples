/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <can.h>
#include <common_utils.h>
#include <menu.h>
#include <slot_info.h>

void hal_entry (void)
{
#if BSP_TZ_SECURE_BUILD

    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif

    fsp_err_t err = FSP_SUCCESS;
    /* Wake up 2nd core if this is first core and we are inside a multicore project. */
#if (0 == _RA_CORE) && (1 == BSP_MULTICORE_PROJECT)
    R_BSP_SecondaryCoreStart();
#endif


    /* Initializes the module. */
    /* Open the flash driver */
    err = R_FLASH_LP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    assert(FSP_SUCCESS == err);

    /*Open can driver*/
    err = can_open();
    assert(FSP_SUCCESS == err);

    err = R_GPT_Open( &g_can_timer_ctrl, &g_can_timer_cfg);
    assert(FSP_SUCCESS == err);
    menu();
}

