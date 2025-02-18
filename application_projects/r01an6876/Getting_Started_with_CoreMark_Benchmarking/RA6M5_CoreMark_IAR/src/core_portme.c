/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Original Author: Shay Gal-on
*/
#include "coremark.h"
#include "core_portme.h"
#include "hal_data.h"

#if VALIDATION_RUN
volatile ee_s32 seed1_volatile = 0x3415;
volatile ee_s32 seed2_volatile = 0x3415;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN
volatile ee_s32 seed1_volatile = 0x0;
volatile ee_s32 seed2_volatile = 0x0;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PROFILE_RUN
volatile ee_s32 seed1_volatile = 0x8;
volatile ee_s32 seed2_volatile = 0x8;
volatile ee_s32 seed3_volatile = 0x8;
#endif
volatile ee_s32 seed4_volatile = ITERATIONS;
volatile ee_s32 seed5_volatile = 0;

/* Since we set the timer period to 50s, the actual value is 50000000 with the counter clock at 100MHz/2 */
#define CLOCKS_PER_SEC 50000000
timer_info_t g_timer_info;
uint32_t g_capture_overflows = 0U;

/* Porting : Timing functions
        How to capture time and convert to seconds must be ported to whatever is
   supported by the platform. e.g. Read value from on board RTC, read value from
   cpu clock cycles performance counter etc. Sample implementation for standard
   time.h and windows.h definitions included.
*/
CORETIMETYPE
barebones_clock()
{
	fsp_err_t err = FSP_SUCCESS;
	timer_status_t status;
	err = R_GPT_StatusGet (&g_timer_periodic_ctrl, &status);
    if (FSP_SUCCESS != err)
    {
    	ee_printf("ERROR: R_GPT_StatusGet!\n");
    }
    /* The period is set to 50s we shouldn't overflow but just in case
       report an error if we do. If we set the a shorter period we need to do:
       info.period_counts * g_capture_overflows   */
    if(g_capture_overflows > 0)
    {
    	ee_printf("ERROR: Timer overflow!\n");
    }
    return status.counter;
}
/* Define : TIMER_RES_DIVIDER
        Divider to trade off timer resolution and total time that can be
   measured.

        Use lower values to increase resolution, but make sure that overflow
   does not occur. If there are issues with the return value overflowing,
   increase this value.
        */
#define GETMYTIME(_t)              (*_t = barebones_clock())
#define MYTIMEDIFF(fin, ini)       ((fin) - (ini))
#define TIMER_RES_DIVIDER          1
#define SAMPLE_TIME_IMPLEMENTATION 1
#define EE_TICKS_PER_SEC           (CLOCKS_PER_SEC / TIMER_RES_DIVIDER)

/** Define Host specific (POSIX), or target specific global time variables. */
static CORETIMETYPE start_time_val, stop_time_val;

/* Function : start_time
        This function will be called right before starting the timed portion of
   the benchmark.

        Implementation may be capturing a system timer (as implemented in the
   example code) or zeroing some system parameters - e.g. setting the cpu clocks
   cycles to 0.
*/
void
start_time(void)
{
    GETMYTIME(&start_time_val);
}
/* Function : stop_time
        This function will be called right after ending the timed portion of the
   benchmark.

        Implementation may be capturing a system timer (as implemented in the
   example code) or other system parameters - e.g. reading the current value of
   cpu cycles counter.
*/
void
stop_time(void)
{
    GETMYTIME(&stop_time_val);
}
/* Function : get_time
        Return an abstract "ticks" number that signifies time on the system.

        Actual value returned may be cpu cycles, milliseconds or any other
   value, as long as it can be converted to seconds by <time_in_secs>. This
   methodology is taken to accommodate any hardware or simulated platform. The
   sample implementation returns millisecs by default, and the resolution is
   controlled by <TIMER_RES_DIVIDER>
*/
CORE_TICKS
get_time(void)
{
    CORE_TICKS elapsed
        = (CORE_TICKS)(MYTIMEDIFF(stop_time_val, start_time_val));
    return elapsed;
}
/* Function : time_in_secs
        Convert the value returned by get_time to seconds.

        The <secs_ret> type is used to accommodate systems with no support for
   floating point. Default implementation implemented by the EE_TICKS_PER_SEC
   macro above.
*/
secs_ret
time_in_secs(CORE_TICKS ticks)
{
    secs_ret retval = ((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
    return retval;
}

ee_u32 default_num_contexts = 1;

/* Function : portable_init
        Target specific initialization code
        Test for some common mistakes.
*/
void
portable_init(core_portable *p, int *argc, char *argv[])
{
    fsp_err_t err = FSP_SUCCESS;

    /* Flush C cache */
    uint32_t * c_cache = (uint32_t *)0x40007004;
    *c_cache = 1;
    /* Enable C cache */
    c_cache = (uint32_t *)0x40007000;
    *c_cache = 1;

    /* Flush S cache */
    uint32_t * s_cache = (uint32_t *)0x40007044;
    *s_cache = 1;
    /* Flush S cache */
    s_cache = (uint32_t *)0x40007040;
    *s_cache = 1;

    /* Initialize GPT Timer */
    err = R_GPT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
    if (FSP_SUCCESS != err)
    {
    	ee_printf("ERROR: R_GPT_Open!\n");
    }
    err = R_GPT_Start(&g_timer_periodic_ctrl);
    if (FSP_SUCCESS != err)
    {
    	ee_printf("ERROR: R_GPT_Start!\n");
    }
    err = R_GPT_InfoGet(&g_timer_periodic_ctrl, &g_timer_info);
    if (FSP_SUCCESS != err)
    {
        ee_printf("ERROR: R_GPT_InfoGet!\n");
    }
    if (sizeof(ee_ptr_int) != sizeof(ee_u8 *))
    {
        ee_printf(
            "ERROR! Please define ee_ptr_int to a type that holds a "
            "pointer!\n");
    }
    if (sizeof(ee_u32) != 4)
    {
        ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\n");
    }
    p->portable_id = 1;
}
/* Function : portable_fini
        Target specific final code
*/
void
portable_fini(core_portable *p)
{
	fsp_err_t err = FSP_SUCCESS;

	err = R_GPT_Stop(&g_timer_periodic_ctrl);
	if (FSP_SUCCESS != err)
	{
	 	ee_printf("ERROR: R_GPT_Stop!\n");
	}
    p->portable_id = 0;
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR(0);
}

/* Example callback called when timer expires. */
void timer_callback (timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
    	g_capture_overflows++;
    }
}
