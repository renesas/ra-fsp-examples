 /**********************************************************************************************************************
 * File Name    : interval_sampling_example_app.h
 * Description  : Public interface for the ADC TAU Interval Sampling example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef INTERVAL_SAMPLING_EXAMPLE_APP_H_
#define INTERVAL_SAMPLING_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "adc_tau_interval_sampling"

#define BANNER_INFO     "\r\n************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module   *"\
                        "\r\n*   Example Project Version "EP_VERSION"                                        *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                   *"\
                        "\r\n************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of ADC interval"\
		                "\r\nsampling using the TAU module on Renesas RA MCUs based on Renesas FSP."\
		                "\r\nWhen ADC interval data sampling is performed, TAU triggers an ADC group"\
		                "\r\nscan at a periodic interval. When the group scan completes, the data"\
		                "\r\ntransfer controller (DTC) triggers a data transfer, copying the scan"\
		                "\r\nresult to a user buffer which is configured as ping-pong between the two"\
		                "\r\nbuffers to allow for continuous processing of the acquired data. The"\
		                "\r\nsample data can be observed using waveform rendering in the memory viewer"\
		                "\r\nin e2studio.\r\n"

#define EP_GUIDE        "\r\nThe user can now open waveform rendering of memory in e2studio to"\
		                "\r\nobserve the output of ADC TAU interval sampling.\r\n"

/* Single entry-point called in hal_entry.c */
void interval_sampling_example_app_run(void);

#endif /* INTERVAL_SAMPLING_EXAMPLE_APP_H_ */
