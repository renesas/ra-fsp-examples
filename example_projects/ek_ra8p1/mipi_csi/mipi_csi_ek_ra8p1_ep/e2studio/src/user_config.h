/*
 * user_config.h
 *
 *  Created on: Jul 11, 2025
 *      Author: a5145403
 */

#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_

#include "common_utils.h"

#define DISPLAY_OUTPUT              (1)
#define AWB_MANUAL_ENABLE           (0)

/* The FPS value depends on the camera input image size.
 * Please refer to the camera sensor’s datasheet before making any adjustments. */
#define FPS_TARGET                  (60)

#endif /* USER_CONFIG_H_ */
