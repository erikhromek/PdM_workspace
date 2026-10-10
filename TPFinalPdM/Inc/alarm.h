#ifndef ALARM_H_
#define ALARM_H_

typedef enum {
	ALARM_DISABLED, ALARM_CONFIGURED, ALARM_ON
} alarmState_t;

void alarmInit(GPIO_TypeDef *buzzerPort, uint16_t buzzerPin);
void incrementAlarmThreshold();
void decrementAlarmThreshold();
uint8_t getAlarmThreshold();
void alarmToggle(); // Habilita / Deshabilita la alarma
void alarmUpdate(); // Actualiza la MEF de la alarma

#endif /* ALARM_H_ */
