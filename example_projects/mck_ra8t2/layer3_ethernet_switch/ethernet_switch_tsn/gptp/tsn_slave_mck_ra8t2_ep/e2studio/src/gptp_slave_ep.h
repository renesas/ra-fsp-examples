/***********************************************************************************************************************
 * File Name    : gptp_slave_ep.h
 * Description  : Public interface for Slave ESWM initialization.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef GPTP_SLAVE_EP_H_
#define GPTP_SLAVE_EP_H_

#define GPTP_ESWM_EXAMPLE_INSTANCE_NUMBER        (4)
#define GPTP_ESWM_EXAMPLE_SLAVE_PRIORITY         (255)

/* Ethernet port this board runs gPTP on. Must match, in the RA Configurator:
 * - the RMAC instance "Channel" property,
 * - the slot the RMAC instance occupies in the gPTP ESWM "Ethernet" list,
 * - the slot the RMAC PHY instance occupies in the Layer3 Switch "Ethernet PHY" list.
 * The gPTP timer number of this port must be 0 (local clock), because the middleware
 * expects the hardware SFD capture in the Timer0 domain and converts it itself. */
#define GPTP_SLAVE_ACTIVE_PORT                   (1)
#include "hal_data.h"
#include "hw_adapt.h"
#include "r_ptp_configuration_api.h"

/* Public functions declarations */
fsp_err_t gptp_init(void);
void gptp_service_tick(void);

#endif /* GPTP_SLAVE_EP_H_ */
