/**
 *
 * delay.c
 *
 *  Created on: 2026
 *      Author: Erik Hromek
 */

#include "delay.h"

void delayInit(delay_t *delay, tick_t duration) {
	delay->startTime = 0;
	delay->duration = duration;
	delay->running = false;
}

void delayUsInit(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL  |= DWT_CTRL_CYCCNTENA_Msk;
}

void delay_us(uint32_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks) { }
}

bool_t delayRead(delay_t *delay) {
	bool_t completed = false;
	tick_t currentTime = HAL_GetTick();

	if (delay == NULL)
		return completed;

	if (delay->running) {
		completed = (currentTime - delay->startTime >= delay->duration);
		if (completed)
			delay->running = false;
	} else {
		delay->startTime = currentTime;
		delay->running = true;
	}
	return completed;
}

void delayWrite(delay_t *delay, tick_t duration) {
	if (delay != NULL)
		delay->duration = duration;
}

bool_t delayIsRunning(delay_t *delay) {
	if (delay == NULL)
		return false;
	return delay->running;
}

