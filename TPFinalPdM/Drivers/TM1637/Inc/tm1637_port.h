/*
 * tm1637_port.h
 *
 *  Created on: Octubre 2026
 *      Author: Erik Hromek
 */

#ifndef TM1637_INC_TM1637_PORT_H_
#define TM1637_INC_TM1637_PORT_H_

#include <stdbool.h>
#include <stdint.h>

void tm1637PortInit(void);
void tm1637PortClkWrite(bool value);
void tm1637PortDataWrite(bool value);
bool tm1637PortDataRead(void);
void tm1637PortDataInput(void);
void tm1637PortDataOutput(void);
void tm1637PortDelayUs(uint32_t us);

#endif /* TM1637_INC_TM1637_PORT_H_ */
