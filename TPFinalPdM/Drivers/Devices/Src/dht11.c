/**
 * dht11.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */

#include <stdbool.h>
#include "dht11.h"
#include "delay.h"

static GPIO_TypeDef *data_port;
static uint16_t data_pin;

static dht11Data_t latestRead = { 0, 0 };

bool dht11Init(GPIO_TypeDef *dataPort, uint16_t dataPin) {
	bool result = false;

	delayUsInit();

	data_port = dataPort;
	data_pin = dataPin;

	if (dataPort == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if (dataPort == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	else if (dataPort == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	else if (dataPort == GPIOH)
		__HAL_RCC_GPIOH_CLK_ENABLE();
	else
		return false;

	/** Configures DIO as output */

	GPIO_InitTypeDef gpio = { 0 };
	gpio.Mode = GPIO_MODE_OUTPUT_OD;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	gpio.Pin = data_pin;
	HAL_GPIO_Init(data_port, &gpio);

	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);

	result = true; // TODO: check if GPIO or DWT setup can fail

	return result;

}

static bool_t validateChecksum(uint8_t *dataRaw) {
	/**
	 * The checksum of DHT11 frame is calculated as the sum of the first 4 bytes.
	 */
	return (uint8_t) (dataRaw[0] + dataRaw[1] + dataRaw[2] + dataRaw[3])
			== dataRaw[4];

}

bool dht11Read(void) {
	/**
	 * Sequence:
	 *
	 * Source: https://controllerstech.com/using-dht11-sensor-with-stm32/
	 *
	 * Must disable IRQ so we get more precise clock cycle readings
	 *
	 * 1) configure dio as output
	 * 2) set pin LOW
	 * 3) wait 18-20 us
	 * 4) set dio high
	 * 5) configure dio as INPUT
	 * 6) start reading 4 bytes
	 * 7) verify checksum
	 *
	 */

	const uint32_t cyclesPerUs = SystemCoreClock / 1000000U;

	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_RESET);

	HAL_Delay(20);

	__disable_irq();
	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);

	/**
	 * Read start signal from DHT11:
	 * 0) Wait the line to go from HIGH to LOW; wait 120 us
	 * 1) Read LOW for 80-120 us
	 * 2) Read HIGH for 80-120 us
	 *
	 * 80 us did not work okay so we add 50% to the response window (120 us).
	 */

	uint32_t start = DWT->CYCCNT;
	while (HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_SET) {
		if ((DWT->CYCCNT - start) > 120U * cyclesPerUs) {
			// ABORT, TIMEOUT. DID NOT GO FROM HIGH TO LOW
			__enable_irq();
			return false;
		}
	}

	start = DWT->CYCCNT;
	while (HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_RESET) {
		if ((DWT->CYCCNT - start) > 120U * cyclesPerUs) {
			// ABORT, TIMEOUT. DID NOT GO FROM HIGH TO LOW
			__enable_irq();
			return false;
		}
	}

	start = DWT->CYCCNT;
	while (HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_SET) {
		// 100 us timeout
		if ((DWT->CYCCNT - start) > 120U * cyclesPerUs) {
			// ABORT, TIMEOUT. DID NOT GO FROM HIGH TO LOW
			__enable_irq();
			return false;
		}
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
	 * Response window is too thin so we increase it.
	 *
	 */

	// 0 = humi int, 1 = humi dec, 2 = temp int, 3 = temp dec, 4 = checksum
	uint8_t dataRaw[] = { 0, 0, 0, 0, 0 };
	uint8_t currentByte = 0;

	int8_t bitPosition = 7;
	for (uint8_t i = 0; i < 40; i++) {
		start = DWT->CYCCNT;
		do {
			if ((DWT->CYCCNT - start) > 100U * cyclesPerUs) {
				// ABORT. DID NOT GO HIGH
				__enable_irq();
				return false;
			}

		} while (HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_RESET ? 1 : 0);

		start = DWT->CYCCNT;

		while (HAL_GPIO_ReadPin(data_port, data_pin) == GPIO_PIN_SET) {
			// MAX 120 US TIMEOUT
			if ((DWT->CYCCNT - start) > 120U * cyclesPerUs) {
				__enable_irq();
				return false;
			}
		}
		uint32_t highCycles = DWT->CYCCNT - start;
		// Determinate if bit is 0 or 1 using cycles (47 that is between 26 and 70 us).
		uint8_t bit = (highCycles > 47U * cyclesPerUs) ? 1 : 0;
		currentByte = (i / 8);
		dataRaw[currentByte] |= (bit << bitPosition);

		bitPosition--;
		if (bitPosition < 0)
			bitPosition = 7;
	}

	__enable_irq();

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

void dht11LatestRead(dht11Data_t *data) {
	if (data == NULL)
		return;
	data->temperature = latestRead.temperature;
	data->humidity = latestRead.humidity;
}

