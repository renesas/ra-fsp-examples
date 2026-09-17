/***********************************************************************************************************************
 * File Name    : gptp_gm_ep.h
 * Description  : Public interface for Grand Master ESWM initialization.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPTP_GM_EP_H_
#define GPTP_GM_EP_H_

/* ---- Instance / priority configuration ------------------------------------- */
#define GPTP_ESWM_EXAMPLE_MASTER_PRIORITY  (50)   /* Win BMCA as Grandmaster */

/* ---- Port configuration ---------------------------------------------------- */
/* Ethernet port this board runs gPTP on. Must match, in the RA Configurator:
 * - the RMAC instance "Channel" property,
 * - the slot the RMAC instance occupies in the gPTP ESWM "Ethernet" list,
 * - the slot the RMAC PHY instance occupies in the Layer3 Switch "Ethernet PHY" list.
 * The gPTP timer number of this port must be 0 (local clock), because the middleware
 * expects the hardware SFD capture in the Timer0 domain and converts it itself. */

/* ---- Includes -------------------------------------------------------------- */
#include "hal_data.h"
#include "hw_adapt.h"
#include "r_ptp_configuration_api.h"

/* Public functions declarations */
fsp_err_t gptp_gm_init(void);
void gptp_service_tick(void);

#endif /* GPTP_GM_EP_H_ */
