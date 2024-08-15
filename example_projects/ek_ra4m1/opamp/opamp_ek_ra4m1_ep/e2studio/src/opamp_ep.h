/***********************************************************************************************************************
 * File Name    : opamp_ep.h
 * Description  : Contains data structures and functions used in opamap_ep.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OPAMP_EP_H_
#define OPAMP_EP_H_

typedef enum e_module_name
{
    DAC_MODULE,
    ADC_MODULE,
    OPAMP_MODULE,
    ALL
}module_name_t;

#ifndef BOARD_RA4W1_EK
#define ADC_CHANNEL                    ADC_CHANNEL_3
#endif
#define DAC_MAX_VAL                    (4095U)         /* DAC12 maximum value */
#ifdef BOARD_RA4W1_EK
#define OPAMP_CH_MASK                  (1U << 2U)      /* OPAMP channel2 mask */
#else
#define OPAMP_CH_MASK                  (1U << 0U)      /* OPAMP channel0 mask */
#endif
#define NULL_CHAR                      '\0'
#define SET_BIT                        (1U)
#define REF_VOLTAGE                    (float)(3.3f)
#ifdef BOARD_RA2A1_EK
#define STEP_SIZE                      (float)(32767.0f)
#else
#define STEP_SIZE                      (float)(16383.0f)
#endif
#define DAC_RES                        (float)(4095.0f)
#define ANALOG_VALUE(raw_data,module)  {\
                                        char buff_d[64] = {NULL_CHAR,};float a_vol=0.0f;\
                                        if (ADC_MODULE == module){\
                                         a_vol = (float)( (float)(REF_VOLTAGE/STEP_SIZE) * (float)raw_data);\
                                        snprintf(buff_d,64,"OPAMP output voltage in volts is %.02f",a_vol); \
                                          APP_PRINT("%s\n",buff_d); }\
                                          else\
                                        {a_vol = (float)( (float)((float)raw_data/DAC_RES) * REF_VOLTAGE);\
                                        snprintf(buff_d,64,"DAC voltage in volts is %.02f",a_vol); \
                                        APP_PRINT("%s\n",buff_d);}  \
                                        }


#define EP_INFO            "\r\nThis Example Project demonstrates the functionality of OPAMP module. "           \
                           "Analog voltage is provided\r\nthrough DAC module which is taken from user through " \
                           "JLINK RTT viewer. OPAMP processing is triggered\r\nthrough software once user input "\
                           "DAC value in the range of 0 to 4095. OPAMP output is fed to ADC\r\n"                \
                           "in turn the result is displayed on RTTViewer(in volts).\r\n"


#endif /* OPAMP_EP_H_ */
