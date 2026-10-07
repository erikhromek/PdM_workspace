

bool tm1637Init(); // Inicializa los pines
void tm1637SetBrightness(uint8_t level); // Asigna un nivel de brillo
void tm1637WriteSegments(const uint8_t seg[4]); // Escribe el valor que corresponde en cada posición
void tm1637Clear(void); // Limpia todos los dígitos
