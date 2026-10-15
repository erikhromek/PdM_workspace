/*
 * tm1637_port.c
 *
 *  Created on: Octubre 2026
 *      Author: Erik Hromek
 */

#include "tm1637_port.h"
#include "stm32f4xx_hal.h"
#include "delay.h"

static GPIO_TypeDef *clk_port;
static uint16_t clk_pin;
static GPIO_TypeDef *data_port;
static uint16_t data_pin;

static void configurePinAsOutput(GPIO_TypeDef *port, uint16_t pin) {
	GPIO_InitTypeDef g = { 0 };
	g.Pin = pin;
	g.Mode = GPIO_MODE_OUTPUT_PP;
	g.Pull = GPIO_NOPULL;
	g.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(port, &g);
}

static void configurePinAsInput(GPIO_TypeDef *port, uint16_t pin) {
	GPIO_InitTypeDef g = { 0 };
	g.Pin = pin;
	g.Mode = GPIO_MODE_INPUT;
	g.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(port, &g);
}

static void pinWrite(GPIO_TypeDef *port, uint16_t pin, bool value) {
	HAL_GPIO_WritePin(port, pin, value);
}

void tm1637PortClkWrite(bool level) {
	pinWrite(clk_port, clk_pin, level);
}
void tm1637PortDataWrite(bool level) {
	pinWrite(data_port, data_pin, level);
}

void tm1637PortInit(void) {
	delayUsInit();

	clk_port = GPIOA;
	clk_pin = GPIO_PIN_6;
	data_port = GPIOA;
	data_pin = GPIO_PIN_9;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure DATA and CLK GPIO AS OUTPUT

	configurePinAsOutput(clk_port, clk_pin);
	configurePinAsOutput(data_port, data_pin);

	tm1637PortClkWrite(true);
	tm1637PortDataWrite(true);

}
bool tm1637PortDataRead(void) {
	return HAL_GPIO_ReadPin(data_port, data_pin);

}
void tm1637PortDataInput(void) {
	configurePinAsInput(data_port, data_pin);
}
void tm1637PortDataOutput(void) {
	configurePinAsOutput(data_port, data_pin);
}

void tm1637PortDelayUs(uint32_t us) {
	delay_us(us);
}


