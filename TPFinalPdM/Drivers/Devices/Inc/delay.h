#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t tick_t;
typedef bool bool_t;

/**
 * @brief Non-blocking delay
 */
typedef struct {
	tick_t startTime;
	tick_t duration;
	bool_t running;
} delay_t;

/**
 * @brief Initializes delay_t variable to be used as a non-blocking dleay
 *
 * @param delay   	Pointer to delay_t variable
 * @param duration 	Delay duration (miliseconds)
 *
 */
void delayInit(delay_t *delay, tick_t duration);


/**
 * @brief Reads the state of the delay and starts it if not running
 *
 * @param delay   	Pointer to delay_t variable
 *
 *	@return Returns true if delay finished and false if not.
 */
bool_t delayRead(delay_t *delay);

/**
 * @brief Modified the duration of the delay
 *
 * @param delay   	Pointer to delay_t variable
 * @param duration 	Delay duration (miliseconds)
 *
 */
void delayWrite(delay_t *delay, tick_t duration);

/**
 * @brief Indicates if delay is running
 *
 * @param delay   	Pointer to delay_t variable
 *
 *	@return Returns true if delay is still running and false if not.
 *
 */
bool_t delayIsRunning(delay_t *delay);

/**
 * @fn void delayUsInit(void)
 * @brief Initializes microseconds blocking delay using DWT
 * Source: https://deepbluembedded.com/stm32-delay-microsecond-millisecond-utility-dwt-delay-timer-delay/
 *
 */
void delayUsInit(void);

/**
 * @fn void delay_us(uint32_t us)
 *
 * @brief Waits for the specified microseconds. delayUsInit() must have been called before.
 *
 * @param duration	Delay duration (in microseconds)
 *
 */
void delay_us(uint32_t duration);

#endif /* API_INC_API_DELAY_H_ */
