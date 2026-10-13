#ifndef APP_BUTTON_H_
#define APP_BUTTON_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/**
 * @brief Events of buttons pressed. BTN_NOEVENT represents no button pressed.
 *
 */
typedef enum {
	BTN_ALARM, BTN_UP, BTN_DOWN, BTN_CONFIG, BTN_NOEVENT
} btnEvent_t;

/**
 * @fn bool buttonPressed(void)
 * @brief Returns whether a button has been pressed.
 *
 * @return True if a button has been pressed.
 */
bool buttonPressed(void); // Devuelve si se presionó un botón

/**
 * @fn btnEvent_t getEventButton(void)
 * @brief Returns the latest button pressed. Should be used after buttonPressed() function.
 *
 * @pre
 * @post The latest button pressed event is removed from the internal state if it is not equal to BTN_NOEVENT.
 * @return	btnEvent_t representing the latest button pressed event.
 */
btnEvent_t getEventButton(void);

/**
 * @fn void appButtonsInit(GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t)
 * @brief Configures GPIO for buttons and starts internal state.
 *
 * @pre
 * @post
 * @param upPort	Pointer to GPIO port for UP button
 * @param upPin		GPIO number to use as UP button
 * @param downPort	Pointer to GPIO port for DOWN button
 * @param downPin	GPIO number to use as DOWN button
 * @param alarmPort	Pointer to GPIO port for ALARM button
 * @param alarmPin	GPIO number to use as ALARM button
 * @param cfgPort	Pointer to GPIO port for CONFIG button
 * @param cfgPin	GPIO number to use as CONFIG button
 */
void appButtonsInit(GPIO_TypeDef *upPort, uint16_t upPin,
		GPIO_TypeDef *downPort, uint16_t downPin, GPIO_TypeDef *alarmPort,
		uint16_t alarmPin, GPIO_TypeDef *cfgPort, uint16_t cfgPin); // Inicializa GPIO de botones

/**
 * @fn void buttonsStateUpdate(void)
 * @brief Updates the alarm state using temperature value.
 *
 */
void buttonsStateUpdate(void); // Actualiza la FSM de los botones

#endif /* APP_BUTTON_H_ */
