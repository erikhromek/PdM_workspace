Práctica 4
===

Se implementó una máquina de estados finitos para un debouncer de botón.

_Nota: El ejercicio 1 quedó absorbido por el ejercicio 2_

__¿Es adecuado el control de los parámetros pasados por el usuario que se hace en las funciones implementadas? ¿Se controla que sean valores válidos? ¿Se controla que estén dentro de los rangos correctos?__


Se incorporaron varios controles de parámetros.


__¿Se nota una mejora en la detección de las pulsaciones respecto a la práctica 0? ¿Se pierden pulsaciones? ¿Hay falsos positivos?__


El funcionamiento del manejo del botón es muy preciso. En el único caso que puede hacer "falsos positivos" o perder pulsaciones es cuando se aprieta por menos tiempo de los 40 ms, el mínimo requerido para los debounce.

__
¿Es adecuada la temporización con la que se llama a debounceFSM_update()? ¿Y a readKey()? ¿Qué pasaría si se llamara con un tiempo mucho más grande? ¿Y mucho más corto?__

La temporización es óptima, ya que se llama en cada ciclo del superloop del programa. Podría llamarse cada menos tiempo, pero teniendo en cuenta estas restricciones:

- Si el tiempo de debounce es 40 ms, se debe chequear como máximo a más tardar cada 40 ms.
- Si el tiempo de chequeo de readKey() es muy alto, se sentirá poco responsivo el botón.
