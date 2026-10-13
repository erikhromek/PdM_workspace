/*
 * app_button.c
 *
 *  Created on: Oct 2026
 *      Author: Erik Hromek
 */

#include "app_button.h"
#include "delay.h"

#define DEBOUNCE_MS 40

static delay_t delay;
static btnEvent_t btnEventPressed;

/**
 * @brief Estados de la FSM
 */
typedef enum {
	BUTTON_UP, BUTTON_FALLING, BUTTON_DOWN, BUTTON_RISING,
} debounceState_t;

static debounceState_t FSMcurrentState;
static btnEvent_t currentBtn;

static GPIO_TypeDef *up_port;
static uint16_t up_pin;

static GPIO_TypeDef *down_port;
static uint16_t down_pin;

static GPIO_TypeDef *alarm_port;
static uint16_t alarm_pin;

static GPIO_TypeDef *cfg_port;
static uint16_t cfg_pin;

void appButtonsInit(GPIO_TypeDef *upPort, uint16_t upPin,
		GPIO_TypeDef *downPort, uint16_t downPin, GPIO_TypeDef *alarmPort,
		uint16_t alarmPin, GPIO_TypeDef *cfgPort, uint16_t cfgPin) {
	/*
	 * Presupone que el botón siempre arranca en UP, no presionado
	 */

	up_port = upPort;
	up_pin = upPin;

	down_port = downPort;
	down_pin = downPin;

	alarm_port = alarmPort;
	alarm_pin = alarmPin;

	cfg_port = cfgPort;
	cfg_pin = cfgPin;

	if (upPort == GPIOA || downPort == GPIOA || alarmPort == GPIOA
			|| cfgPort == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	if (upPort == GPIOB || downPort == GPIOB || alarmPort == GPIOB
			|| cfgPort == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	if (upPort == GPIOC || downPort == GPIOC || alarmPort == GPIOC
			|| cfgPort == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	if (upPort == GPIOH || downPort == GPIOH || alarmPort == GPIOH
			|| cfgPort == GPIOH)
		__HAL_RCC_GPIOH_CLK_ENABLE();

	GPIO_InitTypeDef gpio = { 0 };
	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	gpio.Pin = up_pin;
	HAL_GPIO_Init(up_port, &gpio);

	gpio.Pin = down_pin;
	HAL_GPIO_Init(down_port, &gpio);

	gpio.Pin = alarm_pin;
	HAL_GPIO_Init(alarm_port, &gpio);

	gpio.Pin = cfg_pin;
	HAL_GPIO_Init(cfg_port, &gpio);

	FSMcurrentState = BUTTON_UP;
	delayInit(&delay, DEBOUNCE_MS);
	currentBtn = BTN_NOEVENT;
	btnEventPressed = BTN_NOEVENT;
}

bool buttonPressed(void) {
	return (btnEventPressed != BTN_NOEVENT);

}

btnEvent_t getEventButton(void) {
	btnEvent_t btnToReturn = btnEventPressed;
	if (btnToReturn != BTN_NOEVENT)
		btnEventPressed = BTN_NOEVENT;
	return btnToReturn;
}

static GPIO_PinState readCurrentBtnState(void) {
	switch (currentBtn) {
	case BTN_NOEVENT:
		return GPIO_PIN_SET;
	case BTN_UP:
		return HAL_GPIO_ReadPin(up_port, up_pin);
	case BTN_DOWN:
		return HAL_GPIO_ReadPin(down_port, down_pin);
	case BTN_ALARM:
		return HAL_GPIO_ReadPin(alarm_port, alarm_pin);
	case BTN_CONFIG:
		return HAL_GPIO_ReadPin(cfg_port, cfg_pin);
	default:
		break;
	}

	return GPIO_PIN_RESET;
}

void buttonsStateUpdate(void) {
	/*
	 * Determina el estado de la FSM al estado actual y al estado de los botones
	 */
	switch (FSMcurrentState) {

	case BUTTON_UP:
		if (HAL_GPIO_ReadPin(up_port, up_pin) == GPIO_PIN_RESET) {
			delayRead(&delay);
			FSMcurrentState = BUTTON_FALLING;
			currentBtn = BTN_UP;
		} else if (HAL_GPIO_ReadPin(down_port, down_pin) == GPIO_PIN_RESET) {
			delayRead(&delay);
			FSMcurrentState = BUTTON_FALLING;
			currentBtn = BTN_DOWN;
		} else if (HAL_GPIO_ReadPin(alarm_port, alarm_pin) == GPIO_PIN_RESET) {
			delayRead(&delay);
			FSMcurrentState = BUTTON_FALLING;
			currentBtn = BTN_ALARM;
		} else if (HAL_GPIO_ReadPin(cfg_port, cfg_pin) == GPIO_PIN_RESET) {
			delayRead(&delay);
			FSMcurrentState = BUTTON_FALLING;
			currentBtn = BTN_CONFIG;
		}
		break;
	case BUTTON_FALLING:
		if (delayRead(&delay)) {
			if (readCurrentBtnState() == GPIO_PIN_RESET) {
				btnEventPressed = currentBtn;
				FSMcurrentState = BUTTON_DOWN;
			} else {
				FSMcurrentState = BUTTON_UP;
				currentBtn = BTN_NOEVENT;
			}
		}
		break;
	case BUTTON_DOWN:
		if (readCurrentBtnState() == GPIO_PIN_SET) {
			delayRead(&delay);
			FSMcurrentState = BUTTON_RISING;
		}
		break;
	case BUTTON_RISING:
		if (delayRead(&delay)) {
			if (readCurrentBtnState() == GPIO_PIN_SET) {
				currentBtn = BTN_NOEVENT;
				FSMcurrentState = BUTTON_UP;
			}
			else
				FSMcurrentState = BUTTON_DOWN;
		}
		break;
	default:
		break;
	}
}

