/*
 * alarm.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */
#include "alarm.h"

static uint8_t alarmThreshold;
static alarmState_t alarmState;
static GPIO_TypeDef *buzzer_port;
static uint16_t buzzer_pin;

static void enableBuzzer() {
	HAL_GPIO_WritePin(buzzer_port, buzzer_pin, GPIO_PIN_SET);
}
static void disableBuzzer() {
	HAL_GPIO_WritePin(buzzer_port, buzzer_pin, GPIO_PIN_RESET);
}

void alarmInit(GPIO_TypeDef *buzzerPort, uint16_t buzzerPin) {

	buzzer_port = dataPort;
	buzzer_pin = buzzerPin;

	if (buzzerPort == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if (buzzerPort == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	else
		__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef gpio = { 0 };
	gpio.Mode = GPIO_MODE_OUTPUT_OD;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	gpio.Pin = buzzer_pin;
	HAL_GPIO_Init(buzzerPort, &gpio);

	HAL_GPIO_WritePin(buzzer_port, buzzer_pin, GPIO_PIN_RESET);

	alarmThreshold = 35;
	alarmState = ALARM_DISABLED;
}
void incrementAlarmThreshold() {
	alarmThreshold++;
	if (alarmThreshold > 99)
		alarmThreshold = 0;
}
void decrementAlarmThreshold() {
	alarmThreshold--;
	if (alarmThreshold < 0)
		alarmThreshold = 0;
}
uint8_t getAlarmThreshold() {
	return alarmThreshold;
}
void alarmToggle() {
	if (alarmState == ALARM_CONFIGURED || alarmState == ALARM_ON)
		alarmState = ALARM_DISABLED;
	else if (alarmState == ALARM_DISABLED)
		ALARM_CONFIGURED;

}
// Actualiza la MEF de la alarma
void alarmUpdate(uint8_t temperature) {
	switch (alarmState) {
	case ALARM_DISABLED:
		break;
	case ALARM_CONFIGURED:
		if (alarmThreshold > temperature) {
			enableBuzzer();
			alarmState = ALARM_ON;
		} else
			disableBuzzer();
		break;
	case ALARM_ON:
		if (alarmThreshold <= temperature) {
			disableBuzzer();
			alarmState = ALARM_CONFIGURED;
		} else
			enableBuzzer();
		break;
	default:
		break;
	}
}
