/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_wdt.h"
#include "r_wdt_api.h"
FSP_HEADER
/** Timer on GPT Instance. */
extern const timer_instance_t g_timer;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t g_timer_ctrl;
extern const timer_cfg_t g_timer_cfg;

#ifndef gpt_callback
void gpt_callback(timer_callback_args_t * p_args);
#endif
/** WDT on WDT Instance. */
extern const wdt_instance_t g_wdt;

/** Access the WDT instance using these structures when calling API functions directly (::p_api is not used). */
extern wdt_instance_ctrl_t g_wdt_ctrl;
extern const wdt_cfg_t g_wdt_cfg;

#ifndef NULL
void NULL(wdt_callback_args_t * p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
