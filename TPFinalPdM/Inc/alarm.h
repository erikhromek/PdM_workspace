#ifndef ALARM_H_
#define ALARM_H_

#include <stm32f4xx_hal.h>
#include <stdint.h>

typedef enum {
	ALARM_DISABLED, ALARM_CONFIGURED, ALARM_ON
} alarmState_t;

void alarmInit(GPIO_TypeDef *buzzerPort, uint16_t buzzerPin);
void incrementAlarmThreshold();
void decrementAlarmThreshold();
uint8_t getAlarmThreshold();
void alarmToggle(); // Habilita / Deshabilita la alarma
void alarmUpdate(uint8_t temperature); // Actualiza la MEF de la alarma

#endif /* ALARM_H_ */
