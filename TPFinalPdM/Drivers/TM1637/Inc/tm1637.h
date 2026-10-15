#ifndef TM1637_H_
#define TM1637_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @fn void tm1637Init()
 * @brief Configures and initializes TM1637 display
 *
 */
void tm1637Init(void);
/**
 * @fn bool tm1637SetBrightness(uint8_t)
 * @brief Sets the brightness of the TM1637 module.
 *
 * @param level	Number between 0 and 7. If number is greater than 7, it becomes 7.
 * @return	Returns true if the display acknowledged the command.
 */
bool tm1637SetBrightness(uint8_t level);

/**
 * @fn bool tm1637WriteNumber(uint16_t)
 * @brief Writes a four digit max number.
 *
 * @param number	Number to write. If number is greater than 9999, it becomes 9999.
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
