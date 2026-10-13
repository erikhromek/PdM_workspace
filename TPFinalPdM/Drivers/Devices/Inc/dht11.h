#ifndef DHT11_H_
#define DHT11_H_

#include <stdbool.h>
#include "stm32f4xx_hal.h"

/**
 * @brief DHT11 data structure
 *
 */
typedef struct {
	uint8_t temperature;
	uint8_t humidity;
} dht11Data_t;

/**
 * @fn void dht11Init()
 * @brief Initializes DHT11 on specified port
 *
 * @param dataPort	Pointer to GPIO port for DIO
 * @param dataPin	GPIO number to use as DIO
 * @return Returns true if there have not happened errors in the configuration.
 */

bool dht11Init(GPIO_TypeDef *dataPort, uint16_t dataPin);

/**
 * @fn bool dht11Read(void)
 * @brief Starts a blocking reading on the DHT11 sensor. Takes between 20 and 25 miliseconds.
 *
 * @return Returns true if the reading is valid.
 */
bool dht11Read(void);

/**
 * @fn void dht11LatestRead(dht11Data_t*)
 * @brief Copies the latest valid reading from the DHT11 sensor
 *
 * @param data	Pointer to dht11Data_t variable.
 */
void dht11LatestRead(dht11Data_t *data);

#endif /* DHT11_H_ */
