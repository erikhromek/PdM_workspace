/*
 * api_uart.c
 *
 *  Created on: 24 sept 2026
 *      Author: erik
 */

#include <API_uart.h>

static UART_HandleTypeDef huart1;
#define ARRAY_MAX_SIZE 256;

static uint8_t INIT_MESSAGE[] = "UART INICIALIZED SUCCESFULLY\r\n";

bool uartInit() {
	huart1.Instance = USART2;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		return false;
	} else {
		uartSendString(INIT_MESSAGE);
		return true;
	}

}

/*
 * @brief Checks array size, max ARRAY_MAX_SIZE
 */
static uint16_t countSize(uint8_t *pstring) {
	uint16_t size = 0;

	for (int i = 0; i < 256; i++) {
		if (pstring[i] == '\0')
			break;
		size++;
	}
	return size;
}

void uartSendString(uint8_t *pstring) {
	HAL_UART_Transmit(&huart1, pstring, countSize(pstring),
	UART_TIMEOUT_MS);
}

void uartSendStringSize(uint8_t *pstring, uint16_t size) {
	// Debe validar del array antes de enviarlo
	uint16_t arraySize = size > countSize(pstring) ? countSize(pstring) : size;
	HAL_UART_Transmit(&huart1, pstring, arraySize, UART_TIMEOUT_MS);
}

void uartReceiveStringSize(uint8_t *pstring, uint16_t size) {
	HAL_UART_Receive(&huart1, pstring, size, UART_TIMEOUT_MS);
}
