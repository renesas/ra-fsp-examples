/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"
#include "common_utils.h"

#define NUM_STAGES    (8)               /* Number of IIRFA biquad stages, half of the filter order */
#define NUM_PERIODS   (16)              /* Number of periods to process in the filter */
#define RES           (256)             /* Samples per period of input sinusoid */
#define LENGTH        (RES*NUM_PERIODS) /* Length of input and output signals */

/* Sets a breakpoint and notify users once signal processing is complete */
#define APP_COMPLETION_TRAP {\
SEGGER_RTT_printf(SEGGER_INDEX, "\r\nFilter processing is complete. Output signal is ready to be exported for visualization.\r\n");\
__asm("BKPT #0\n");}

/* Variable specifies the chunk size of data to pass to the function R_IIRFA_Filter() */
static const uint16_t  NUM_SAMP = 256;

/* Input signal noise is a 256 samples/period sinusoid @ 100 Hz with added random noise */
static const float noise[] = {
1151,3855,-4477,1965,902,-2379,-538,1100,7776,6168,-2219,6809,2127,559,2199,363,574,3925,3805,3867,2384,-1421,2563,4475,2179,3338,2747,
676,1938,-238,3230,-903,-708,-141,-4480,4527,2279,97,4480,-1804,1519,1261,2434,2445,56,1788,1533,3176,4149,4198,175,2117,-514,-295,1982,
5146,440,2785,1570,4325,-190,2111,3177,4301,5208,2220,-1014,516,-143,6838,756,3540,1602,3805,406,-913,-970,2926,1546,1489,4777,2447,2233,
5056,133,3182,3439,1202,2115,-744,-738,1796,3029,6812,116,1830,1242,-2584,437,-2378,2980,-600,1382,21,1716,-178,2012,2479,4425,477,-3551,
-937,3510,-1507,2610,848,3488,-3519,43,-2075,6305,1990,3074,-1968,-810,-459,2299,-570,1385,-4304,-876,-1888,-3481,739,226,-332,-3181,
1811,170,-1208,-597,-1227,-4322,-1370,-2533,-2882,-3290,-2059,-5112,921,-31,-1180,-1251,-2855,826,-1572,-2802,1391,-1873,-2655,-2085,
-3255,-3846,3590,1775,-1016,-4250,-3476,-2092,-137,-4510,-6578,-4797,-1168,-1071,-967,-2178,-1552,-2921,-195,-4763,-1056,-3737,-2695,
-886,102,-4322,543,-691,-2185,-2447,-2494,-2668,-1999,-1937,-347,1094,-1070,-2448,-729,-1623,-4096,-30,-1332,-1669,-873,-1376,-3821,
-2205,-2151,-2919,1638,-3576,-2748,-3189,-4108,-2069,-2207,1518,-2094,-3731,1765,1045,-1919,-4497,-2287,-1658,-734,-1795,-312,-377,
-3700,-3038,-2571,-2051,-1623,-896,-7080,-1766,1760,-2922,1222,74,-655,-174,-3704,-623,2884,-150,-217,-1755,-265,324,772,-815,-486
};

/* Output signal */
volatile float output[LENGTH] = {0};

/* Variables to support the filter processing */
static volatile int cnt = 0;
static volatile int cnt_period = 0;
static volatile bool processing = 1;
static volatile bool filter_err = 0;

volatile fsp_err_t err = FSP_SUCCESS;

/* Biquad state data (clear to start) */
iir_filter_state_t gp_iirfa0_filter_state[NUM_STAGES] = {0};

const iir_filter_coeffs_t gp_iirfa0_filter_coeffs[NUM_STAGES] =
{
     /* Insert coefficients from 'IIRFA_coeffs.txt' generated from the 'extract_coefficients.m' MATLAB script below this line. */
{ //stage 1
	.b0 = 0.0021671222057193517684937F,
	.b1 = 0.0000000000000000000000000F,
	.b2 = -0.0021671222057193517684937F,
	.a1 = 1.9951459169387817382812500F,
	.a2 = -0.9956698417663574218750000F,
},
{ //stage 2
	.b0 = 0.0035693196114152669906616F,
	.b1 = 0.0000000000000000000000000F,
	.b2 = -0.0035693196114152669906616F,
	.a1 = 1.9944081306457519531250000F,
	.a2 = -0.9950836896896362304687500F,
},
{ //stage 3
	.b0 = 0.0007819454185664653778076F,
	.b1 = 0.0000000000000000000000000F,
	.b2 = -0.0007819454185664653778076F,
	.a1 = 1.9985574483871459960937500F,
	.a2 = -0.9990341067314147949218750F,
},
{ //stage 4
	.b0 = 0.0060391305014491081237793F,
	.b1 = 0.0000000000000000000000000F,
	.b2 = -0.0060391305014491081237793F,
	.a1 = 1.9980473518371582031250000F,
	.a2 = -0.9987925291061401367187500F,
},
{ //stage 5
	.b0 = 0.0033282302320003509521484F,
	.b1 = 0.0000000000000000000000000F,
	.b2 = -0.0033282302320003509521484F,
	.a1 = 1.9941017627716064453125000F,
	.a2 = -0.9946704506874084472656250F,
},
{ //stage 6
	.b0 = 0.0038115631323307752609253F,
	.b1 = 0.0000000000000000000000000F,
	.b2 = -0.0038115631323307752609253F,
	.a1 = 1.9938050508499145507812500F,
	.a2 = -0.9944270253181457519531250F,
},
{ //stage 7
	.b0 = 0.0014748690882697701454163F,
	.b1 = 0.0000000000000000000000000F,
	.b2 = -0.0014748690882697701454163F,
	.a1 = 1.9967080354690551757812500F,
	.a2 = -0.9972006082534790039062500F,
},
{ //stage 8
	.b0 = 0.0052448874339461326599121F,
	.b1 = 0.0000000000000000000000000F,
	.b2 = -0.0052448874339461326599121F,
	.a1 = 1.9958966970443725585937500F,
	.a2 = -0.9966164231300354003906250F,
},


};

/* Filter configuration */
iir_filter_cfg_t g_iirfa0_filter_cfg =
{
    .p_filter_coeffs = (iir_filter_coeffs_t *) gp_iirfa0_filter_coeffs, // Pointer to filter coefficient array
    .p_filter_state  = gp_iirfa0_filter_state,  // Pointer to filter state data array
    .stage_base      = 0,                       // Which hardware biquad stage to start allocation from (0-31)
    .stage_num       = NUM_STAGES,              // Number of stages to allocate
};


FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{

    /* Initialize the IIRFA */
    err = R_IIRFA_Open(&g_iirfa0_ctrl, &g_iirfa0_cfg);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {   /* IIRFA module open failed */
        APP_ERR_PRINT("\r\n ** IIRFA Open API failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Configure filter with coefficients and state variables */
    err = R_IIRFA_Configure(&g_iirfa0_ctrl, &g_iirfa0_filter_cfg);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {   /* IIRFA module configure failed */
        APP_ERR_PRINT("\r\n ** IIRFA Configure API failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Initialize the AGT timer and start it */
    err = R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {   /* AGT module open failed */
        APP_ERR_PRINT("\r\n ** AGT Open API failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    err = R_AGT_Start(&g_timer0_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {   /* AGT module start failed */
        APP_ERR_PRINT("\r\n ** AGT Start API failed ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Code will loop until filter has processed all periods specified by NUM_PERIODS */
    while(processing){
        /* Handle error */
        if(filter_err)
        {   /* IIR module filter failed */
            APP_ERR_PRINT("\r\n ** IIRFA Filter API failed ** \r\n");
            APP_ERR_TRAP(err);
        }

    }

    /* Once this breakpoint is reached the output signal is ready to be exported */
    APP_COMPLETION_TRAP;

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif

/* AGT callback function which will trigger at 25600Hz */
void agt_callback(timer_callback_args_t *p_args){
    FSP_PARAMETER_NOT_USED(p_args);
    /* Filter 1 period of the sinusoid with the R_IIRFA_Filter function */
    err = R_IIRFA_Filter(&g_iirfa0_ctrl, &noise[0], (float *) &output[cnt], NUM_SAMP);

    /* Handle error */
    if (FSP_SUCCESS != err)
    {   /* IIRFA module filter failed */
        filter_err = 1;
    }

    /* Increment the period and sample count */
    cnt += NUM_SAMP;
    ++cnt_period;

    /* Stop the timer after all samples have been processed */
    if(cnt_period >= NUM_PERIODS)
    {
        processing = 0;
        err = R_AGT_Stop(&g_timer0_ctrl);
    }

    return;
}

