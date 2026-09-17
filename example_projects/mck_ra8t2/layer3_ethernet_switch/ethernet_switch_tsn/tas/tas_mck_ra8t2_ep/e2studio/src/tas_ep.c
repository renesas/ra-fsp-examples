/***********************************************************************************************************************
 * File Name    : tas_ep.c
 * Description  : Reusable TAS (IEEE 802.1Qbv) setup module. Calls FSP APIs, checks and
 *                returns error status, prints nothing.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <string.h>
#include "tas_ep.h"

#if (TAS_MAX_ENTRIES_PER_GATE < ((2U * TAS_MAX_WINDOWS_PER_ROW) + 1U))
 #error "TAS_MAX_ENTRIES_PER_GATE too small for TAS_MAX_WINDOWS_PER_ROW"
#endif

/* Private functions declarations */
static bool      tas_mac_is_set(const uint8_t mac[6]);
static bool      tas_profile_is_valid(const tas_profile_t * p_profile);
static fsp_err_t tas_prepare_switch_config(const tas_profile_t * p_profile);
static uint8_t   tas_build_gate(const tas_row_t * p_row, layer3_switch_tas_entry_t * p_dst,
                                layer3_switch_tas_gate_cfg_t * p_gate);
static fsp_err_t tas_resolve_cycle_start(tas_cycle_start_config_t const * p_cycle_start,
                                         uint64_t * p_start_ns,
                                         uint64_t * p_timer_now_ns);
static uint16_t  tas_build_eth_frame(uint8_t * p_buf, const uint8_t * p_dst_mac, uint8_t ipv);

static layer3_switch_tas_entry_t s_entries[BSP_FEATURE_ETHER_NUM_CHANNELS]
                                           [BSP_FEATURE_ESWM_ETHA_IPV_QUEUE_NUM]
                                           [TAS_MAX_ENTRIES_PER_GATE];
static layer3_switch_tas_cfg_t s_tas_cfg[BSP_FEATURE_ETHER_NUM_CHANNELS];
static bool s_port_used[BSP_FEATURE_ETHER_NUM_CHANNELS];
static ether_switch_cfg_t s_switch_cfg;
static layer3_switch_extended_cfg_t s_switch_extended_cfg;
static uint64_t s_cycle_start_ns;   /* Anchor point tas_wait_next_cycle() resyncs against */

static const layer3_switch_tas_gate_cfg_t s_gate_closed =
{
    .initial_gate_state = LAYER3_SWITCH_TAS_GATE_STATE_CLOSE,
    .tas_entry_num      = 1U,
    .p_tas_entry_list   = NULL,     /* Filled in per-port at use (needs a stable 1-entry array) */
};
static const layer3_switch_tas_entry_t s_entry_closed = {LAYER3_SWITCH_TAS_GATE_STATE_CLOSE, 0U};

/***********************************************************************************************************************
 *  Function Name: tas_mac_is_set
 *  Description  : Checks whether a 6-byte MAC address is non-zero.
 *  Arguments    : mac           The MAC address to check.
 *  Return Value : true          At least one byte is non-zero.
 *                 false         All bytes are zero (no MAC table entry for this row).
 **********************************************************************************************************************/
static bool tas_mac_is_set(const uint8_t mac[6])
{
    for (uint8_t b = 0U; b < 6U; b++)
    {
        if (0U != mac[b]) { return true; }
    }
    return false;
}
/***********************************************************************************************************************
* End of function tas_mac_is_set.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_profile_is_valid
 *  Description  : Checks that a selected profile points to a usable schedule and source port.
 *  Arguments    : p_profile     Runtime-selected use-case profile.
 *  Return Value : true          Profile can be used to configure TAS.
 *                 false         Profile is NULL or contains invalid fields.
 **********************************************************************************************************************/
static bool tas_profile_is_valid(const tas_profile_t * p_profile)
{
    return (NULL != p_profile) && (NULL != p_profile->p_rows) && (0U != p_profile->row_count) &&
           (p_profile->row_count <= TAS_ROW_COUNT) &&
           (p_profile->source_port < (BSP_FEATURE_ETHER_NUM_CHANNELS + 1U)) &&
           ((TAS_USE_CASE_PORT_TO_PORT == p_profile->use_case) ||
            (TAS_USE_CASE_CPU_GENERATED == p_profile->use_case));
}
/***********************************************************************************************************************
* End of function tas_profile_is_valid.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_build_gate
 *  Description  : Translates one schedule-table row into a gate config plus its backing entry list.
 *  Arguments    : p_row         Schedule row to translate.
 *                 p_dst         Backing storage for the generated entry list.
 *                 p_gate        Gate config to fill in.
 *  Return Value : n             Number of entries written (always >= 1).
 *                 0             Invalid input (bad window count/ordering, or a window overruns the cycle).
 **********************************************************************************************************************/
static uint8_t tas_build_gate(const tas_row_t * p_row, layer3_switch_tas_entry_t * p_dst,
                              layer3_switch_tas_gate_cfg_t * p_gate)
{
    uint8_t  n        = 0U;
    uint32_t prev_end = 0U;

    if (p_row->window_count > TAS_MAX_WINDOWS_PER_ROW)
    {
        return 0U;
    }

    if (0U == p_row->window_count)
    {
        p_gate->initial_gate_state = LAYER3_SWITCH_TAS_GATE_STATE_CLOSE;
        p_dst[n++] = s_entry_closed;
    }
    else
    {
        p_gate->initial_gate_state = (0U == p_row->windows[0].offset_ns) ?
            LAYER3_SWITCH_TAS_GATE_STATE_OPEN : LAYER3_SWITCH_TAS_GATE_STATE_CLOSE;

        for (uint8_t w = 0U; w < p_row->window_count; w++)
        {
            uint32_t offset_ns   = p_row->windows[w].offset_ns;
            uint32_t duration_ns = p_row->windows[w].duration_ns;

            /* Windows must be ascending, non-overlapping, and within the cycle */
            if ((offset_ns < prev_end) || ((offset_ns + duration_ns) > TAS_CYCLE_TIME_NS))
            {
                return 0U;
            }

            if (offset_ns > prev_end)
            {
                p_dst[n++] = (layer3_switch_tas_entry_t)
                    {LAYER3_SWITCH_TAS_GATE_STATE_CLOSE, offset_ns - prev_end};
            }
            p_dst[n++] = (layer3_switch_tas_entry_t)
                {LAYER3_SWITCH_TAS_GATE_STATE_OPEN, duration_ns};
            prev_end = offset_ns + duration_ns;
        }
        p_dst[n++] = s_entry_closed;    /* Terminator: close for the cycle remainder */
    }

    p_gate->tas_entry_num    = n;
    p_gate->p_tas_entry_list = p_dst;
    return n;
}
/***********************************************************************************************************************
* End of function tas_build_gate.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_prepare_switch_config
 *  Description  : Copies the generated switch configuration and adds the selected profile's gated egress port to
 *                 the source-port forwarding target mask. The generated configuration remains untouched.
 *  Arguments    : p_profile     Runtime-selected traffic profile.
 *  Return Value : FSP_SUCCESS   Mutable switch configuration is ready.
 *                 FSP_ERR_INVALID_ARGUMENT   Profile or a schedule port is invalid.
 *                 FSP_ERR_INVALID_POINTER    Generated switch extended configuration is absent.
 **********************************************************************************************************************/
static fsp_err_t tas_prepare_switch_config(const tas_profile_t * p_profile)
{
    const layer3_switch_extended_cfg_t * p_generated_extended_cfg;

    if (!tas_profile_is_valid(p_profile))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    p_generated_extended_cfg = (const layer3_switch_extended_cfg_t *) g_layer3_switch0_cfg.p_extend;
    if (NULL == p_generated_extended_cfg)
    {
        return FSP_ERR_INVALID_POINTER;
    }

    memcpy(&s_switch_cfg, &g_layer3_switch0_cfg, sizeof(s_switch_cfg));
    memcpy(&s_switch_extended_cfg, p_generated_extended_cfg, sizeof(s_switch_extended_cfg));
    s_switch_cfg.p_extend = &s_switch_extended_cfg;

    for (uint8_t i = 0U; i < p_profile->row_count; i++)
    {
        uint8_t egress_port = p_profile->p_rows[i].port;

        if (egress_port >= BSP_FEATURE_ETHER_NUM_CHANNELS)
        {
            return FSP_ERR_INVALID_ARGUMENT;
        }

        s_switch_extended_cfg.fowarding_target_port_masks[p_profile->source_port] |=
            (uint32_t) (1UL << egress_port);
    }

    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function tas_prepare_switch_config.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_open
 *  Description  : Opens the layer3 switch with a profile-specific base forwarding path. No-op if it is already open.
 *  Arguments    : p_profile     Runtime-selected traffic profile.
 *  Return Value : FSP_SUCCESS   Switch open (or already was).
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_open(const tas_profile_t * p_profile)
{
    fsp_err_t err;

    /* Ctrl.open is 0 until R_LAYER3_SWITCH_Open() sets it; the "is open" sentinel value
     * itself is private to r_layer3_switch.c, so check against zero instead of that value. */
    if (0U != g_layer3_switch0_ctrl.open)
    {
        return FSP_SUCCESS;
    }

    /* Build mutable switch cfg for this profile */
    err = tas_prepare_switch_config(p_profile);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open switch with prepared cfg */
    return R_LAYER3_SWITCH_Open(&g_layer3_switch0_ctrl, &s_switch_cfg);
}
/***********************************************************************************************************************
* End of function tas_open.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_timer_start
 *  Description  : Configures and starts the gPTP timer TAS_GPTP_TIMER points to. No-op when TAS_START_TIMER == 0
 *                 (the clock source is already running, e.g. a gPTP GM/Bridge/Slave project).
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS   Timer configured and started (or start skipped by design).
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_timer_start(void)
{
#if (1 == TAS_START_TIMER)
    gptp_timer_cfg_t cfg = {.clock_period = TAS_GPTP_CLOCK_PERIOD};
    fsp_err_t         err;

    /* Configure TAS timer period */
    err = R_GPTP_TimerCfg(&g_gptp0_ctrl, TAS_GPTP_TIMER, &cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    /* Start TAS timer */
    return R_GPTP_Start(&g_gptp0_ctrl, TAS_GPTP_TIMER);
#else
    return FSP_SUCCESS;
#endif
}
/***********************************************************************************************************************
* End of function tas_timer_start.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_resolve_cycle_start
 *  Description  : Resolves the selected cycle start: a fixed delay from the timer epoch, or a dynamic value read
 *                 from the running timer, offset by a margin and rounded up to the next cycle boundary.
 *  Arguments    : p_cycle_start Runtime-selected mode and fixed delay, when applicable.
 *                 p_start_ns    Resolved cycle start time, in nanoseconds.
 *                 p_timer_now_ns Timer value read while resolving the start.
 *  Return Value : FSP_SUCCESS   Cycle start time resolved.
 *                 Any other error code apart from FSP_SUCCESS (including a driver error while reading the timer).
 **********************************************************************************************************************/
static fsp_err_t tas_resolve_cycle_start(tas_cycle_start_config_t const * p_cycle_start,
                                         uint64_t * p_start_ns,
                                         uint64_t * p_timer_now_ns)
{
    gptp_timer_value_t now = {0};
    fsp_err_t           err;
    uint64_t            now_ns;
    uint64_t            target_ns;

    if ((NULL == p_cycle_start) || (NULL == p_start_ns) || (NULL == p_timer_now_ns))
    {
        return FSP_ERR_INVALID_POINTER;
    }

    /* Read current timer value */
    err = R_GPTP_TimerValueGet(&g_gptp0_ctrl, TAS_GPTP_TIMER, &now);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    now_ns = ((uint64_t) now.time_sec_lower * 1000000000ULL) + (uint64_t) now.time_nsec;
    *p_timer_now_ns = now_ns;

    if (TAS_CYCLE_START_MODE_FIXED == p_cycle_start->mode)
    {
        if ((p_cycle_start->fixed_delay_seconds < TAS_CYCLE_START_FIXED_MIN_S) ||
            (p_cycle_start->fixed_delay_seconds > TAS_CYCLE_START_FIXED_MAX_S))
        {
            return FSP_ERR_INVALID_ARGUMENT;
        }

        *p_start_ns = now_ns + ((uint64_t) p_cycle_start->fixed_delay_seconds * 1000000000ULL);

        return FSP_SUCCESS;
    }

    if (TAS_CYCLE_START_MODE_DYNAMIC != p_cycle_start->mode)
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    target_ns   = now_ns + TAS_CYCLE_START_MARGIN_NS;
    *p_start_ns = ((target_ns + TAS_CYCLE_TIME_NS - 1ULL) / TAS_CYCLE_TIME_NS)
                  * (uint64_t) TAS_CYCLE_TIME_NS;
    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function tas_resolve_cycle_start.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_configure_schedule
 *  Description  : Translates the selected profile rows into TAS gate configuration and programs it on every port the table
 *                 references. Does not enable TAS. Call after tas_timer_start().
 *  Arguments    : p_profile     UART-selected traffic profile that supplies the schedule rows.
 *                 p_cycle_start UART-selected cycle-start mode and fixed delay, when applicable.
 *                 p_info        Filled in with the programmed cycle/port/gate info for the caller to print.
 *                                May be NULL if the caller doesn't need it.
 *  Return Value : FSP_SUCCESS   Schedule programmed on every referenced port.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_configure_schedule(tas_profile_t const * p_profile,
                                 tas_cycle_start_config_t const * p_cycle_start,
                                 tas_info_t * p_info)
{
    uint64_t  start_ns = 0U;
    uint64_t  timer_now_ns = 0U;
    fsp_err_t err;

    if (!tas_profile_is_valid(p_profile) || (NULL == p_cycle_start))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    if (NULL != p_info)
    {
        memset(p_info, 0, sizeof(*p_info));
    }

    /* Resolve cycle start time */
    err = tas_resolve_cycle_start(p_cycle_start, &start_ns, &timer_now_ns);
    if (FSP_SUCCESS != err)
    {
        if (NULL != p_info)
        {
            p_info->timer_now_ns = timer_now_ns;
        }
        return err;
    }

    /* Anchor tas_wait_next_cycle() to this cycle's actual epoch */
    s_cycle_start_ns = start_ns;

    memset(s_tas_cfg, 0, sizeof(s_tas_cfg));
    memset(s_port_used, 0, sizeof(s_port_used));

    for (uint8_t i = 0U; i < p_profile->row_count; i++)
    {
        const tas_row_t * p_row = &p_profile->p_rows[i];

        if ((p_row->port >= BSP_FEATURE_ETHER_NUM_CHANNELS) ||
            (p_row->queue >= BSP_FEATURE_ESWM_ETHA_IPV_QUEUE_NUM))
        {
            return FSP_ERR_INVALID_ARGUMENT;
        }

        layer3_switch_tas_gate_cfg_t gate = {0};
        /* Build gate entries for this row */
        uint8_t n = tas_build_gate(p_row, s_entries[p_row->port][p_row->queue], &gate);
        if (0U == n)
        {
            return FSP_ERR_INVALID_ARGUMENT;
        }

        s_tas_cfg[p_row->port].gate_cfg_list[p_row->queue] = gate;
        s_port_used[p_row->port] = true;
    }

    for (uint8_t port = 0U; port < BSP_FEATURE_ETHER_NUM_CHANNELS; port++)
    {
        if (!s_port_used[port])
        {
            continue;
        }

        /* Any queue not covered by a row stays permanently closed. */
        uint32_t total_entries = 0U;
        for (uint8_t q = 0U; q < BSP_FEATURE_ESWM_ETHA_IPV_QUEUE_NUM; q++)
        {
            if (NULL == s_tas_cfg[port].gate_cfg_list[q].p_tas_entry_list)
            {
                s_entries[port][q][0] = s_entry_closed;
                s_tas_cfg[port].gate_cfg_list[q] = s_gate_closed;
                s_tas_cfg[port].gate_cfg_list[q].p_tas_entry_list = s_entries[port][q];
            }
            total_entries += s_tas_cfg[port].gate_cfg_list[q].tas_entry_num;
        }
        if (total_entries > TAS_MAX_TOTAL_ENTRIES_PER_PORT)
        {
            return FSP_ERR_OUT_OF_MEMORY;
        }

        s_tas_cfg[port].gptp_timer_number     = TAS_GPTP_TIMER;
        s_tas_cfg[port].cycle_time            = TAS_CYCLE_TIME_NS;
        s_tas_cfg[port].cycle_time_start_high = (uint32_t) (start_ns >> 32);
        s_tas_cfg[port].cycle_time_start_low  = (uint32_t) (start_ns & 0xFFFFFFFFULL);

        /* Program gate schedule on this port */
        fsp_err_t err2 = R_LAYER3_SWITCH_ConfigureTAS(&g_layer3_switch0_ctrl, port, &s_tas_cfg[port]);
        if (FSP_SUCCESS != err2)
        {
            return err2;
        }

        if (NULL != p_info)
        {
            p_info->ports[p_info->port_count++] = port;
            for (uint8_t q = 0U; q < BSP_FEATURE_ESWM_ETHA_IPV_QUEUE_NUM; q++)
            {
                p_info->entries_per_gate[port][q] = s_tas_cfg[port].gate_cfg_list[q].tas_entry_num;
            }
        }
    }

    if (NULL != p_info)
    {
        p_info->cycle_start_ns = start_ns;
        p_info->timer_now_ns   = timer_now_ns;
        p_info->cycle_start_lead_ns = start_ns - timer_now_ns;
        p_info->cycle_time_ns  = TAS_CYCLE_TIME_NS;
        p_info->fixed_delay_seconds = p_cycle_start->fixed_delay_seconds;
        p_info->timer_number   = TAS_GPTP_TIMER;
        p_info->cycle_start_mode = p_cycle_start->mode;
    }

    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function tas_configure_schedule.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_configure_mac_table
 *  Description  : Configures the MAC table and adds one entry per schedule row with a non-zero MAC, steered
 *                 from the selected profile source port to its destination port/queue. No-op when TAS_MAC_TABLE_ENABLE == 0.
 *  Arguments    : p_info        Filled in with the programmed MAC-entry count. May be NULL.
 *  Return Value : FSP_SUCCESS   MAC table configured (or skipped by design).
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_configure_mac_table(tas_profile_t const * p_profile, tas_info_t * p_info)
{
#if (1 == TAS_MAC_TABLE_ENABLE)
    layer3_switch_table_cfg_t table_cfg = {0};
    fsp_err_t                 err;
    uint8_t                   mac_count = 0U;

    if (!tas_profile_is_valid(p_profile))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    for (uint8_t i = 0U; i < p_profile->row_count; i++)
    {
        /* Skip rows with no MAC entry */
        if (tas_mac_is_set(p_profile->p_rows[i].mac))
        {
            uint8_t port        = p_profile->p_rows[i].port;
            uint8_t source_port = p_profile->source_port;
            if ((port >= BSP_FEATURE_ETHER_NUM_CHANNELS) ||
                (source_port >= (BSP_FEATURE_ETHER_NUM_CHANNELS + 1U)))
            {
                return FSP_ERR_INVALID_ARGUMENT;
            }
            table_cfg.port_cfg_list[source_port].mac_table_enable   = true;
            table_cfg.port_cfg_list[source_port].mac_address_search =
                LAYER3_SWITCH_MAC_ADDRESS_SEARCH_DESTINATION;
            table_cfg.port_cfg_list[port].mac_table_enable = true;
            mac_count++;
        }
    }
    table_cfg.unsecure_entry_maximum_num = mac_count;

    /* Configure MAC table settings */
    err = R_LAYER3_SWITCH_ConfigureTable(&g_layer3_switch0_ctrl, &table_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    for (uint8_t i = 0U; i < p_profile->row_count; i++)
    {
        const tas_row_t * p_row = &p_profile->p_rows[i];
        /* Skip rows with no MAC entry */
        if (!tas_mac_is_set(p_row->mac))
        {
            continue;
        }

        layer3_switch_table_entry_t entry = {0};
        entry.target_frame.entry_type                 = LAYER3_SWITCH_TABLE_ENTRY_TYPE_MAC;
        entry.target_frame.p_destination_mac_address   = (uint8_t *) p_row->mac;
        entry.entry_cfg.entry_enable                   = true;
        entry.entry_cfg.security_enable                = false;
        entry.entry_cfg.source_ports                   = (uint32_t) (1U << p_profile->source_port);
        entry.entry_cfg.destination_ports               = (uint32_t) (1U << p_row->port);
        entry.entry_cfg.internal_priority_update_enable = true;
        entry.entry_cfg.internal_priority_update_value  = p_row->queue;

        /* Add MAC steering entry */
        err = R_LAYER3_SWITCH_AddTableEntry(&g_layer3_switch0_ctrl,
                                            &entry.target_frame, &entry.entry_cfg);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }

    if (NULL != p_info)
    {
        p_info->mac_entry_count = mac_count;
    }
#else
    (void) p_info;
#endif
    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function tas_configure_mac_table.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_configure_status_pins
 *  Description  : Routes each schedule row's status_pin to its TASSTSR field (ET_TAS_STA0..3 oscilloscope
 *                 pins). No-op when TAS_STATUS_PINS_ENABLE == 0.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS   Status pins routed (or skipped by design).
 *                 FSP_ERR_INVALID_ARGUMENT   A row's status_pin is out of range (> 3).
 **********************************************************************************************************************/
fsp_err_t tas_configure_status_pins(tas_profile_t const * p_profile)
{
#if (1 == TAS_STATUS_PINS_ENABLE)
    uint32_t reg = R_ESWM->TASSTSR;

    if (!tas_profile_is_valid(p_profile))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    for (uint8_t i = 0U; i < p_profile->row_count; i++)
    {
        const tas_row_t * p_row = &p_profile->p_rows[i];
        if (TAS_STATUS_PIN_UNUSED == p_row->status_pin)
        {
            continue;
        }
        if (p_row->status_pin > 3U)
        {
            return FSP_ERR_INVALID_ARGUMENT;
        }

        uint32_t shift = (uint32_t) p_row->status_pin * 8U;
        uint32_t mss   = (uint32_t) (((p_row->port & 0x1U) << 4) | (p_row->queue & 0xFU));

        reg &= ~(0x1FUL << shift);
        reg |= (mss << shift);
    }
    R_ESWM->TASSTSR = reg;
#endif
    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function tas_configure_status_pins.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_enable
 *  Description  : Enables TAS on every port configured by tas_configure_schedule(). Call last.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS   TAS enabled on every configured port.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_enable(void)
{
    fsp_err_t err;

    for (uint8_t port = 0U; port < BSP_FEATURE_ETHER_NUM_CHANNELS; port++)
    {
        if (!s_port_used[port])
        {
            continue;
        }
        /* Enable TAS gating on this port */
        err = R_LAYER3_SWITCH_EnableTAS(&g_layer3_switch0_ctrl, port);
        if (FSP_SUCCESS != err)
        {
            return err;
        }
    }
    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function tas_enable.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_init
 *  Description  : Convenience wrapper: runs open -> timer_start -> configure_schedule -> configure_mac_table ->
 *                 configure_status_pins -> enable, in order, stopping at the first failure.
 *  Arguments    : p_cycle_start Runtime-selected cycle-start mode and fixed delay, when applicable.
 *                 p_info        Passed through to tas_configure_schedule()/tas_configure_mac_table().
 *                                May be NULL.
 *  Return Value : FSP_SUCCESS   Every step succeeded.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_init(tas_profile_t const * p_profile,
                   tas_cycle_start_config_t const * p_cycle_start,
                   tas_info_t * p_info)
{
    fsp_err_t err;

    /* Open switch */
    err = tas_open(p_profile);
    if (FSP_SUCCESS != err) { return err; }

    /* Start timer */
    err = tas_timer_start();
    if (FSP_SUCCESS != err) { return err; }

    /* Program gate schedule */
    err = tas_configure_schedule(p_profile, p_cycle_start, p_info);
    if (FSP_SUCCESS != err) { return err; }

    /* Program MAC table */
    err = tas_configure_mac_table(p_profile, p_info);
    if (FSP_SUCCESS != err) { return err; }

    /* Route status pins */
    err = tas_configure_status_pins(p_profile);
    if (FSP_SUCCESS != err) { return err; }

    /* Enable TAS */
    return tas_enable();
}
/***********************************************************************************************************************
* End of function tas_init.
***********************************************************************************************************************/

/***********************************************************************************************************************
 * CPU-generated frame TX (Use Case 2)
 **********************************************************************************************************************/

#define TAS_TX_HEADER_LEN  (14U)   /* Dst(6) + src(6) + EtherType(2) */

static layer3_switch_descriptor_t           s_tx_desc[TAS_TX_DESC_NUM + 1U];
static uint8_t                              s_tx_frame_buf[TAS_TX_DESC_NUM][TAS_FRAME_SIZE_MAX];
static uint32_t                             s_tx_queue_index;

static layer3_switch_descriptor_queue_cfg_t s_tx_queue_cfg =
{
    .descriptor_format    = LAYER3_SWITCH_DISCRIPTOR_FORMTAT_EXTENDED,
    .type                 = LAYER3_SWITCH_QUEUE_TYPE_TX,
    .write_back_mode      = LAYER3_SWITCH_WRITE_BACK_MODE_FULL,
    .p_descriptor_array   = s_tx_desc,
    .array_length         = TAS_TX_DESC_NUM + 1U,
    .rx_timestamp_storage = LAYER3_SWITCH_RX_TIMESTAMP_STORAGE_DISABLE,
    .ports                = LAYER3_SWITCH_PORT_BITMASK_PORT2,
};

/***********************************************************************************************************************
 *  Function Name: tas_close
 *  Description  : Closes the Layer 3 switch before a runtime profile change and clears the local TX-queue state.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS   Switch closed, or was already closed.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_close(void)
{
    fsp_err_t err;

    if (0U == g_layer3_switch0_ctrl.open)
    {
        return FSP_SUCCESS;
    }

    /* Close switch */
    err = R_LAYER3_SWITCH_Close(&g_layer3_switch0_ctrl);
    if (FSP_SUCCESS == err)
    {
        s_tx_queue_index = 0U;
        memset(s_tx_desc, 0, sizeof(s_tx_desc));
    }

    return err;
}
/***********************************************************************************************************************
* End of function tas_close.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_build_eth_frame
 *  Description  : Builds a minimal Ethernet frame: [dst MAC 6B][src MAC 6B][EtherType 2B][payload ...].
 *  Arguments    : p_buf         Destination buffer for the built frame.
 *                 p_dst_mac     Destination MAC address (6 bytes).
 *                 ipv           IPV/queue value written as the first payload byte.
 *  Return Value : len           Total frame length in bytes.
 **********************************************************************************************************************/
static uint16_t tas_build_eth_frame(uint8_t * p_buf, const uint8_t * p_dst_mac, uint8_t ipv)
{
    static const uint8_t src_mac[6] = TAS_TX_SOURCE_MAC;

    memcpy(&p_buf[0], p_dst_mac, 6U);
    memcpy(&p_buf[6], src_mac, 6U);
    p_buf[12] = (uint8_t) (TAS_TX_ETHERTYPE >> 8);
    p_buf[13] = (uint8_t) (TAS_TX_ETHERTYPE & 0xFFU);
    p_buf[TAS_TX_HEADER_LEN] = ipv;
    memset(&p_buf[TAS_TX_HEADER_LEN + 1U], 0xABU, 32U);

    return (uint16_t) (TAS_TX_HEADER_LEN + 1U + 32U);
}
/***********************************************************************************************************************
* End of function tas_build_eth_frame.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_wait_link
 *  Description  : Blocks until TAS_CPU_PORT's link partner PHY (the PHY instance for TAS_DESTINATION_PORT) is up.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS   Link is up.
 **********************************************************************************************************************/
fsp_err_t tas_wait_link(void)
{
    ether_phy_instance_t const * p_phy;

    if (0U == TAS_DESTINATION_PORT)
    {
        p_phy = &g_rmac_phy0;
    }
    else if (1U == TAS_DESTINATION_PORT)
    {
        p_phy = &g_rmac_phy1;
    }
    else
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }

    /* Poll PHY link status */
    while (FSP_SUCCESS != p_phy->p_api->linkStatusGet(p_phy->p_ctrl))
    {
        /* Wait before retry */
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
    }
    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function tas_wait_link.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_tx_queue_create
 *  Description  : Creates the GWCA TX descriptor queue used to inject frames from TAS_CPU_PORT. Call once.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS   Descriptor queue created.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_tx_queue_create(void)
{
    /* Create TX descriptor queue */
    return R_LAYER3_SWITCH_CreateDescriptorQueue(&g_layer3_switch0_ctrl,
                                                 &s_tx_queue_index, &s_tx_queue_cfg);
}
/***********************************************************************************************************************
* End of function tas_tx_queue_create.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_send_frames
 *  Description  : Builds and submits one Ethernet frame per selected-profile schedule row with a non-zero mac[] --
 *                 one frame per IPV queue so TAS can shape each class.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS   Every matching row's frame submitted.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t tas_send_frames(tas_profile_t const * p_profile)
{
    fsp_err_t                  err;
    layer3_switch_descriptor_t desc = {0};

    if (!tas_profile_is_valid(p_profile))
    {
        return FSP_ERR_INVALID_ARGUMENT;
    }
    if (TAS_USE_CASE_CPU_GENERATED != p_profile->use_case)
    {
        return FSP_ERR_UNSUPPORTED;
    }

    for (uint8_t i = 0U; i < p_profile->row_count; i++)
    {
        const tas_row_t * p_row = &p_profile->p_rows[i];
        if (!tas_mac_is_set(p_row->mac))
        {
            continue;
        }

        /* Build frame for this row */
        uint16_t len = tas_build_eth_frame(s_tx_frame_buf[p_row->queue], p_row->mac, p_row->queue);

        memset(&desc, 0, sizeof(desc));
        desc.basic.dt     = LAYER3_SWITCH_DESCRIPTOR_TYPE_FSINGLE;
        desc.basic.ds_l   = (uint8_t) (len & 0xFFU);
        desc.basic.ds_h   = (uint8_t) ((len >> 8U) & 0x0FU);
        desc.basic.die    = 1U;
        desc.basic.ptr_l  = (uint32_t) (uintptr_t) s_tx_frame_buf[p_row->queue];
        desc.basic.ptr_h  = 0U;
        desc.info1_tx.fmt = 1U;
        desc.info1_tx.dv  = (uint8_t) (1U << p_row->port);
        desc.info1_tx.ipv = p_row->queue;

        /* GWCA stops after each FSINGLE and does not auto-chain to the next slot.
         * StartDescriptorQueue always resets tail=0, so every iteration writes to slot[0];
         * retry until the previous TX write-back (dt: FSINGLE -> FEMPTY) releases it. */
        do
        {
            /* Submit descriptor */
            err = R_LAYER3_SWITCH_SetDescriptor(&g_layer3_switch0_ctrl, s_tx_queue_index, &desc);
            if (FSP_ERR_IN_USE == err)
            {
                /* Wait for descriptor slot to free */
                R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
            }
        } while (FSP_ERR_IN_USE == err);
        if (FSP_SUCCESS != err) { return err; }

        do
        {
            /* Kick TX queue */
            err = R_LAYER3_SWITCH_StartDescriptorQueue(&g_layer3_switch0_ctrl, s_tx_queue_index);
            if (FSP_ERR_IN_USE == err)
            {
                /* Wait for queue to free */
                R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
            }
        } while (FSP_ERR_IN_USE == err);
        if (FSP_SUCCESS != err) { return err; }
    }

    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function tas_send_frames.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_wait_next_cycle
 *  Description  : Delays until the next TAS cycle boundary, resyncing to the timer each call instead of
 *                 accumulating drift from an open-loop software delay. Falls back to a plain cycle-length
 *                 delay if the timer read fails.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void tas_wait_next_cycle(void)
{
    gptp_timer_value_t now = {0};
    uint64_t           now_ns;
    uint64_t           elapsed_ns;
    uint64_t           remaining_ns;

    /* Read current timer value */
    if (FSP_SUCCESS != R_GPTP_TimerValueGet(&g_gptp0_ctrl, TAS_GPTP_TIMER, &now))
    {
        /* Fall back to a plain cycle-length delay */
        R_BSP_SoftwareDelay(TAS_CYCLE_TIME_NS / 1000U, BSP_DELAY_UNITS_MICROSECONDS);
        return;
    }

    now_ns     = ((uint64_t) now.time_sec_lower * 1000000000ULL) + (uint64_t) now.time_nsec;
    elapsed_ns = (now_ns >= s_cycle_start_ns) ? (now_ns - s_cycle_start_ns) : 0U;

    /* Resync to the next cycle boundary instead of drifting further each call */
    remaining_ns = (uint64_t) TAS_CYCLE_TIME_NS - (elapsed_ns % (uint64_t) TAS_CYCLE_TIME_NS);

    R_BSP_SoftwareDelay((uint32_t) (remaining_ns / 1000ULL), BSP_DELAY_UNITS_MICROSECONDS);
}
/***********************************************************************************************************************
* End of function tas_wait_next_cycle.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_idle_wait
 *  Description  : Delays briefly while the switch independently shapes external traffic.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void tas_idle_wait(void)
{
    /* Short idle delay */
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
}
/***********************************************************************************************************************
* End of function tas_idle_wait.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: tas_wait_for_input
 *  Description  : Delays briefly while the example checks UART input for a runtime profile change.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void tas_wait_for_input(void)
{
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
}
/***********************************************************************************************************************
* End of function tas_wait_for_input.
***********************************************************************************************************************/
