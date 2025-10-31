/*
 * glcdc_display.h
 *
 *  Created on: Jun 26, 2025
 *      Author: a5145403
 */

#ifndef GLCDC_DISPLAY_H_
#define GLCDC_DISPLAY_H_

#include "common_utils.h"
#include "user_config.h"

#if (DISPLAY_OUTPUT == 1U)

#define RESET_FLAG              (0U)
#define SET_FLAG                (1U)

/* Public function declarations */
fsp_err_t glcdc_init(void);

#endif /* DISPLAY_OUTPUT */
#endif /* GLCDC_DISPLAY_H_ */
