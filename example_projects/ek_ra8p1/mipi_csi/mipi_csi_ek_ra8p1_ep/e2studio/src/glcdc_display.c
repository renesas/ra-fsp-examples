/*
 * glcdc_display.c
 *
 *  Created on: Jun 26, 2025
 *      Author: a5145403
 */

#include "glcdc_display.h"
#if (DISPLAY_OUTPUT == 1U)

/* Variables to store resolution information */
volatile uint8_t g_vsync_flag = RESET_FLAG;

/* Variables used for buffer usage */
uint32_t g_buffer_size;

extern uint16_t g_image_width;
extern uint16_t g_image_height;

/***********************************************************************************************************************
 *  Function Name: glcdc_init
 *  Description  : This function is used to initialize GLCDC module with selected resolution
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS    Upon successful operation
 *                 Any Other Error code apart from FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t glcdc_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close opened GLCDC module*/
    if(DISPLAY_STATE_CLOSED != g_display_ctrl.state)
    {
        err = R_GLCDC_Close(&g_display_ctrl);
        APP_ERR_RET( FSP_SUCCESS != err, err, "** R_GLCDC_Close API FAILED **");
    }
    display_cfg_t g_display_user_cfg;
    memcpy(&g_display_user_cfg, &g_display_cfg, sizeof(display_cfg_t));
    g_display_user_cfg.input[0].hsize = g_image_width;
    g_display_user_cfg.input[0].vsize = g_image_height;

    /* Initialize GLCDC driver */
    err = R_GLCDC_Open(&g_display_ctrl, &g_display_user_cfg);
    APP_ERR_RET( FSP_SUCCESS != err, err, "** R_GLCDC_Open API FAILED **");

    /* Start GLCDC display output */
    err = R_GLCDC_Start(&g_display_ctrl);
    APP_ERR_RET( FSP_SUCCESS != err, err, "** R_GLCDC_Start API FAILED **");

    g_vsync_flag = RESET_FLAG;
    /* Wait for a Vsync event */
    while(!g_vsync_flag);

    return err;
}
/***********************************************************************************************************************
* End of function glcdc_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: glcdc_vsync_isr
 *  Description  : This function is used to detect vsync interrupt.
 *  Arguments    : p_args      Pointer to callback argument
 *  Return Value : None
 **********************************************************************************************************************/
void glcdc_vsync_isr(display_callback_args_t *p_args)
{
    if (p_args->event == DISPLAY_EVENT_LINE_DETECTION)
    {
        g_vsync_flag = SET_FLAG;
    }
}
/***********************************************************************************************************************
* End of function glcdc_vsync_isr
***********************************************************************************************************************/
#endif /* DISPLAY_OUTPUT */
