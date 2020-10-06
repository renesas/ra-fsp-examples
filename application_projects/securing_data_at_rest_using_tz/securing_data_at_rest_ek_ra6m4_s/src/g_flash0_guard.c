#include "guard.h"

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_open_guard(flash_ctrl_t *const p_api_ctrl, flash_cfg_t const *const p_cfg)
{
    FSP_PARAMETER_NOT_USED (p_api_ctrl);
    FSP_PARAMETER_NOT_USED (p_cfg);

    return R_FLASH_HP_Open (&g_flash0_ctrl, &g_flash0_cfg);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_write_guard(flash_ctrl_t *const p_api_ctrl, uint32_t const src_address,
        uint32_t flash_address, uint32_t const num_bytes)
{
	/* Verify source address is in non-secure memory. */
	void const *const src_address_range_checked = cmse_check_address_range ((unsigned char *) src_address, num_bytes,
			CMSE_AU_NONSECURE);

	FSP_ASSERT ((unsigned char*)src_address == src_address_range_checked);

	/* Verify destination address is in non-secure memory. */
	void const *const dest_address_range_checked = cmse_check_address_range ((unsigned char *) flash_address, num_bytes,
			CMSE_AU_NONSECURE);

	FSP_ASSERT ((unsigned char*)flash_address == dest_address_range_checked);

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_Write (&g_flash0_ctrl, (uint32_t)src_address_range_checked, (uint32_t)dest_address_range_checked, num_bytes);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_erase_guard(flash_ctrl_t *const p_api_ctrl, uint32_t const address,
        uint32_t const num_blocks)
{

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_Erase (&g_flash0_ctrl, address, num_blocks);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_blank_check_guard(flash_ctrl_t *const p_api_ctrl, uint32_t const address,
        uint32_t num_bytes, flash_result_t *blank_check_result)
{
    /* Verify all pointers are in non-secure memory. */
    flash_result_t *blank_check_result_checked = cmse_check_pointed_object (blank_check_result, CMSE_AU_NONSECURE);
    FSP_ASSERT (blank_check_result == blank_check_result_checked);

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_BlankCheck (&g_flash0_ctrl, address, num_bytes, blank_check_result_checked);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_close_guard(flash_ctrl_t *const p_api_ctrl)
{

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return R_FLASH_HP_Close (&g_flash0_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_status_get_guard(flash_ctrl_t *const p_api_ctrl,
        flash_status_t *const p_status)
{
    /* Verify all pointers are in non-secure memory. */
    flash_status_t *const p_status_checked = cmse_check_pointed_object (p_status, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_status == p_status_checked);

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;

}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_access_window_set_guard(flash_ctrl_t *const p_api_ctrl,
        uint32_t const start_addr, uint32_t const end_addr)
{
    FSP_PARAMETER_NOT_USED (p_api_ctrl);
    FSP_PARAMETER_NOT_USED (start_addr);
    FSP_PARAMETER_NOT_USED (end_addr);

    return FSP_ERR_UNSUPPORTED;

}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_access_window_clear_guard(flash_ctrl_t *const p_api_ctrl)
{

    FSP_PARAMETER_NOT_USED (p_api_ctrl);
    return FSP_ERR_UNSUPPORTED;

}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_id_code_set_guard(flash_ctrl_t *const p_api_ctrl,
        uint8_t const *const p_id_code, flash_id_code_mode_t mode)
{
    /* Verify all pointers are in non-secure memory. */
    uint8_t const *const p_id_code_checked = cmse_check_address_range ((void*) p_id_code, 16, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_id_code == p_id_code_checked);

    FSP_PARAMETER_NOT_USED (p_api_ctrl);
    FSP_PARAMETER_NOT_USED (mode);

    return FSP_ERR_UNSUPPORTED;

}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_reset_guard(flash_ctrl_t *const p_api_ctrl)
{

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;

}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_update_flash_clock_freq_guard(flash_ctrl_t *const p_api_ctrl)
{

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;

}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_start_up_area_select_guard(flash_ctrl_t *const p_api_ctrl,
        flash_startup_area_swap_t swap_type, bool is_temporary)
{
    FSP_PARAMETER_NOT_USED (p_api_ctrl);
    FSP_PARAMETER_NOT_USED (is_temporary);
    FSP_PARAMETER_NOT_USED (swap_type);

    return FSP_ERR_UNSUPPORTED;

}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_callback_set_guard(flash_ctrl_t *const p_api_ctrl,
        void (*p_callback)(flash_callback_args_t*), void const *const p_context,
        flash_callback_args_t *const p_callback_memory)
{
    /* Verify all pointers are in non-secure memory. */
    void (*p_callback_checked)(
            flash_callback_args_t*) = (void(*)(flash_callback_args_t *)) cmse_check_address_range((void *) p_callback, sizeof(void *), CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback == p_callback_checked);
    flash_callback_args_t *const p_callback_memory_checked = cmse_check_pointed_object(p_callback_memory, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_memory == p_callback_memory_checked);

    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_context);

    return FSP_ERR_UNSUPPORTED;

}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_version_get_guard(fsp_version_t *const p_version)
{
    /* Verify all pointers are in non-secure memory. */
    fsp_version_t *const p_version_checked = cmse_check_pointed_object (p_version, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_version == p_version_checked);

    return FSP_ERR_UNSUPPORTED;
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_flash0_info_get_guard(flash_ctrl_t *const p_api_ctrl, flash_info_t *const p_info)
{
    /* Verify all pointers are in non-secure memory. */
    flash_info_t *const p_info_checked = cmse_check_pointed_object (p_info, CMSE_AU_NONSECURE);
    FSP_ASSERT (p_info == p_info_checked);

    FSP_PARAMETER_NOT_USED (p_api_ctrl);

    return FSP_ERR_UNSUPPORTED;

}
