#ifndef TM1637_H_
#define TM1637_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

bool tm1637Init(GPIO_TypeDef *clkPort, uint16_t clkPin, GPIO_TypeDef *dataPort,
		uint16_t dataPin); // Inicializa los pines
bool tm1637SetBrightness(uint8_t level); // Asigna un nivel de brillo
bool tm1637WriteNumber(uint16_t number); // Escribe el valor que corresponde en cada posición
bool tm1637Clear(void); // Limpia todos los dígitos

#endif /* TM1637_H_ */
