/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "guard.h"


BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_doc_open_guard (doc_ctrl_t *const p_api_ctrl, doc_cfg_t const *const p_cfg)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);

    return R_DOC_Open(&g_doc_ctrl, &g_doc_cfg);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_doc_close_guard (doc_ctrl_t *const p_api_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return R_DOC_Close(&g_doc_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_doc_read_guard (doc_ctrl_t *const p_api_ctrl, uint32_t *p_result)
{
    /* Verify all pointers are in non-secure memory. */
    uint32_t * p_result_checked = cmse_check_address_range((void *) p_result, sizeof(uint32_t), CMSE_AU_NONSECURE);
    FSP_ASSERT(p_result == p_result_checked);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return R_DOC_Read(&g_doc_ctrl, p_result_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_doc_write_guard (doc_ctrl_t *const p_api_ctrl, uint32_t data)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return R_DOC_Write(&g_doc_ctrl, data);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_doc_callback_set_guard (doc_ctrl_t *const p_api_ctrl, \
        void(*p_callback)(doc_callback_args_t *), void *const p_context, \
        doc_callback_args_t *const p_callback_memory)
{
    /* Verify all pointers are in non-secure memory. */
    void(*p_callback_checked)(doc_callback_args_t *) = (void(*)(doc_callback_args_t *)) \
            cmse_check_address_range((void *) p_callback, sizeof(void *), CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback == p_callback_checked);
    doc_callback_args_t *const p_callback_memory_checked = cmse_check_pointed_object(p_callback_memory, \
                                                                                     CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_memory == p_callback_memory_checked);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_context);

    return R_DOC_CallbackSet(&g_doc_ctrl, p_callback_checked, p_context, p_callback_memory_checked);
}
