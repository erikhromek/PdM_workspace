/*
 * tm1637.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */

#include <stdbool.h>
#include "tm1637.h"
#include "stm32f4xx_hal.h"

static TIM_HandleTypeDef htim;
static uint16_t data_pin;
static uint16_t clk_pin;

/* 0xXgfedcba */
// TODO COULD BE AN ARRAY
#define ZERO 0b00111111
#define ONE 0b00000110
#define TWO 0b01011011
#define THREE 0b01001111
#define FOUR 0b01100110
#define FIVE 0b01101101
#define SIX 0b01111100
#define SEVEN 0b00000111
#define EIGHT 0b01111111
#define NINE 0b01100111ve
#define BLANK 0b00000000

#define AUTO_MODE 0x40
// TODO COULD BE AN ARRAY
#define FIRST_DIGIT_ADDRESS 0xC0
#define SECOND_DIGIT_ADDRESS 0xC1
#define THIRD_DIGIT_ADDRESS 0xC2
#define FOURTH_DIGIT_ADDRESS 0xC3

#define BRIGHTNESS_COMMAND 0x8F // 0b10001XXX Last 3 digits set BRIGHTNESS

static uint8_t currentSegment[] = { 0, 0, 0, 0 };

void tm1637Init(uint16_t data_gpio, uint16_t clk_gpio, TIM_TypeDef tim) {
	bool result = false;

	data_pin = data_gpio;
	clk_pin = clk_gpio;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	/**
	 * Configure DATA PGIO as OUTPUT
	 */

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = data_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, data_pin, GPIO_PIN_RESET);

	/**
	 * Configure CLK PGIO as OUTPUT
	 */

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = clk_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, clk_pin, GPIO_PIN_RESET);

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
		result = true;
	}

	return result;
}

static uint8_t tm1637NumberToDigit(uint8_t number) {
	if (number == 0)
		return ZERO;
	if (number == 1)
		return ONE;
	if (number == 2)
		return TWO;
	if (number == 3)
		return THREE;
	if (number == 4)
		return FOUR;
	if (number == 5)
		return FIVE;
	if (number == 6)
		return SIX;
	if (number == 7)
		return SEVEN;
	if (number == 8)
		return EIGHT;
	if (number == 9)
		return NINE;

	return BLANK;

}

static uint8_t* numberToRawData(uint16_t number, uint8_t *rawData) {
	/**
	 * Number can be up to 9999. We start dividing each number by 10 up to
	 * four times to get each digit
	 */
	uint16_t currentNumber = number;
	for (uint8_t i = 0; i < 4; i++) {
		rawData[i] = currentNumber / (10 * *(3 - i));
		currentNumber = currentNumber - rawData[i] * 10 * *(3 - i);
	}

	for (uint8_t i = 0; i < 4; i++) {
		rawData[i] = tm1637NumberToDigit(rawData[i]);
	}
}

void tm1637WriteNumber(uint16_t number) {

	/**
	 * 1) Start Sequence
	 * 2) Write every byte
	 * 3) Read ACK
	 * 4) Stop sequence
	 *
	 * Source: https://controllerstech.com/interface-7-segment-display-with-stm32-tm1637/
	 */

	numberToRawData(number, currentSegment);

	sendStartSequence();
	readACK();
	writeRawData(currentSegment);
	sendStopSequence();

}

static void readACK() {
	/**
	 * 1) Put CLK LOW
	 * 2) Set DATA GPIO as INPUT and wait for it to be LOW
	 * 3) Wait 5 us
	 * 4) Put CLK HIGH
	 * 5) Wait 2 ms
	 * 6) Put CLK LOW
	 *
	 */

	HAL_GPIO_WritePin(GPIOA, clk_pin, GPIO_PIN_RESET);

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = data_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	delay_us(5);

	uint8_t maxWait = 0;

	do {
		if (maxWait > 10) {
			// ABORT, TIMEOUT, NO ACK!
			return;
		}
		maxWait++;
		delay_us(1);
	}
	while (HAL_GPIO_ReadPin(GPIOA, data_pin));

	HAL_GPIO_WritePin(GPIOA, clk_pin, GPIO_PIN_SET);

	delay_us(2);

	HAL_GPIO_WritePin(GPIOA, clk_pin, GPIO_PIN_RESET);

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = data_pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

}

static void sendStopSequence() {
	/**
	 * 1) Put CLK HIGH
	 * 2) Wait 2 us
	 * 3) Put DATA LOW
	 * 4) Wait 2 us
	 * 5) Put CLK HIGH
	 * 6) Wait 2 us
	 * 7) Put DATA HIGH
	 */

	HAL_GPIO_WritePin(GPIOA, clk_pin, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(GPIOA, data_pin, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(GPIOA, clk_pin, GPIO_PIN_SET);
	delay_us(2);
	HAL_GPIO_WritePin(GPIOA, data_pin, GPIO_PIN_SET);
}

static void writeRawData(uint8_t *rawData) {

	/**
	 * 1) Send Start Sequence
	 * 2) Send AUTO ADDRESS INCREMENT command
	 * 3) ReadACK
	 * 4) Write every byte of rawData and ReadACK
	 * 5) Send Stop Sequence
	 */

}

static void sendStartSequence() {
	/**
	 * 1) Put CLK HIGH
	 * 2) Put DATA HIGH
	 * 3) Wait 2 us
	 * 4) Put DATA LOW
	 */

	HAL_GPIO_WritePin(GPIOA, clk_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, data_pin, GPIO_PIN_SET);
	delay_us(2);
	HAL_GPIO_WritePin(GPIOA, data_pin, GPIO_PIN_RESET);

}

void tm1637Clear(void) {
	static const uint8_t rawData[] = { 0x00, 0x00, 0x00, 0x00 };

	writeRawData(*rawData);
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
