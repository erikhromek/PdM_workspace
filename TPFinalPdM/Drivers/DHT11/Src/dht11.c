/**
 * dht11.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */

#include <stdbool.h>
#include "dht11.h"
#include "dht11_port.h"
#include <stddef.h>

static dht11Data_t latestRead = { 0, 0 };

void dht11Init(void) {

	dht11PortInit();

}

static bool validateChecksum(uint8_t *dataRaw) {
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

	dht11PortDataLow();

	dht11PortDelayMs(20);

	dht11PortCriticalEnter();

	dht11PortDataHigh();


	/**
	 * Read start signal from DHT11:
	 * 0) Wait the line to go from HIGH to LOW; wait 120 us
	 * 1) Read LOW for 80-120 us
	 * 2) Read HIGH for 80-120 us
	 *
	 * 80 us did not work okay so we add 50% to the response window (120 us).
	 */

	uint32_t elapsedUs = dht11PortWaitWhile(true, 120);
	if (elapsedUs == -1) {
		dht11PortCriticalExit();
		return false;
	}

	elapsedUs = dht11PortWaitWhile(false, 120);
	if (elapsedUs == -1) {
		dht11PortCriticalExit();
		return false;
	}

	elapsedUs = dht11PortWaitWhile(true, 120);
	if (elapsedUs == -1) {
		dht11PortCriticalExit();
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
	 * Response window is too thin so we increase it.
	 *
	 */

	// 0 = humi int, 1 = humi dec, 2 = temp int, 3 = temp dec, 4 = checksum
	uint8_t dataRaw[] = { 0, 0, 0, 0, 0 };
	uint8_t currentByte = 0;

	int8_t bitPosition = 7;

	for (uint8_t i = 0; i < 40; i++) {
		elapsedUs = dht11PortWaitWhile(false, 100);
		if (elapsedUs == -1) {
			dht11PortCriticalExit();
			return false;
		}

		elapsedUs = dht11PortWaitWhile(true, 120);
		if (elapsedUs == -1) {
			dht11PortCriticalExit();
			return false;
		}

		// Determinate if bit is 0 or 1 using cycles (47 that is between 26 and 70 us).
		uint8_t bit = (elapsedUs > 47) ? 1 : 0;
		currentByte = (i / 8);
		dataRaw[currentByte] |= (bit << bitPosition);

		bitPosition--;
		if (bitPosition < 0)
			bitPosition = 7;
	}

	dht11PortCriticalExit();

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

