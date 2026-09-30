#include "API_delay.h"

void delayInit(delay_t *delay, tick_t duration) {
	delay->startTime = 0;
	delay->duration = duration;
	delay->running = false;
}

bool_t delayRead(delay_t *delay) {
	if (delay == NULL)
		return NULL;
	bool_t completed = false;
	tick_t currentTime = HAL_GetTick();

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
		return NULL;
	return delay->running;
}
