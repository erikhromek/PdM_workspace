/*
 * tm1637.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */

#include "tm1637.h"
#include "delay.h"

static TIM_HandleTypeDef htim;
static GPIO_TypeDef *clk_port;
static uint16_t      clk_pin;
static GPIO_TypeDef *data_port;
static uint16_t      data_pin;

/* 0xXgfedcba */
// TODO COULD BE AN ARRAY
static const uint8_t DIGITS[] = { 0b00111111, 0b00000110, 0b01011011,
		0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111,
		0b01101111 };

#define BLANK 0b00000000

#define AUTO_MODE 0x40
// TODO COULD BE AN ARRAY
#define FIRST_DIGIT_ADDRESS 0xC0
#define SECOND_DIGIT_ADDRESS 0xC1
#define THIRD_DIGIT_ADDRESS 0xC2
#define FOURTH_DIGIT_ADDRESS 0xC3

#define BRIGHTNESS_COMMAND 0x8F // 0b10001XXX Last 3 digits set BRIGHTNESS

static uint8_t currentSegment[] = { 0, 0, 0, 0 };

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

	HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_RESET);

	// This frees DATA DPIO
	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);

	delay_us(5);

	uint8_t maxWait = 0;

	do {
		if (maxWait > 10) {
			// ABORT, TIMEOUT, NO ACK!
			return;
		}
		maxWait++;
		delay_us(1);
	} while (HAL_GPIO_ReadPin(data_port, data_pin));

	HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_RESET);

	delay_us(2);


}
static void sendStartSequence() {
	/**
	 * 1) Put CLK HIGH
	 * 2) Put DATA HIGH
	 * 3) Wait 2 us
	 * 4) Put DATA LOW
	 */

	HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);
	delay_us(2);
	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_RESET);

}

static void sendStopSequence() {
	/**
	 * 1) Put CLK LOW
	 * 2) Wait 2 us
	 * 3) Put DATA LOW
	 * 4) Wait 2 us
	 * 5) Put CLK HIGH
	 * 6) Wait 2 us
	 * 7) Put DATA HIGH
	 */

	HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_SET);
	delay_us(2);
	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);
}

static void writeByte(uint8_t byte) {
	/**
	 *
	 */
	for (uint8_t i = 0; i < 8; i++) {
		HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_RESET);
		delay_us(3);
		if (byte & 0x01) {
			HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);
		} else
			HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_SET);
		delay_us(3);
		byte = byte >> 1; // Shift next bit to the right
	}
}


static void writeRawData(uint8_t *rawData) {

	/**
	 * 1) Send Start Sequence
	 * 2) Send AUTO ADDRESS INCREMENT command
	 * 3) ReadACK
	 * 4) Write every byte of rawData and ReadACK
	 * 5) Send Stop Sequence
	 */

	sendStartSequence();
	writeByte(AUTO_MODE);
	readACK();
	sendStopSequence();
	sendStartSequence();
	writeByte(FIRST_DIGIT_ADDRESS);
	readACK();
	for (uint8_t i = 0; i < 4; i++) {
		writeByte(rawData[i]);
		readACK();
	}
	sendStopSequence();

}


bool tm1637Init(GPIO_TypeDef *clkPort, uint16_t clkPin, GPIO_TypeDef *dataPort,
		uint16_t dataPin) {
	bool result = false;

	delayUsInit();

	clk_port = clkPort;
	clk_pin = clkPin;
	data_port = dataPort;
	data_pin = dataPin;

	if (dataPort == GPIOA || clkPort == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	if (dataPort == GPIOB || clkPort == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	if (dataPort == GPIOC || clkPort == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();

	// Configure DATA and CLK GPIO AS OUTPUT OPEN DRAIN

	GPIO_InitTypeDef gpio = {0};
	gpio.Mode  = GPIO_MODE_OUTPUT_OD;
	gpio.Pull  = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	gpio.Pin = clk_pin;
	HAL_GPIO_Init(clk_port, &gpio);
	gpio.Pin = data_pin;
	HAL_GPIO_Init(data_port, &gpio);

	HAL_GPIO_WritePin(data_port, data_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_SET);

	result = true; // TODO: check if GPIO or DWT can fail


	return result;
}

void tm1637SetBrightness(uint8_t level) {
	// TODO: allow to receive values between 0 and 7 (111)
	sendStartSequence();
	writeByte(BRIGHTNESS_COMMAND);
	readACK();
	sendStopSequence();

}

static void numberToRawData(uint16_t number, uint8_t *rawData) {
	/**
	 * Number can be up to 9999. We start dividing each number by 10 up to
	 * four times to get each digit
	 */
	static const uint16_t divisors[4] = {1000, 100, 10, 1};

	if (rawData == NULL)
		return;
	if (number > 9999)
		number = 9999;

	for (uint8_t i = 0; i < 4; i++) {
		rawData[i] = DIGITS[(number / divisors[i]) % 10];
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

	writeRawData(currentSegment);

}


void tm1637Clear(void) {
	static const uint8_t rawData[] = { 0x00, 0x00, 0x00, 0x00 };

	writeRawData(rawData);
}


