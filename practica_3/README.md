Práctica 3
===

_¿Es suficientemente clara la consigna 2 o da lugar a implementaciones con distinto comportamiento?_

```
En mi opinión, se entiende la consigna. Lo único que no es claro cuantas veces debe titilar un LED. En el caso mío puse solo un ciclo de ON/OFF.
```

_¿Se puede cambiar el tiempo de encendido del led fácilmente en un solo lugar del código o éste está hardcodeado? ¿Hay números “mágicos” en el código?_

```
Se pueden agregar ciclos fácilmente y cambiar el tiempo de encendido de cada ciclo modificando el vector. No hay números mágicos en el código.
```

_¿Qué bibliotecas estándar se debieron agregar a API_delay.h para que el código compile? Si las funcionalidades de una API propia crecieran, habría que pensar cuál sería el mejor lugar para incluir esas bibliotecas y algunos typedefs que se usen en la implementación, ¿Cuál sería el mejor lugar?._

```
Tuve que agregar los siguientes includes:

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "stdbool.h"

Creería que lo ideal seria crear un archivo global .h donde esten todos los include necesarios y definiciones de tipos de datos propios del proyecto.

```

_¿Es adecuado el control de los parámetros pasados por el usuario que se hace en las funciones implementadas? ¿Se controla que sean valores válidos? ¿Se controla que estén dentro de los rangos esperados?_

```
Es adecuado, se controlan que las variables como punteros esten inicializadas. El problema sería si el usuario intenta generar un contador de tiempo con valor 0, funcionaría pero no representa una magnitud de tiempo "real" por decirlo de alguna forma.
```
