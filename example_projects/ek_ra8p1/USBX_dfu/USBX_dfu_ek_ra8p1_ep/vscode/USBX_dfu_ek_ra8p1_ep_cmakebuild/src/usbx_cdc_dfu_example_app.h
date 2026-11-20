/***********************************************************************************************************************
 * File Name    : usbx_cdc_dfu_example_app.h
 * Description  : Public interface for the USBX CDC DFU example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_CDC_DFU_EXAMPLE_APP_H_
#define USBX_CDC_DFU_EXAMPLE_APP_H_

/* Single entry-point called in usbx_peri_cdc_dfu_thread_entry.c */
void usbx_peri_cdc_dfu_example_app_run(void);

/* Function declarations */
void current_drivers_un_init(void);
void usbx_cdc_dfu_cleanup(void);

#endif /* USBX_CDC_DFU_EXAMPLE_APP_H_ */
