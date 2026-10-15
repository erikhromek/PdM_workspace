#ifndef DHT11_H_
#define DHT11_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief DHT11 data structure
 *
 */
typedef struct {
	uint8_t temperature;
	uint8_t humidity;
} dht11Data_t;

/**
 * @fn void dht11Init(void)
 * @brief Configure and initialize DHT11.
 *
 */
void dht11Init(void);

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
