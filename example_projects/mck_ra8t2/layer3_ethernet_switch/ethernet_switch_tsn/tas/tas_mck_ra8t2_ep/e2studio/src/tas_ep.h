/***********************************************************************************************************************
 * File Name    : tas_ep.h
 * Description  : Reusable TAS (IEEE 802.1Qbv) setup module. Calls FSP APIs, checks and
 *                returns error status, prints nothing. Copy this file + tas_ep.c verbatim into another
 *                project; only tas_config.{c,h} needs to change per project.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TAS_EP_H_
#define TAS_EP_H_

#include "hal_data.h"
#include "tas_config.h"

/* Snapshot of what was actually programmed, for tas_example.c to print without recomputing
 * anything or calling any FSP API itself. */
typedef struct st_tas_ep_info
{
    uint64_t cycle_start_ns;
    uint64_t timer_now_ns;
    uint64_t cycle_start_lead_ns;
    uint32_t cycle_time_ns;
    uint32_t fixed_delay_seconds;
    uint8_t  timer_number;
    uint8_t  cycle_start_mode;
    uint8_t  port_count;
    uint8_t  ports[BSP_FEATURE_ETHER_NUM_CHANNELS];
    uint8_t  entries_per_gate[BSP_FEATURE_ETHER_NUM_CHANNELS][BSP_FEATURE_ESWM_ETHA_IPV_QUEUE_NUM];
    uint8_t  mac_entry_count;
} tas_info_t;

/* Public functions declarations */

/* Open the layer3 switch for p_profile, including its source-to-egress base forwarding path. */
fsp_err_t tas_open(const tas_profile_t * p_profile);

/* Close the layer3 switch and its associated resources before a runtime profile change. */
fsp_err_t tas_close(void);

/* Start the gPTP timer TAS_GPTP_TIMER points to (no-op if TAS_START_TIMER == 0). */
fsp_err_t tas_timer_start(void);

/* Translate p_profile's rows into TAS gate configuration and program it on every port the table
 * references. Does not enable TAS. Caller order: after tas_timer_start(). The selected fixed
 * delay is resolved from that timer epoch; dynamic mode reads the running timer and aligns the
 * start to the next cycle boundary. */
fsp_err_t tas_configure_schedule(tas_profile_t const * p_profile,
                                 tas_cycle_start_config_t const * p_cycle_start,
                                 tas_info_t * p_info);

/* Configure the MAC table and add one entry per row with a non-zero MAC, steered from the
 * profile's selected source port. */
fsp_err_t tas_configure_mac_table(tas_profile_t const * p_profile, tas_info_t * p_info);

/* Route each row's status_pin to its TASSTSR field (ET_TAS_STA0..3 oscilloscope pins). */
fsp_err_t tas_configure_status_pins(tas_profile_t const * p_profile);

/* Enable TAS on every port configured by tas_configure_schedule(). Call last. */
fsp_err_t tas_enable(void);

/* Convenience: runs open -> timer_start -> configure_schedule -> configure_mac_table ->
 * configure_status_pins -> enable, in order, stopping at the first failure. */
fsp_err_t tas_init(tas_profile_t const * p_profile,
                   tas_cycle_start_config_t const * p_cycle_start,
                   tas_info_t * p_info);

/* ---- CPU-generated frame TX (Use Case 2) -------------------------------------------------- */

/* Block until TAS_CPU_PORT's link partner PHY (the PHY instance for TAS_DESTINATION_PORT) is up. */
fsp_err_t tas_wait_link(void);

/* Create the GWCA TX descriptor queue used to inject frames from TAS_CPU_PORT. Call once. */
fsp_err_t tas_tx_queue_create(void);

/* Build and submit one Ethernet frame per selected-profile row with a non-zero mac[] -- one frame per
 * IPV queue so TAS can shape each class. This operation supports only the CPU-generated profile. */
fsp_err_t tas_send_frames(tas_profile_t const * p_profile);

/* Delay one TAS cycle without exposing BSP APIs to the example application. */
void tas_wait_next_cycle(void);

/* Idle briefly while an external sender supplies port-to-port traffic. */
void tas_idle_wait(void);

/* Delay briefly while the example checks UART input for a runtime profile change. */
void tas_wait_for_input(void);

#endif /* TAS_EP_H_ */
