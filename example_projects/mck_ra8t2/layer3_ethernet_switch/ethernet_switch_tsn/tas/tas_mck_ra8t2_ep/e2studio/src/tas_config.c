/***********************************************************************************************************************
 * File Name    : tas_config.c
 * Description  : TAS gate schedule and runtime-selectable traffic-source profiles.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "tas_config.h"

/* Shared 25 ms gate schedule. Q0-Q3 each open one 5 ms slot back-to-back (0/5/10/15 ms),
 * while Q4-Q7 stay closed. The selected profile supplies the MAC-steering source port: UC1 uses
 * external Port 0, and UC2 uses the internal CPU/GWCA Port 2. */
const tas_row_t g_tas_rows[TAS_ROW_COUNT] =
{
/*   port                  queue  cnt              windows{offset_ns, duration_ns}                    mac                              pin */
    { TAS_DESTINATION_PORT, 0U,   1U, {{        0UL, 5000000UL}, {0UL, 0UL}}, {0x44,0x44,0x44,0x00,0x00,0x0A}, 0U },
    { TAS_DESTINATION_PORT, 1U,   1U, {{  5000000UL, 5000000UL}, {0UL, 0UL}}, {0x44,0x44,0x44,0x00,0x00,0x0B}, 1U },
    { TAS_DESTINATION_PORT, 2U,   1U, {{ 10000000UL, 5000000UL}, {0UL, 0UL}}, {0x44,0x44,0x44,0x00,0x00,0x0C}, 2U },
    { TAS_DESTINATION_PORT, 3U,   1U, {{ 15000000UL, 5000000UL}, {0UL, 0UL}}, {0x44,0x44,0x44,0x00,0x00,0x0D}, 3U },
    { TAS_DESTINATION_PORT, 4U,   0U, {{        0UL,       0UL}, {0UL, 0UL}}, {0x00,0x00,0x00,0x00,0x00,0x00}, TAS_STATUS_PIN_UNUSED },
    { TAS_DESTINATION_PORT, 5U,   0U, {{        0UL,       0UL}, {0UL, 0UL}}, {0x00,0x00,0x00,0x00,0x00,0x00}, TAS_STATUS_PIN_UNUSED },
    { TAS_DESTINATION_PORT, 6U,   0U, {{        0UL,       0UL}, {0UL, 0UL}}, {0x00,0x00,0x00,0x00,0x00,0x00}, TAS_STATUS_PIN_UNUSED },
    { TAS_DESTINATION_PORT, 7U,   0U, {{        0UL,       0UL}, {0UL, 0UL}}, {0x00,0x00,0x00,0x00,0x00,0x00}, TAS_STATUS_PIN_UNUSED },
};

static const tas_profile_t s_uc1_profile =
{
    .use_case    = TAS_USE_CASE_PORT_TO_PORT,
    .source_port = TAS_SOURCE_PORT,
    .p_rows      = g_tas_rows,
    .row_count   = TAS_ROW_COUNT,
};

static const tas_profile_t s_uc2_profile =
{
    .use_case    = TAS_USE_CASE_CPU_GENERATED,
    .source_port = TAS_CPU_PORT,
    .p_rows      = g_tas_rows,
    .row_count   = TAS_ROW_COUNT,
};

/***********************************************************************************************************************
 *  Function Name: tas_config_get_profile
 *  Description  : Returns the TAS profile for a use case selected at runtime.
 *  Arguments    : use_case      Use case selected by the application layer.
 *  Return Value : Pointer to the matching immutable profile, or NULL if use_case is invalid.
 **********************************************************************************************************************/
tas_profile_t const * tas_config_get_profile(tas_use_case_t use_case)
{
    switch (use_case)
    {
        case TAS_USE_CASE_PORT_TO_PORT:
            return &s_uc1_profile;

        case TAS_USE_CASE_CPU_GENERATED:
            return &s_uc2_profile;

        default:
            return NULL;
    }
}
/***********************************************************************************************************************
* End of function tas_config_get_profile.
***********************************************************************************************************************/
