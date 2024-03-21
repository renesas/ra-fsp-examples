#include "guard.h"


BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_i2c_master_open_guard (i2c_master_ctrl_t *const p_api_ctrl, i2c_master_cfg_t const *const p_cfg)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_cfg);

    return R_IIC_MASTER_Open(&g_i2c_master_ctrl, &g_i2c_master_cfg);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_i2c_master_read_guard (i2c_master_ctrl_t *const p_api_ctrl, uint8_t *const p_dest, uint32_t const bytes, bool const restart)
{
    /* Verify all pointers are in non-secure memory. */
    uint8_t *const p_dest_checked = cmse_check_address_range((void *) p_dest, bytes, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_dest == p_dest_checked);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return R_IIC_MASTER_Read(&g_i2c_master_ctrl, p_dest_checked, bytes, restart);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_i2c_master_write_guard (i2c_master_ctrl_t *const p_api_ctrl, uint8_t *const p_src, uint32_t const bytes, bool const restart)
{
    /* Verify all pointers are in non-secure memory. */
    uint8_t *const p_src_checked = cmse_check_address_range((void *) p_src, bytes, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_src == p_src_checked);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return R_IIC_MASTER_Write(&g_i2c_master_ctrl, p_src_checked, bytes, restart);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_i2c_master_abort_guard (i2c_master_ctrl_t *const p_api_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return R_IIC_MASTER_Abort(&g_i2c_master_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_i2c_master_slave_address_set_guard (i2c_master_ctrl_t *const p_api_ctrl, uint32_t const slave, i2c_master_addr_mode_t const addr_mode)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return R_IIC_MASTER_SlaveAddressSet(&g_i2c_master_ctrl, slave, addr_mode);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_i2c_master_close_guard (i2c_master_ctrl_t *const p_api_ctrl)
{
    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);

    return R_IIC_MASTER_Close(&g_i2c_master_ctrl);
}

BSP_CMSE_NONSECURE_ENTRY fsp_err_t g_i2c_master_callback_set_guard (i2c_master_ctrl_t *const p_api_ctrl, void(*p_callback)(i2c_master_callback_args_t *), void const *const p_context, i2c_master_callback_args_t *const p_callback_memory)
{
    /* Verify all pointers are in non-secure memory. */
    void(*p_callback_checked)(i2c_master_callback_args_t *) = (void(*)(i2c_master_callback_args_t *)) cmse_check_address_range((void *) p_callback, sizeof(void *), CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback == p_callback_checked);
    i2c_master_callback_args_t *const p_callback_memory_checked = cmse_check_pointed_object(p_callback_memory, CMSE_AU_NONSECURE);
    FSP_ASSERT(p_callback_memory == p_callback_memory_checked);

    /* TODO: add your own security checks here */

    FSP_PARAMETER_NOT_USED(p_api_ctrl);
    FSP_PARAMETER_NOT_USED(p_context);

    return R_IIC_MASTER_CallbackSet(&g_i2c_master_ctrl, p_callback_checked, p_context, p_callback_memory_checked);
}
