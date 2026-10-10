typedef enum {
	BTN_ALARM, BTN_UP, BTN_DOWN, BTN_CONFIG, BTN_NOEVENT
} btnEvent_t; // Eventos de botón
btnEvent_t readBtn(); // Devuelve si se presionó un botón
buttonsInit(); // Inicializa GPIO de botones
void buttonsUpdate(); // Actualiza la FSM de los botones
