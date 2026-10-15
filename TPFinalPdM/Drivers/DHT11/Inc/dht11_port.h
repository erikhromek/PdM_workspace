/*
 * dht1port.h
 *
 *  Created on: Octubre 2026
 *      Author: Erik Hromek
 */

#ifndef DHT11_INC_DHT11_PORT_H_
#define DHT11_INC_DHT11_PORT_H_

#include <stdbool.h>
#include <stdint.h>

void dht11PortInit(void);
void dht11PortDataLow(void);
void dht11PortDataHigh(void);
void dht11PortDelayMs(uint32_t ms);
void dht11PortCriticalEnter(void);
void dht11PortCriticalExit(void);

/**
 * @fn int32_t dht11PortWaitWhile(bool level, uint32_t)
 * @brief Waits for the Data pin to change its level
 *
 * @param level Level of Pin to check (high or low)
 * @param timeoutUs Maximun timeout in us
 * @return	us elapsed or -1 if timeout
 */
int32_t  dht11PortWaitWhile(bool level, uint32_t timeoutUs);

#endif /* DHT11_INC_DHT11_PORT_H_ */
