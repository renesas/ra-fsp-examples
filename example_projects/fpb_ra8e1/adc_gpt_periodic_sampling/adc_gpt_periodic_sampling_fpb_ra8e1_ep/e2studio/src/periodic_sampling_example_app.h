 /**********************************************************************************************************************
 * File Name    : periodic_sampling_example_app.h
 * Description  : Public interface for the ADC GPT Periodic Sampling example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef PERIODIC_SAMPLING_EXAMPLE_APP_H_
#define PERIODIC_SAMPLING_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "adc_gpt_periodic_sampling"

#define BANNER_INFO     "\r\n************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module   *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                        *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                   *"\
                        "\r\n************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the basic functionality of ADC GPT periodic"\
		                "\r\nsampling on Renesas RA MCUs based on FSP. On successful initialization of ADC,"\
		                "\r\nGPT, ELC and DTC modules, GPT triggers an ADC group scan at periodic intervals."\
		                "\r\nWhen each group scan completes, DTC triggers data transfer and copies data to"\
		                "\r\nuser buffer. The sample ADC data can be viewed through waveform rendering of"\
		                "\r\nmemory using memory viewer in e2studio.\r\n"\

#define EP_GUIDE        "\r\nThe user can now open waveform rendering of memory in e2studio to observe the"\
                        "\r\noutput of ADC GPT periodic sampling.\r\n"

/* Single entry-point called in hal_entry.c */
void periodic_sampling_example_app_run(void);

#endif /* PERIODIC_SAMPLING_EXAMPLE_APP_H_ */
