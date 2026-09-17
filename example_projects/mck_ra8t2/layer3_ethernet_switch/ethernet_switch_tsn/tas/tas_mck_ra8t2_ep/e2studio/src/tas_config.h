/***********************************************************************************************************************
 * File Name    : tas_config.h
 * Description  : User-editable TAS configuration — schedule table, timer selection, cycle geometry.
 *                This is the only file to edit when moving tas_ep.{c,h} into another project.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TAS_CONFIG_H_
#define TAS_CONFIG_H_

#include <stdint.h>
#include "hal_data.h"

/* ---- Timer selection -------------------------------------------------------
 * TAS independently selects either ESWM timer as its schedule timebase. This standalone
 * example uses timer 0; the egress port does not determine the selected timer.
 * ---- */
#define TAS_GPTP_TIMER                  (0)

/* Start the ESWM timer from tas_ep.
 * 1 = standalone: nothing else starts it.
 * 0 = gPTP project: the gPTP stack already started both timers; starting again
 *     would reset the synchronized clock. */
#define TAS_START_TIMER                 (1)
#define TAS_GPTP_CLOCK_PERIOD           (4)

/* ---- Cycle geometry -------------------------------------------------------- */
#define TAS_CYCLE_TIME_NS               (25000000UL)      /* 25 ms */

/* Runtime cycle-start selection. Fixed values are delays from the timer epoch that starts after
 * the user confirms a traffic profile. Dynamic values are resolved from the running timer. */
#define TAS_CYCLE_START_MODE_FIXED      (0U)
#define TAS_CYCLE_START_MODE_DYNAMIC    (1U)
#define TAS_CYCLE_START_MARGIN_NS       (500000000ULL)
#define TAS_CYCLE_START_FIXED_MIN_S     (1U)
#define TAS_CYCLE_START_FIXED_MAX_S     (60U)
#define TAS_CYCLE_START_FIXED_DEFAULT_S (3U)

/* ---- Switch topology -------------------------------------------------------
 * Port 2 is the CPU / GWCA internal port (frames the CPU itself generates).
 * TAS_SOURCE_PORT (when UC1 is selected) must differ from TAS_DESTINATION_PORT -- the switch does not
 * re-egress a frame out the same physical port it ingressed on. */
#define TAS_SOURCE_PORT                 (0U)
#define TAS_CPU_PORT                    (2U)
#define TAS_DESTINATION_PORT            (1U)

/* ---- MAC table ------------------------------------------------------------- */
#define TAS_MAC_TABLE_ENABLE            (1)

/* ---- Oscilloscope pin routing (ET_TAS_STA0..3) ----------------------------- */
#define TAS_STATUS_PINS_ENABLE          (1)
#define TAS_STATUS_PIN_UNUSED           (0xFFU)

/* ---- Table sizing ---------------------------------------------------------- */
#define TAS_ROW_COUNT                   (8U)
#define TAS_MAX_WINDOWS_PER_ROW         (2U)
#define TAS_MAX_ENTRIES_PER_GATE        ((2U * TAS_MAX_WINDOWS_PER_ROW) + 1U)

/* Total entries across all 8 gates share one RAM region per port. */
#define TAS_MAX_TOTAL_ENTRIES_PER_PORT  (32U)

/* ---- CPU-generated frame TX (Use Case 2) ----------------------------------- */
#define TAS_TX_DESC_NUM                 (5U)
#define TAS_FRAME_SIZE_MAX              (128U)   /* Bytes, enough for a minimal Eth frame */
/* Source MAC placed in generated frames -- this board's Port 1 MAC address (TAS_DESTINATION_PORT,
 * the port these frames actually egress from). */
#define TAS_TX_SOURCE_MAC               {0x74, 0x90, 0x50, 0x6D, 0x82, 0x01}
#define TAS_TX_ETHERTYPE                (0x88B5U)   /* IEEE local experimental */

/* ---- Schedule table -------------------------------------------------------- */

/* One open window inside a cycle: gate stays CLOSED until offset_ns, then OPEN for duration_ns. */
typedef struct st_tas_window
{
    uint32_t offset_ns;
    uint32_t duration_ns;
} tas_window_t;

/* One row per (port, queue). */
typedef struct st_tas_row
{
    uint8_t      port;                                  /* Egress (gated) port */
    uint8_t      queue;                                 /* 0..7, IPV gate index */
    uint8_t      window_count;                          /* 0 = gate permanently CLOSED */
    tas_window_t windows[TAS_MAX_WINDOWS_PER_ROW];      /* Ascending, non-overlapping */
    uint8_t      mac[6];                                /* All-zero = no MAC table entry */
    uint8_t      status_pin;                            /* 0..3 = ET_TAS_STA<n>, else UNUSED */
} tas_row_t;

/* The application selects this once through UART after reset, before TAS setup. */
typedef enum e_tas_use_case
{
    TAS_USE_CASE_PORT_TO_PORT  = 1U,
    TAS_USE_CASE_CPU_GENERATED = 2U,
} tas_use_case_t;

typedef struct st_tas_cycle_start_config
{
    uint8_t  mode;
    uint32_t fixed_delay_seconds;
} tas_cycle_start_config_t;

/* Binds the shared gate schedule to the MAC-steering ingress source. */
typedef struct st_tas_profile
{
    tas_use_case_t   use_case;
    uint8_t          source_port;
    tas_row_t const * p_rows;
    uint8_t          row_count;
} tas_profile_t;

extern const tas_row_t g_tas_rows[TAS_ROW_COUNT];

/* Return the profile selected at runtime, or NULL for an invalid use case. */
tas_profile_t const * tas_config_get_profile(tas_use_case_t use_case);

#endif /* TAS_CONFIG_H_ */
