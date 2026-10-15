/*
 * tm1637.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */

#include "tm1637.h"
#include "tm1637_port.h"
#include "stddef.h"

/* Format of each digit: 0xXgfedcba */
static const uint8_t DIGITS[] = { 0b00111111, 0b00000110, 0b01011011,
		0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111,
		0b01101111 };
#define BLANK 0b00000000

// Command that configures TM1637 in AUTO ADDRESS MODE
#define AUTO_MODE 0x40
// TODO COULD BE AN ARRAY
#define FIRST_DIGIT_ADDRESS 0xC0
#define SECOND_DIGIT_ADDRESS 0xC1
#define THIRD_DIGIT_ADDRESS 0xC2
#define FOURTH_DIGIT_ADDRESS 0xC3

#define BRIGHTNESS_MASK 0x88 // 0b10001XXX Last 3 digits set BRIGHTNESS

static uint8_t currentSegment[] = { 0, 0, 0, 0 };

static void sendStartSequence() {
	/**
	 * 1) Put CLK HIGH
	 * 2) Put DATA HIGH
	 * 3) Wait 2 us
	 * 4) Put DATA LOW
	 */

	tm1637PortClkWrite(true);
	tm1637PortDataWrite(true);
	tm1637PortDelayUs(2);
	tm1637PortDataWrite(false);

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

	tm1637PortClkWrite(false);
	tm1637PortDelayUs(2);
	tm1637PortDataWrite(false);
	tm1637PortDelayUs(2);
	tm1637PortClkWrite(true);
	tm1637PortDelayUs(2);
	tm1637PortDataWrite(true);
}

static bool readACK() {
	/**
	 * 1) Put CLK LOW
	 * 2) Set DATA GPIO as INPUT and wait for it to be LOW
	 * 3) Wait 5 us
	 * 4) Put CLK HIGH
	 * 5) Wait 2 ms
	 * 6) Put CLK LOW
	 *
	 */

	bool error = false;

	tm1637PortClkWrite(false);
	tm1637PortDataInput();

	tm1637PortDelayUs(5);

	uint8_t maxWait = 0;

	do {
		if (maxWait > 10) {
			// ABORT, TIMEOUT, NO ACK!
			error = true;
			break;
		}
		maxWait++;
		tm1637PortDelayUs(1);
	} while (tm1637PortDataRead());

	tm1637PortClkWrite(true);
	tm1637PortDelayUs(3);
	tm1637PortClkWrite(false);
	tm1637PortDelayUs(3);

	tm1637PortDataOutput();

	return !error;

}

static void writeByte(uint8_t byte) {
	/**
	 * 1) We put CLK on LOW. Data must change while CLK is LOW
	 * 2) Wait 3 us
	 * 3) For each bit of the byte, we put DATA HIGH or LOW
	 * 4) We put CLK on HIGH
	 * 5) Wait 3 us
	 */
	for (uint8_t i = 0; i < 8; i++) {
		tm1637PortClkWrite(false);
		tm1637PortDelayUs(3);
		if (byte & 0x01) {
			tm1637PortDataWrite(true);
		} else
			tm1637PortDataWrite(false);
		tm1637PortClkWrite(true);
		tm1637PortDelayUs(3);
		byte = byte >> 1; // Shift next bit to the right
	}
}

static bool writeRawData(uint8_t *rawData) {
	if (rawData == NULL)
		return false;

	bool error = false;

	/**
	 * 1) Send Start Sequence
	 * 2) Send AUTO ADDRESS INCREMENT command
	 * 3) ReadACK
	 * 4) Write every byte of rawData and ReadACK
	 * 5) Send Stop Sequence
	 */

	sendStartSequence();
	writeByte(AUTO_MODE);
	if (!readACK()) {
		error = true;
	}
	sendStopSequence();
	sendStartSequence();
	writeByte(FIRST_DIGIT_ADDRESS);
	if (!readACK()) {
		error = true;
	}
	for (uint8_t i = 0; i < 4; i++) {
		writeByte(rawData[i]);
		if (!readACK()) {
			error = true;
		}
	}
	sendStopSequence();
	return !error;

}

void tm1637Init(void) {
	tm1637PortInit();
}

bool tm1637SetBrightness(uint8_t level) {
	uint8_t value = level;
	if (value > 7)
		value = 7;
	sendStartSequence();
	writeByte(value | BRIGHTNESS_MASK);
	if (!readACK()) {
		sendStopSequence();
		return false;
	}
	sendStopSequence();
	return true;

}

static void numberToRawData(uint16_t number, uint8_t *rawData) {
	/**
	 * Number can be up to 9999. We start dividing each number by 10 up to
	 * four times to get each digit
	 */
	static const uint16_t divisors[4] = { 1000, 100, 10, 1 };

	if (rawData == NULL)
		return;
	if (number > 9999)
		number = 9999;

	for (uint8_t i = 0; i < 4; i++) {
		rawData[i] = DIGITS[(number / divisors[i]) % 10];
	}
}

bool tm1637WriteNumber(uint16_t number) {

	/**
	 * 1) Start Sequence
	 * 2) Write every byte
	 * 3) Read ACK
	 * 4) Stop sequence
	 *
	 * Source: https://controllerstech.com/interface-7-segment-display-with-stm32-tm1637/
	 */

	numberToRawData(number, currentSegment);

	return writeRawData(currentSegment);

}

bool tm1637Clear(void) {
	static uint8_t rawData[] = { 0x00, 0x00, 0x00, 0x00 };

	return writeRawData(rawData);
}

