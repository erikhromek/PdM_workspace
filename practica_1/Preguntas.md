Preguntas
===

1. El usar arrays permitiria en lugar de intercambiar entre 2 valores (200 y 500) ir moviendose entre una opción u otra dentro del array. Y el delay se va moviendo de acuerdo a cuantas veces lo apriete (200 -> 500 -> X -> Y ...) y cuando llega al final, vuelvo a la posición 0. Si tuviera que agregar o sacar un valor, simplemente elimino el valor del array.

2. Hay pulsaciones no detectadas porque caen por fuera del delay. Es decir, si se aprieta el botón durante el tiempo del delay, no se detecta, y tampoco tiene una lógica para detectar debounce donde no se aprieta del todo o tiene falsos positivos.

3. Si mantengo el botón siempre pulsado (por lo que hace un switch en cada ciclo) y leo primero que el delay, ocurre lo siguiente:

OFF -> ON
500
ON -> OFF
200
OFF -> ON
500
ON -> OFF
200
...

El LED permanece prendido 500 ms y apagado 200 ms.

Si hago al revés (luego del delay):

Toggle
Delay
Lectura boton

OFF -> ON
200
Switch
ON -> OFF
500
Switch
OFF -> ON
200
Switch
ON -> OFF
500
Switch
OFF -> ON
200
...

Ocurre lo inverso. A fines prácticos,  parecer no haber diferencia entre leer al principio y al final. Puede ser más sencillo de comprender si se lee al principio del bucle.

4. Si el tiempo de encendido fuera muy largo, tendriamos un problema porque me deberíar quedar esperando al menos un ciclo para que lo vuelva a leer. Y si fuera muy corto, cambiaria muchas veces porque leeria varias veces el estado del botón. Ahí deberia pulsar brevemente el botón. Claramente el tiempo de encendido afecta al comportamiento esperado del programa y tendria muchas más pulsaciones detectadas, o no detectadas.
