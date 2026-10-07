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
#define ZERO 0x00111111
#define ONE 0x00000110
#define TWO 0x01011011
#define THREE 0x01001111
#define FOUR 0x01100110
#define FIVE 0x01101101
#define SIX 0x01111100
#define SEVEN 0x00000111
#define EIGHT 0x01111111
#define NINE 0x01100111

void tm1637Init(uint16_t data_gpio, uint16_t clk_gpio, TIM_TypeDef tim) {
	bool result = false;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	/**
	 * Configure GPIO as OUTPUT
	*/

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, pin, GPIO_PIN_RESET);

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
		data_pin = data_gpio;
		clk_pin = clk_gpio;
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

}

static uint8_t* numberToRawData(uint16_t number)
{
	/**
	 * Number can be up to 9999. We start dividing each number by 10 up to
	 * four times to get each digit
	*/
	uint8_t rawData[] = { 0, 0, 0, 0};
}

void tm1637WriteSegments(uint16_t number, bool enableDots) {

	/**
	 * 1) Start Sequence
	 * 2) Write every byte
	 * 3) Read ACK
	 * 4) Stop sequence
	 *
	 * Source: https://controllerstech.com/interface-7-segment-display-with-stm32-tm1637/
	 */


}

void tm1637Clear(void)() {

}
