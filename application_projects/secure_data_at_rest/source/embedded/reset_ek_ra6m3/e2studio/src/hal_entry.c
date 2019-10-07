#include "hal_data.h"
#include "bsp_pin_cfg.h"
#include "r_ioport.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);

/* address of the securityMPU access control register */
uint32_t reg_secureMPUAccessControl = 0x00000438;

#define NUMBER_OF_SECTOR					(1)
#define APP_ERR_TRAP(a)             if(a) {__asm("BKPT #0\n");} /* trap the error location */

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void) {
	fsp_err_t err = FSP_SUCCESS;

	/* Configure and open the Flash API */
	err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
        APP_ERR_TRAP(err);

    /* Remove any prior access window */
 	err = R_FLASH_HP_AccessWindowClear(&g_flash0_ctrl);
        APP_ERR_TRAP(err);

    /* erase SECMPUAC at 0x00000438 */
	err = R_FLASH_HP_Erase(&g_flash0_ctrl, reg_secureMPUAccessControl,	NUMBER_OF_SECTOR);
        APP_ERR_TRAP(err);

    /* close flash API */
	err = R_FLASH_HP_Close(&g_flash0_ctrl);
        APP_ERR_TRAP(err);

        while(1);
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
	if (BSP_WARM_START_POST_C == event) {
		/* C runtime environment and system clocks are setup. */

		/* Configure pins. */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}
