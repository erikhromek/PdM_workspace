/**
 * dht11.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */

#include <stdbool.h>
#include "dht11.h"
#include "stm32f4xx_hal.h"
#include "delay.h"

static GPIO_TypeDef *data_port;
static uint16_t data_pin;

static dht11Data_t latestRead;

bool dht11Init(GPIO_TypeDef *dataPort, uint16_t dataPin) {
	bool result = false;

	delayUsInit();

	data_port = dataPort;
	data_pin = dataPin;

	if (dataPort == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if (dataPort == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	else
		__HAL_RCC_GPIOC_CLK_ENABLE();

	// Configure DATA GPIO AS OUTPUT OPEN DRAIN

	GPIO_InitTypeDef gpio = { 0 };
	gpio.Mode = GPIO_MODE_OUTPUT_OD;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	gpio.Pin = data_pin;
	HAL_GPIO_Init(data_port, &gpio);

	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);

	result = true; // TODO: check if GPIO or DWT can fail

	return result;

}

static bool_t validateChecksum(uint8_t *dataRaw) {
	return (uint8_t) (dataRaw[0] + dataRaw[1] + dataRaw[2] + dataRaw[3])
			== dataRaw[4];

}

bool dht11Read(void) {
	/**
	 * Sequence:
	 *
	 * Source: https://controllerstech.com/using-dht11-sensor-with-stm32/
	 *
	 * 1) configure pin as OUTPUT
	 * 2) set pin LOW
	 * 3) wait 18-20 us
	 * 4) set pin HIGH
	 * 5) configure as INPUT
	 * 6) start reading 4 bytes
	 * 7) verify checksum
	 */

	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_RESET);

	HAL_Delay(20);

	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);

	/**
	 * Read start signal from DHT11:
	 * 0) Wait the line to go from HIGH to LOW
	 * 1) Read LOW for 80 us
	 * 2) Read HIGH for 80 us
	 */

	uint8_t waitTime = 0;
	while (HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_SET) {
		delay_us(1);
		waitTime++;
		if (waitTime >= 40) {
			// ABORT, TIMEOUT. DID NOT GO FROM HIGH TO LOW
			return false;
		}
	}

	waitTime = 0;

	while ((HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_RESET)
			&& waitTime < 90) {
		delay_us(1);
		waitTime++;
	}
	if (waitTime >= 90) {
		// ABORT, TIMEOUT
		return false;
	}

	waitTime = 0;

	while ((HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_SET)
			&& waitTime < 90) {
		delay_us(1);
		waitTime++;
	}
	if (waitTime >= 90) {
		// ABORT, TIMEOUT
		return false;
	}

	/**
	 *
	 * Read data (40 bits to read):
	 *
	 * 1) Pin stays LOW for 50 us
	 * 2) Pin stays HIGH for 26-30 us if bit is 0 or 70 us if bit is 1
	 * 3) Starts again
	 *
	 * Starts from the MSB
	 *
	 */

	// 0 = humi int, 1 = humi dec, 2 = temp int, 3 = temp dec, 4 = checksum
	uint8_t dataRaw[] = { 0, 0, 0, 0, 0 };
	uint8_t currentByte = 0;

	int8_t bitPosition = 7;
	for (uint8_t i = 0; i < 40; i++) {
		waitTime = 0;
		uint8_t bit = 0;
		do {
			waitTime++;
			if (waitTime > 50) {
				// ABORT, TIMEOUT
				return false;
			}
			delay_us(1);
		} while (HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_RESET ? 1 : 0);

		waitTime = 0;

		while (HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_SET) {
			if (waitTime > 30) {
				bit = 1;
			}
			delay_us(1);
			waitTime++;
			if (waitTime > 90) {
				// ABORT, TIMEOUT. MAX 70 US
				return false;
			}
		}
		currentByte = (i / 8);
		dataRaw[currentByte] |= (bit << bitPosition);

		bitPosition--;
		if (bitPosition < 0)
			bitPosition = 7;
	}

	if (validateChecksum(dataRaw)) {
		// Ignore decimal part
		latestRead.humidity = dataRaw[0];
		latestRead.temperature = dataRaw[2];
		return true;
	}

	else {
		// CHECKSUM ERROR, ABORT
		return false;
	}
}

void getLatestRead(dht11Data_t *data) {
	data->temperature = latestRead.temperature;
	data->humidity = latestRead.humidity;
}

