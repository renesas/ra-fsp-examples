/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the �contents�) are proprietary and confidential to Renesas Electronics Corporation 
 * and/or its licensors (�Renesas�) and subject to statutory and contractual protections.
 *
 * Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
 * display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
 * purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED �AS IS� WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
 * NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
 * INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
 * be subject to different terms.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* File Name    : brightness.h
* Description  : Brightness settings.
***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           TODO       1.00    Initial Release.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup Thermostat
 * @defgroup Brightness
 *
 * Brightness settings are controlled in this module.
 *
 * @{
 **********************************************************************************************************************/

#ifndef BRIGHTNESS_H
#define BRIGHTNESS_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

/** Included for access to PWM control block. */
#include "hal_data.h"

/**********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define GPT_PWM_MAX_PERCENT      (100U)
/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
Function prototypes
***********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @brief  Increases screen brightness within allowable configurable range and updates state.
 *
 * @param[in,out]  p_brightness  Current brightness.
***********************************************************************************************************************/
void brightness_up(uint8_t * p_brightness);

/*******************************************************************************************************************//**
 * @brief  Decreases screen brightness within allowable configurable range and updates state.
 *
 * @param[in,out]  p_brightness  Current brightness.
***********************************************************************************************************************/
void brightness_down(uint8_t * p_brightness);

#endif /* BRIGHTNESS_H */

/*******************************************************************************************************************//**
 * @}
***********************************************************************************************************************/
