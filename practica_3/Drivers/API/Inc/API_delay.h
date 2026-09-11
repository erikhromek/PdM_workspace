#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "stdbool.h"

typedef uint32_t tick_t;
typedef bool bool_t;

/**
 * @brief Delay no bloqueante
 */
typedef struct {
	tick_t startTime;
	tick_t duration;
	bool_t running;
} delay_t;

/**
 * @brief Inicializa una variable delay_t para usar como timer no bloqueante
 *
 * @param delay   	Puntero a variable del tipo delay_t
 * @param duration 	Duración inicial del delay (en milisegundos)
 *
 */
void delayInit(delay_t *delay, tick_t duration);


/**
 * @brief Lee el estado del delay y lo inicializa si
 *
 * @param delay   	Puntero a variable del tipo delay_t
 *
 *	@return Devuelve true si el delay terminó y false si no.
 *	Si no inició todavía, devuelve false también.
 *	Si el delay no está inicializado, devuelve false.
 */
bool_t delayRead(delay_t *delay);

/**
 * @brief Modifica el valor de la duración del delay
 *
 * @param delay   	Puntero a variable del tipo delay_t
 * @param duration 	Duración del delay (en milisegundos)
 *
 */
void delayWrite(delay_t *delay, tick_t duration);

/**
 * @brief Indica si un delay está corriendo
 *
 * @param delay   	Puntero a variable del tipo delay_t
 *
 *	@return Devuelve true si el delay todavía está corriendo y false en caso
 *	contrario.
 *	Si el delay no está inicializado, devuelve false.
 */
bool_t delayIsRunning(delay_t *delay);

#endif /* API_INC_API_DELAY_H_ */
