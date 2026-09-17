/***********************************************************************************************************************
 * File Name    : gptp_bridge_ep.h
 * Description  : Public interface for Bridge ESWM initialization.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPTP_BRIDGE_EP_H_
#define GPTP_BRIDGE_EP_H_

/* ---- Instance / port configuration ---------------------------------------- */
#define GPTP_ESWM_EXAMPLE_MASTER_PRIORITY          (120)
/* ---- Bridge port roles ------------------------------------------------------ */
#define GPTP_BRIDGE_SLAVE_PORT  (0)    /* Port 0: receives Sync from upstream GM */
#define GPTP_BRIDGE_MASTER_PORT (1)    /* Port 1: forwards Sync to downstream nodes */

/* ---- Includes -------------------------------------------------------------- */
#include "hal_data.h"
#include "hw_adapt.h"
#include "r_ptp_configuration_api.h"

/* Public functions declarations */
fsp_err_t gptp_bridge_init(void);
void gptp_service_tick(void);

#endif /* GPTP_BRIDGE_EP_H_ */
