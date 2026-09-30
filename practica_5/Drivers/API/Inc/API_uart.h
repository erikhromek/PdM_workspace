/*
 * api_uart.h
 *
 *  Created on: 24 sept 2026
 *      Author: erik
 */

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

#define UART_TIMEOUT_MS 100

bool uartInit();
void uartSendString(uint8_t * pstring);
void uartSendStringSize(uint8_t * pstring, uint16_t size);
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);



#endif /* API_INC_API_UART_H_ */
