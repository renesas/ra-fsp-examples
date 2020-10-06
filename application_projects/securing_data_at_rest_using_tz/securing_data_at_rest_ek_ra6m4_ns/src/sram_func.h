/*
 * sram_func.h
 *
 *  Created on: Sep 6, 2020
 *      Author: xianghui.wang
 */

#ifndef SRAM_FUNC_H_
#define SRAM_FUNC_H_

#define SRAM_PROGRAM __attribute__((section (".code_in_ram")))

SRAM_PROGRAM void write_to_secure_sram(void);
SRAM_PROGRAM void read_secure_sram(void);

#endif /* SRAM_FUNC_H_ */
