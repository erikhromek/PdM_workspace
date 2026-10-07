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
void dht11Init(uint16_t gpio, TIM_TypeDef tim);
static bool dht11Read(); // Realiza lectura
void getLatestRead(dht11Data_t *data); // Devuelve lectura del sensor



