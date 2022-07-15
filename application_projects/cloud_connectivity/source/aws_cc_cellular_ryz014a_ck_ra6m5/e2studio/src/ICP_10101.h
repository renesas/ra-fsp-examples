/*
 * ICM10101.h
 *
 *  Created on: Jan 24, 2022
 *      Author: Vivek
 */

#ifndef ICP_10101_H_
#define ICP_10101_H_

#include "stdbool.h"
#include "hal_data.h"

typedef enum
{ FAST,
NORMAL,
ACCURATE,
VERY_ACCURATE
}mmode;

void timer_start(void);
void timer_reset(void);
uint8_t icp_begin(void);
bool isConnected(void);
void measure(mmode mode);
uint8_t measureStart(mmode mode);
bool dataReady();
float getTemperatureC(void);
float getTemperatureF(void);
float getPressurePa(void);
void ICP_10101_get();
uint8_t begin(void);

void _calculate(void);
void _sendCommand(uint16_t cmd);
void _sendCommands(uint8_t *cmd_buf, uint8_t cmd_len);
void _readResponse(uint8_t *res_buf, uint8_t res_len );

uint32_t GetMilliTick(void);


#endif /* ICP_10101_H_ */
