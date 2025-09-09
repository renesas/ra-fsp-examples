/***********************************************************************************************************************
 * File Name    : usb_typec.h
 * Description  : Contains macros for usb typeC application.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_TYPEC_H_
#define USB_TYPEC_H_

#define ADC_MAX_COUNTS              (4095U)    /* 12-bit ADC */
#define VREF_mV                     (3300.0f)  /* Reference voltage in millivolts */
#define SHUNT_OHMS                  (0.05f)    /* Shunt resistor value for current sensing (Ohms) */
#define ISL28006_GAIN               (20.0f)    /* Gain of ISL28006 */
#define ADC_STATUS_RETRY_MAX        (1000U)
#define WRITE_BUF_SIZE              (64)
#define ERROR_TRAP                  ({ __asm("BKPT #0\n");})
#define ERROR_LED_INDEX             (2)
#define CC_PLUG_STATUS_COUNT        (2)
#define CC_CONNECTION_STATUS_COUNT  (7)

void usb_typec_entry(void);

#endif /* USB_TYPEC_H_ */
