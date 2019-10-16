/***********************************************************************************************************************
 * File Name    : agt_ep.c
 * Description  : Contains data structures and functions used in agt_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas RA Flex Software Package (FSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#include "common_utils.h"
#include "agt_ep.h"
#include "icu_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup agt_ep
 * @{
 **********************************************************************************************************************/

extern bsp_leds_t g_bsp_leds;
extern volatile uint8_t g_icu_flag;
extern volatile uint8_t g_one_shot_timer_flag ;
extern volatile uint8_t g_periodic_timer_flag ;
extern volatile uint32_t g_error_flag ;
/*******************************************************************************************************************//**
 * @brief       This functions initializes AGT module.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Upon successful open of AGT module
 * @retval      Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
fsp_err_t agt_init(void)
{
    fsp_err_t err = FSP_SUCCESS;     // Error status

    /* Open Timer0 in One Shot Mode */
    err = R_AGT_Open(&g_timer_one_shot_ctrl, &g_timer_one_shot_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nAGT0 timer open failed.\r\nRestart the Application\r\n");
        return err;
    }

    /* Open Timer1 in Periodic mode */
    err = R_AGT_Open(&g_timer_periodic_ctrl, &g_timer_periodic_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        /* Close Timer0 in One Shot Mode */
        if ( (FSP_SUCCESS != R_AGT_Close(&g_timer_one_shot_ctrl)))
        {
            APP_ERR_PRINT ("\r\nOne shot timer close failed.\r\nRestart the Application\r\n");
        }
        APP_ERR_PRINT ("\r\nAGT1 timer open failed.\r\nRestart the Application\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions starts AGT0 in one-shot mode.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Timer started successfully
 * @retval      Any Other Error code apart from FSP_SUCCESS  
 **********************************************************************************************************************/
fsp_err_t agt_start_oneshot_timer(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start Timer0 in one-shot mode */
    err = R_AGT_Start(&g_timer_one_shot_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n AGT0 timer start failed.\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions stops the timers.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Timer stopped successfully
 * @retval      Any Other Error code apart from FSP_SUCCESS  
 **********************************************************************************************************************/
fsp_err_t agt_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Stop timer running in one-shot mode */
    err = R_AGT_Stop(&g_timer_one_shot_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCould not stop AGT0 timer.\r\n");
        return err;
    }

    /* Stop timer running in periodic mode */
    err = R_AGT_Stop(&g_timer_periodic_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nCould not stop AGT1 timer.\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions stops the timers and disables push button.
 * @param[IN]   None
 * @retval      FSP_SUCCESS                  Timer is stopped and push button is disabled
 * @retval      Any Other Error code apart from FSP_SUCCESS  
 **********************************************************************************************************************/
fsp_err_t stop_timer_disble_icu(void)
{
    fsp_err_t err = FSP_SUCCESS;  //Error status

    /* Variable to get the status of timer */
    timer_status_t periodic_timer_status;

    /* Retrieve the status of timer running in periodic mode */
    err = R_AGT_StatusGet(&g_timer_periodic_ctrl, &periodic_timer_status);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nR_AGT_StatusGet failed.\r\n");
    }

    if (TIMER_STATE_COUNTING == periodic_timer_status.state)
    {
        /* Stop the timer only when it is running */
        err = agt_stop();
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT ("\r\nStopping Timer failed.\r\nClosing all drivers.\r\n");
            return err;
        }
        else
        {
            g_one_shot_timer_flag = RESET_FLAG; //Clear the flag as timer is stopped
        }

        /* Disable the interrupt on the pin to prevent re-triggering the same interrupt due to switch de-bouncing */
        err = ext_irq_disable();
        if (FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\nDisabling the interrupt failed.\r\nClosing all drivers.\r\n");
        }
        else
        {
            g_icu_flag = RESET_FLAG; //Clear flag since ICU is disabled
        }
    }
    else
    {
        APP_PRINT("\r\nAGT timer is not yet started");
        if(g_icu_flag)
        {
            APP_PRINT("\r\nPress the Switch to start the AGT0 timer");
        }
        APP_PRINT("\r\nEnter any other key to go back to the main menu\r\nUser Input:  ");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief This function is callback for one-shot mode timer and starts AGT1 timer in Periodic mode.
 *
 * @param[in] (timer_callback_args_t *)
 **********************************************************************************************************************/
void one_shot_timer_callback(timer_callback_args_t *p_args)
{
    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        fsp_err_t err = FSP_SUCCESS;
        timer_status_t periodic_timer_status;

        /* Retrieve the status of timer running in periodic mode */
        err = R_AGT_StatusGet(&g_timer_periodic_ctrl, &periodic_timer_status);
        if (FSP_SUCCESS != err)
        {
            g_error_flag = SET_FLAG;
        }
        else
        {
            if (TIMER_STATE_COUNTING != periodic_timer_status.state)
            {
                /* Start the timer in periodic mode only if the timer is in stooped state */
                err = R_AGT_Start(&g_timer_periodic_ctrl);
                /* Handle error */
                if (FSP_SUCCESS != err)
                {
                    g_error_flag = SET_FLAG;
                    APP_ERR_PRINT("\r\nAGT1 Cannot be started in Periodic mode");
                }
                else
                {
                    g_periodic_timer_flag = SET_FLAG;   //Set the flag since timer1 is starrted in periodic mode
                }
            }
            else
            {
                g_periodic_timer_flag = ALREADY_RUNNING;
            }
        }
    }
}

/*******************************************************************************************************************//**
 * @brief This function is callback for periodic timer and blinks LED on every 1 Second.
 *
 * @param[in] (timer_callback_args_t *)
 **********************************************************************************************************************/
void periodic_timer_callback(timer_callback_args_t *p_args)
{
    static volatile uint8_t led_level = BSP_IO_LEVEL_HIGH;

    /* If this board has no LEDs then trap here */
    if (LED_COUNT_ZERO == g_bsp_leds.led_count)
    {
        g_error_flag = SET_FLAG;
        APP_ERR_PRINT ("\r\nError: No Led's Found on the board");
        return;
    }

    /* Change the state of the LED write value */
    led_level ^= BSP_IO_LEVEL_HIGH;

    if(TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* Change LED state */
        fsp_err_t err = R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[RESET_FLAG], led_level);
        /* Handle error */
        if (FSP_SUCCESS != err)
        {
            g_error_flag = SET_FLAG;
            APP_ERR_PRINT("\r\nLED_PIN state cannot be toggled");
            return;
        }
    }
}

/*******************************************************************************************************************//**
 * @brief This function closes opened AGT module before the project ends up in an Error Trap.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void agt_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close AGT0 module */
    err = R_AGT_Close(&g_timer_one_shot_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* AGT0 Close failure message */
        APP_ERR_PRINT("** R_AGT_Close API for channel 0 failed **  \r\n");
    }

    /* Close AGT1 module */
    err = R_AGT_Close(&g_timer_periodic_ctrl);
    /* handle error */
    if (FSP_SUCCESS != err)
    {
        /* AGT1 Close failure message */
        APP_ERR_PRINT("** R_AGT_Close API for channel 1 failed **  \r\n");
    }

}

/*******************************************************************************************************************//**
 * @} (end addtogroup agt_ep)
 **********************************************************************************************************************/





