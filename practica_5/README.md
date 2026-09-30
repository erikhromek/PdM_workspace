Práctica 5
===

- Se implementó un wrapper del módulo de UART que usa UART2.
- Se implementó una FSM para leer los siguientes comandos UART y controlar el 
LED:
	- HELP
	- LED ON
	- LED OFF
	- LED TOGGLE
	- LED STATUS
- La FSM maneja y devuelve diferentes estados de acuerdo al resultado.
- Solo se reciben los siguientes caracteres: a-z, a-Z, #,
- Los comandos comentados se hacen solamente con el caracter "#" por una 
cuestión de simplicidad.

No se implementó el mecanismo de reconfiguración del baudrate de UART.

No se implementó el uso del estado CMD_ERR_ARG ya que el comando de "LED ..." 
y sus variantes se implementaron como comandos diferentes por una cuestión de
simplicidad.
