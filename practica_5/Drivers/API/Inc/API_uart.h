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

/**
 * @brief Inicializa la UART2 en modo polling.
 *
 *	@return Devuelve true si puedo inicializar el controlador o no.
 */
bool uartInit();

/*
 *  @brief Envía una cadena de caracteres vía UART.
 *
 *	@param pstring	Puntero a un vector de caracteres a enviar
 */
void uartSendString(uint8_t * pstring);

/*
 *  @brief Envía una cadena de caracteres delimitada por un tamaño vía UART
 *
 *	@param pstring	Puntero a un vector de caracteres a enviar
 *	@param size		Cantidad de caracteres a enviar
 */
void uartSendStringSize(uint8_t * pstring, uint16_t size);

/*
 *  @brief Recibe una cantidad de caracteres en un vector
 *
 *	@param pstring	Puntero a un vector de caracteres a almacenar
 *	@param size		Cantidad de caracteres a recibir
 */
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);



#endif /* API_INC_API_UART_H_ */
