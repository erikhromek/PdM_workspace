#include "API_debounce.h"

#define DEBOUNCE_MS 40

void buttonPressed();

static delay_t delay;
static bool_t keyPressed;

/**
 * @brief Estados de la FSM
 */
typedef enum {
	BUTTON_UP, BUTTON_FALLING, BUTTON_DOWN, BUTTON_RISING,
} debounceState_t;

static debounceState_t FSMcurrentState;


void debounceFSM_init() {
	/*
	 * Presupone que el botón siempre arranca en UP, no presionado
	 */
	FSMcurrentState = BUTTON_UP;
	delayInit(&delay, DEBOUNCE_MS);
	keyPressed = false;
}

bool_t readKey() {
	bool_t status = keyPressed;
	keyPressed = false;
	return status;
}

void debounceFSM_update() {
	/*
	 * Determina el estado de la FSM al estado actual y al estado del botón
	 */
	switch (FSMcurrentState) {
	case BUTTON_UP:
		if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET) {
			delayRead(&delay);
			FSMcurrentState = BUTTON_FALLING;
		}
		break;
	case BUTTON_FALLING:
		if (delayRead(&delay)) {
			if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET) {
				keyPressed = true;
				FSMcurrentState = BUTTON_DOWN;
			} else
				FSMcurrentState = BUTTON_UP;
		}
		break;
	case BUTTON_DOWN:
		if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET) {
			delayRead(&delay);
			FSMcurrentState = BUTTON_RISING;
		}
		break;
	case BUTTON_RISING:
		if (delayRead(&delay)) {
			if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET)
				FSMcurrentState = BUTTON_UP;
			 else
				FSMcurrentState = BUTTON_DOWN;
		}
		break;
	default:
		debounceFSM_init();
		break;
	}
}
