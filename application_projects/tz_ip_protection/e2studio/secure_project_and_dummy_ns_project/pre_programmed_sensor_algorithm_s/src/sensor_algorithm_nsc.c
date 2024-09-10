/***********************************************************************************************************************
 * File Name    : sensor_algorithm_nsc.c
 * Description  : implementation of sensor IP algorithm non-secure callable APIs
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "hal_data.h"
#include "sensor_algorithm_nsc.h"

#define MINIMAL_TEMPERATURE_READING                    (1140)  /* this is about 0 degree C */
#define TEMPERATURE_READING_GAP_BETWEEN_TWO_SETTINGS    (12)    /* this is about 2.4 degree C */

/* globals */

extern bsp_leds_t g_bsp_leds;


/* locals */

static uint16_t g_adc_data;
static unsigned int Threshold = MINIMAL_TEMPERATURE_READING;
static unsigned int threshold_already_setup = 0;

static void setup_threshold(void);
static void adc_operation(void);

/* Define the units to be used with the software delay function */
 const bsp_delay_units_t bsp_delay_units = BSP_DELAY_UNITS_MILLISECONDS;

 /* Set the blink frequency (must be <= bsp_delay_units */
 const uint32_t freq_in_hz = 4;

 /* Calculate the delay in terms of bsp_delay_units */
 const uint32_t delay = bsp_delay_units / freq_in_hz;

 /*****************************************************************************************************************
  *  @brief      adc_operation
  *              This function read adc reading to g_adc_data
  *  @retval     none
  *  ****************************************************************************************************************/
 static void adc_operation(void)
 {
     fsp_err_t status = FSP_SUCCESS;
     status = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
     if(FSP_SUCCESS == status)
     {
         R_BSP_SoftwareDelay(delay, bsp_delay_units);
        status = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
        if(FSP_SUCCESS == status)
        {
            R_BSP_SoftwareDelay(delay, bsp_delay_units);
            status = R_ADC_ScanStart(&g_adc0_ctrl);
            if(FSP_SUCCESS == status)
            {
                R_BSP_SoftwareDelay(delay, bsp_delay_units);
                status = R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_TEMPERATURE, &g_adc_data);
                if(FSP_SUCCESS == status)
                {
                  status = R_ADC_ScanStop (&g_adc0_ctrl);
                }

                status = R_ADC_Close(&g_adc0_ctrl);
            }
        }
     }

}

 /*****************************************************************************************************************
  *  @brief      setup_threshold
  *              This function is called once every reset to take the current temperature as the reference point to
  *              decide whether the temperature change is beyond the threshold
  *  @retval     none
  *  ****************************************************************************************************************/
static void setup_threshold(void)
{
   adc_operation();
   if (g_adc_data > MINIMAL_TEMPERATURE_READING)
   {
       Threshold = g_adc_data;
       threshold_already_setup = 1;
   }
}

/*****************************************************************************************************************
  *  @brief      toggle_led
  *              This function toggles the requested led
  *  @retval     none
  *  ****************************************************************************************************************/
static void toggle_led( uint32_t pin)
{

    /* Holds level to set for pins */
    bsp_io_level_t pin_level = BSP_IO_LEVEL_HIGH;

    for (int i = 0; i < 4; i++)
    {
        /* Enable access to the PFS registers. If using r_ioport module then register protection is automatically
        * handled. This code uses BSP IO functions to show how it is used.
        */
        R_BSP_PinAccessEnable();

        /* Write to this pin */
        R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);

        /* Protect PFS registers */
        R_BSP_PinAccessDisable();

        /* Toggle level for next write */
        if (BSP_IO_LEVEL_LOW == pin_level)
        {
          pin_level = BSP_IO_LEVEL_HIGH;
        }
        else
        {
          pin_level = BSP_IO_LEVEL_LOW;
        }

        /* Delay */
        R_BSP_SoftwareDelay(delay, bsp_delay_units);
    }
}
/*****************************************************************************************************************
 *  @brief      sensor_algorithm_start: non-secure callable function
 *              This function will start the ADC to read the temperature.
 *              It will blink the green led if the temperature is lower than:
 *              reference temperature reading + TEMPERATURE_READING_GAP_BETWEEN_TWO_SETTINGS
 *              It will blink the red led if the temperature is higher than:
 *              reference temperature reading + TEMPERATURE_READING_GAP_BETWEEN_TWO_SETTINGS
 *  @retval     none
 *  ****************************************************************************************************************/

BSP_CMSE_NONSECURE_ENTRY void sensor_algorithm_start_guard(void)
{
    bsp_leds_t leds = g_bsp_leds;

    if(threshold_already_setup == 0)
        setup_threshold();
    else
        adc_operation();

    if(g_adc_data > 0)
    {
        /* temperature is below the Threshold */
       if(g_adc_data < (Threshold + TEMPERATURE_READING_GAP_BETWEEN_TWO_SETTINGS))
       {

            /* Get pin to toggle - Blue LED*/
            uint32_t pin = leds.p_leds[BSP_LED_LED2];   /* Green LED */
            toggle_led(pin);

       }
       /* temperature is above threshold */
       else
       {
            /* Get pin to toggle */
            uint32_t pin = leds.p_leds[BSP_LED_LED3];  /* Red LED */
            toggle_led(pin);
       }
    }

}

BSP_CMSE_NONSECURE_ENTRY void sensor_algorithm_stop_guard(void)
{
    bsp_leds_t leds = g_bsp_leds;


    /* Get pin to toggle */
    uint32_t pin = leds.p_leds[BSP_LED_LED1]; /* Blue LED */
    toggle_led(pin);

}
