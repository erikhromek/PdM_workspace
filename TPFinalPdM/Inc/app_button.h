#ifndef APP_BUTTON_H_
#define APP_BUTTON_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

typedef enum {
	BTN_ALARM, BTN_UP, BTN_DOWN, BTN_CONFIG, BTN_NOEVENT
} btnEvent_t; // Eventos de botón
bool buttonPressed(void); // Devuelve si se presionó un botón
btnEvent_t getEventButton(void);
void appButtonsInit(GPIO_TypeDef *upPort, uint16_t upPin, GPIO_TypeDef *downPort,
		uint16_t downPin, GPIO_TypeDef *alarmPort, uint16_t alarmPin,
		GPIO_TypeDef *cfgPort, uint16_t cfgPin); // Inicializa GPIO de botones
void buttonsStateUpdate(void); // Actualiza la FSM de los botones

#endif /* APP_BUTTON_H_ */
