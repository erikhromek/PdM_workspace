#include "API_delay.h"
#include "stm32f4xx_hal.h"

#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC

/**
 * @brief Inicializa la máquina de estados
 *
 *	@retval None
 *
 */
void debounceFSM_init();

/**
 * @fn bool_t readKey()
 * @brief Indica si el botón B1 fue apretado
 * desde la última vez que se llamó a la función
 *
 * @pre La máquina de estados debe estar inicializada
 * @retval Devuelve true si fue apretado o false caso contrario
 */
bool_t readKey();

/**
 * @brief Actualiza la máquina de estados
 * @pre La máquina de estados debe estar inicializada
 *	@retval None
 *
 */
void debounceFSM_update();
