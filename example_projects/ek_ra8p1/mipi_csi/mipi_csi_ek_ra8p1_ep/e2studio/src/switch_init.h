/*
 * switch_init.h
 *
 *  Created on: Jul 18, 2025
 *      Author: a5145403
 */

#ifndef SWITCH_INIT_H_
#define SWITCH_INIT_H_

#define HIGH_STATE                  (1U)
#define LOW_STATE                   (0U)

#define SWITCH_ADDR                 (0x43)
#define SOFT_RESET_REG              (0x01)
#define PIN_DIR_REG                 (0x03)
#define OUTPUT_STATE_REG            (0x05)
#define OUTPUT_ENABLE_REG           (0x07)

#define OUTPUT_DIR                  (1U)
#define OUTPUT_ENABLE               (0U)

/* Public function declaration */
fsp_err_t set_switch_state(uint8_t sw_num, uint8_t target_state);

#endif /* SWITCH_INIT_H_ */
