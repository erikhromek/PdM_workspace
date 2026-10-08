#ifndef DHT11_H_
#define DHT11_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"

typedef struct {
   uint8_t temperature;
   uint8_t humidity;
} dht11Data_t; // Estructura de datos del DHT11


/**
 * @fn void dht11Init()
 * @brief Initializes DHT11 on GPIO
 *
 * @pre
 * @post
 */
bool dht11Init(GPIO_TypeDef *dataPort, uint16_t dataPin);
bool dht11Read(void); // Realiza lectura
void getLatestRead(dht11Data_t *data); // Devuelve lectura del sensor


#endif /* DHT11_H_ */
