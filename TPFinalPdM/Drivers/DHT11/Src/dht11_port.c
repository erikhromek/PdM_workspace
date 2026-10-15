/*
 * dht11_port.c
 *
 *  Created on: Octubre 2026
 *      Author: Erik Hromek
 */

#include "stm32f4xx_hal.h"
#include "delay.h"
#include "dht11_port.h"

static GPIO_TypeDef *data_port;
static uint16_t data_pin;

void dht11PortInit(void) {
	delayUsInit();

	data_port = GPIOB;
	data_pin = GPIO_PIN_6;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	/** Configures DIO as output */

	GPIO_InitTypeDef gpio = { 0 };
	gpio.Mode = GPIO_MODE_OUTPUT_OD;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	gpio.Pin = data_pin;
	HAL_GPIO_Init(data_port, &gpio);

	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);
}

void dht11PortCriticalEnter(void) {
	__disable_irq();
}

void dht11PortCriticalExit(void) {
	__enable_irq();
}

void dht11PortDataLow(void) {
	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_RESET);
}

void dht11PortDataHigh(void) {
	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);
}

int32_t  dht11PortWaitWhile(bool level, uint32_t timeoutUs) {
	const uint32_t cyclesPerUs = SystemCoreClock / 1000000U;
	uint32_t start = DWT->CYCCNT;
		while (HAL_GPIO_ReadPin(data_port, data_pin) == level) {
			if ((DWT->CYCCNT - start) > timeoutUs * cyclesPerUs) {
				// ABORT, TIMEOUT. DID NOT GO FROM HIGH TO LOW
				return -1;
			}
		}
	return start;
}

void dht11PortDelayMs(uint32_t ms) {
	HAL_Delay(ms);
}
