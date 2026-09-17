/***********************************************************************************************************************
 * File Name    : tas_example.h
 * Description  : Public interface for the standalone TAS example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TAS_EXAMPLE_H_
#define TAS_EXAMPLE_H_

#define EP_VERSION      "1.0"
#define MODULE_NAME     "TAS"
#define BANNER_INFO \
    "\r\n**********************************************************************************" \
    "\r\n*   Renesas FSP Example Project for " MODULE_NAME " Module                                   *" \
    "\r\n*   Example Project Version %s                                                  *" \
    "\r\n*   Flex Software Pack Version  %u.%u.%u                                            *" \
    "\r\n**********************************************************************************" \
    "\r\n"
#define EP_INFO \
    "\r\nThis example project demonstrates the IEEE 802.1Qbv Time-Aware Shaper (TAS) on" \
    "\r\nthe Ethernet switch (ESWM) of a single Renesas RA MCU. TAS lets the switch" \
    "\r\nhardware open and close each egress queue's transmission gate on a repeating," \
    "\r\ntime-based cycle, so time-critical traffic classes get guaranteed transmission" \
    "\r\nwindows and cannot be delayed by best-effort traffic.\r\n" \
    "\r\n" \
    "\r\nThis is a standalone, single-board TAS demonstration: it needs no gPTP network" \
    "\r\nand no other TSN nodes. The gate-control schedule is driven by the ESWM's own" \
    "\r\nlocal free-running timer by default, so the whole demonstration runs on one" \
    "\r\nboard.\r\n"

#define MAIN_MENU \
    "\r\n\r\nMain menu:\r\n" \
    "  1. Port 0 ingress -> Port 1 TAS egress\r\n" \
    "  2. CPU/GWCA Port 2 -> Port 1 TAS egress\r\n" \
    "  3. Reconfigure cycle-start settings (default: fixed 3 seconds)\r\n" \
    "Enter 1, 2, or 3, then press Enter: "

#define CYCLE_START_MENU \
    "\r\n\r\nCycle-start mode:\r\n" \
    "  1. Fixed delay after timer start\r\n" \
    "  2. Dynamic start from the running timer\r\n" \
    "Enter 1 or 2, then press Enter: "

/* Public functions declarations */
void tas_example_run(void);

#endif /* TAS_EXAMPLE_H_ */
