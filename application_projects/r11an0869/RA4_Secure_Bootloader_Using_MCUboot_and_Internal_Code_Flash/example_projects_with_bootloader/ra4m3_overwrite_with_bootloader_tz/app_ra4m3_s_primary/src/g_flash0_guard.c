#include "guard.h"

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_open_guard(flash_ctrl_t *const p_api_ctrl, flash_cfg_t const *const p_cfg)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);
    FSP_PARAMETER_NOT_USED (p_cfg);

    return R_FLASH_HP_Open (&g_flash0_ctrl, &g_flash0_cfg);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_write_guard(flash_ctrl_t *const p_api_ctrl, uint32_t const src_address,
        uint32_t flash_address, uint32_t const num_bytes)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_Write (&g_flash0_ctrl, src_address, flash_address, num_bytes);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_erase_guard(flash_ctrl_t *const p_api_ctrl, uint32_t const address,
        uint32_t const num_blocks)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_Erase (&g_flash0_ctrl, address, num_blocks);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_blank_check_guard(flash_ctrl_t *const p_api_ctrl, uint32_t const address,
        uint32_t num_bytes, flash_result_t *p_blank_check_result)
{
    /* Verify all pointers are in non-secure memory. */
    flash_result_t *p_blank_check_result_checked = cmse_check_pointed_object (p_blank_check_result, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_blank_check_result_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_BlankCheck (&g_flash0_ctrl, address, num_bytes, p_blank_check_result_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_close_guard(flash_ctrl_t *const p_api_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_Close (&g_flash0_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_status_get_guard(flash_ctrl_t *const p_api_ctrl,
        flash_status_t *const p_status)
{
    /* Verify all pointers are in non-secure memory. */
    flash_status_t *const p_status_checked = cmse_check_pointed_object (p_status, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_status_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_StatusGet (&g_flash0_ctrl, p_status_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_access_window_set_guard(flash_ctrl_t *const p_api_ctrl,
        uint32_t const start_addr, uint32_t const end_addr)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_AccessWindowSet (&g_flash0_ctrl, start_addr, end_addr);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_access_window_clear_guard(flash_ctrl_t *const p_api_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_AccessWindowClear (&g_flash0_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_id_code_set_guard(flash_ctrl_t *const p_api_ctrl,
        uint8_t const *const p_id_code, flash_id_code_mode_t mode)
{
    /* Verify all pointers are in non-secure memory. */
    uint8_t const *const p_id_code_checked = cmse_check_address_range ((void*) p_id_code, 16, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_id_code_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_IdCodeSet (&g_flash0_ctrl, p_id_code_checked, mode);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_reset_guard(flash_ctrl_t *const p_api_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_Reset (&g_flash0_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_update_flash_clock_freq_guard(flash_ctrl_t *const p_api_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_UpdateFlashClockFreq (&g_flash0_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_start_up_area_select_guard(flash_ctrl_t *const p_api_ctrl,
        flash_startup_area_swap_t swap_type, bool is_temporary)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_StartUpAreaSelect (&g_flash0_ctrl, swap_type, is_temporary);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_callback_set_guard(flash_ctrl_t *const p_api_ctrl,
        void (*p_callback)(flash_callback_args_t*), void const *const p_context,
        flash_callback_args_t *const p_callback_memory)
{
    /* Verify all pointers are in non-secure memory. */
    void (*p_callback_checked)(
            flash_callback_args_t*) = (void(*)(flash_callback_args_t *)) cmse_check_address_range((void *) p_callback, sizeof(void *), CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_checked != NULL);
    flash_callback_args_t *const p_callback_memory_checked = cmse_check_pointed_object(p_callback_memory, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_memory_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_context);

    return R_FLASH_HP_CallbackSet(&g_flash0_ctrl, p_callback_checked, p_context, p_callback_memory_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_bank_swap_guard(flash_ctrl_t *const p_api_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_BankSwap (&g_flash0_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_info_get_guard(flash_ctrl_t *const p_api_ctrl, flash_info_t *const p_info)
{
    /* Verify all pointers are in non-secure memory. */
    flash_info_t *const p_info_checked = cmse_check_pointed_object (p_info, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_info_checked != NULL);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_InfoGet (&g_flash0_ctrl, p_info_checked);
}
