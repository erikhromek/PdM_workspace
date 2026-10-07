/**
 * dht11.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */

#include <stdbool.h>
#include "dht11.h"
#include "stm32f4xx_hal.h"

static TIM_HandleTypeDef htim;
static uint16_t pin;
static dht11Data_t latestRead;

bool dht11Init(uint16_t gpio, TIM_TypeDef tim) {
	bool result = false;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* ===================================================================== */

	/**
	 * Configure Timer
	 * Source: MX_TIM1_Init function from STM32CubeMX and
	 * https://controllerstech.com/create-microsecond-delay-stm32/
	 */

	HAL_TIM_Base_Start(&htim);

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	htim1.Instance = tim;
	htim1.Init.Prescaler = 72 - 1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 65535;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		result = false;
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		result = false;
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		result = false;
	} else {
		HAL_TIM_Base_Start(&htim);
		pin = gpio;
		result = true;
	}

	return result;

}

/**
 * @fn void delay_us(uint16_t)
 * @brief Generates a delay
 *
 * @pre timer must be initialized
 * @param us delay in microseconds
 */
static void delay_us(uint16_t us) {
	/**
	 * Source: https://controllerstech.com/create-microsecond-delay-stm32/
	 */

	if (htim == NULL)
		return;
	__HAL_TIM_SET_COUNTER(&htim, 0);
	while (__HAL_TIM_GET_COUNTER(&htim) < us)
		;
}

static bool dht11Read() {
	/**
	 * Sequence:
	 * 1) configure pin as OUTPUT
	 * 2) set pin LOW
	 * 3) wait 18 us
	 * 4) set pin HIGH
	 * 5) configure as INPUT
	 * 6) start reading 4 bytes
	 * 7) verify checksum
	 */

	/**
	 * Configure GPIO
	 * Source:
	 * https://deepbluembedded.com/stm32-gpio-pin-read-lab-digital-input/
	 */

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_RESET);

	delay_us(18);

	HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_SET);

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	delay_us(20);

	/**
	 * Read start signal from DHT11:
	 * 1) Read LOW for 80 us
	 * 2) Read HIGH for 80 us
	 */

	uint8_t waitTime = 0;
	while ((HAL_GPIO_ReadPin(GPIOA, pin) == GPIO_PIN_RESET) || waitTime < 90) {
		delay_us(1);
		waitTime++;
	}
	if (waitTime < 80 && waitTime >= 90) {
		// ABORT, TIMEOUT
		return false;
	}

	waitTime = 0;

	while ((HAL_GPIO_ReadPin(GPIOA, pin) == GPIO_PIN_SET) || waitTime < 90) {
		delay_us(1);
		waitTime++;
	}
	if (waitTime < 80 && waitTime >= 90) {
		// ABORT, TIMEOUT
		return false;
	}

	/**
	 *
	 * Read data (40 bits to read):
	 *
	 * 1) Pin stays LOW for 50 us
	 * 2) Pin stays HIGH for 26-30 us if bit is 0 or 70 ms if bit is 1
	 * 3) Starts again
	 *
	 * Starts from the MSB
	 *
	 */

	// 0 = humi int, 1 = humi dec, 2 = temp int, 3 = temp dec, 4 = checksum
	uint8_t dataRaw[] = { 0, 0, 0, 0, 0 };
	currentByte = 0;

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
		} while (HAL_GPIO_ReadPin(GPIOA, pin) == GPIO_PIN_RESET ? 1 : 0);

		while (HAL_GPIO_ReadPin(GPIOA, pin) == GPIO_PIN_SET) {
			if (waitTime > 30) {
				bit = 1;
				break;
			}
			delay_us(1);
			waitTime++;
		}
		currentByte = (i / 8);
		dataRaw[currentByte] = dataRaw[currentByte] || (bit << bitPosition);

		bitPosition--;
		if (bitPosition < 0)
			bitPosition = 7;
	}

	if (dataRaw[0] + dataRaw[1] + dataRaw[2] + dataRaw[3] != dataRaw[4])
		// CHECKSUM ERROR, ABORT
		return false;
	else {
		// Ignore decimal part
		latestRead->humidity = dataRaw[0];
		latestRead->temperature = dataRaw[2];
		return true;
	}


}

dht11Status_t getLatestRead(dht11Data_t *data) {
	data->temperature = latestRead->temperature;
	data->humidity = latestRead->humidity;
}

static void readByte(); // Lee un byte y aplica la espera activa correspondiente
static bool_t validateChecksum(); // Chequea que coincida el último byte con los datos enviados

