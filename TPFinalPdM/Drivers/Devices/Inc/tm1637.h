#ifndef TM1637_H_
#define TM1637_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

/**
 * @fn bool tm1637Init(GPIO_TypeDef*, uint16_t, GPIO_TypeDef*, uint16_t)
 * @brief
 *
 * @pre
 * @post
 * @param clkPort	Pointer to GPIO port for CLK
 * @param clkPin	GPIO number to use as CLK
 * @param dataPort	Pointer to GPIO port for DIO
 * @param dataPin	GPIO number to use as DIO
 * @return	Returns true if there have not happened errors in the configuration.
 */
bool tm1637Init(GPIO_TypeDef *clkPort, uint16_t clkPin, GPIO_TypeDef *dataPort,
		uint16_t dataPin);
/**
 * @fn bool tm1637SetBrightness(uint8_t)
 * @brief Sets the brightness of the TM1637 module.
 *
 * @param level	Number between 0 and 7.
 * @return	Returns true if the display acknowledged the command.
 */
bool tm1637SetBrightness(uint8_t level);

/**
 * @fn bool tm1637WriteNumber(uint16_t)
 * @brief Writes a four digit max number.
 *
 * @param number	Number to write.
 * @return	Returns true if the display acknowledged the command.
 */
bool tm1637WriteNumber(uint16_t number);

/**
 * @fn bool tm1637Clear(void)
 * @brief Clears the screen of the TM1637 module.
 *
 * @return	Returns true if the display acknowledged the command.
 */
bool tm1637Clear(void);

#endif /* TM1637_H_ */
