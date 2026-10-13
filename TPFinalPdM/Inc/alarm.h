#ifndef ALARM_H_
#define ALARM_H_

#include <stm32f4xx_hal.h>
#include <stdint.h>

/**
 * @brief State of the alarm
 *
 */
typedef enum {
	ALARM_DISABLED, ALARM_CONFIGURED, ALARM_ON
} alarmState_t;

/**
 * @fn void alarmInit(GPIO_TypeDef*, uint16_t)
 * @brief Initializes alarm module. Default threshold is 35 °C.
 *
 * @param buzzerPort Pointer to GPIO port for buzzer
 * @param buzzerPin	GPIO number to use as buzzer
 */
void alarmInit(GPIO_TypeDef *buzzerPort, uint16_t buzzerPin);

/**
 * @fn void incrementAlarmThreshold()
 * @brief Increases alarm threshold in °C. Maximum value is 99.
 *
 */
void incrementAlarmThreshold();

/**
 * @fn void decrementAlarmThreshold()
 * @brief Decreases alarm threshold °C. Minimun value is 0.
 *
 */
void decrementAlarmThreshold();

/**
 * @fn uint8_t getAlarmThreshold()
 * @brief Returns current alarm threshold in °C.
 *
 * @pre
 * @post
 * @return	Returns current alarm threshold.
 */
uint8_t getAlarmThreshold();

/**
 * @fn void alarmToggle()
 * @brief Enables or disables the alarm.
 *
 */
void alarmToggle(void);

/**
 * @fn void alarmUpdate(uint8_t)
 * @brief Updates the alarm state using temperature value.
 *
 * @param temperature	temperature in °C.
 */
void alarmUpdate(uint8_t temperature); // Actualiza la MEF de la alarma

#endif /* ALARM_H_ */
