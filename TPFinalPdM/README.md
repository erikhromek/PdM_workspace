# Trabajo Final para PdM y PCSE – Termómetro con alarma de temperatura

Trabajo final de las materias Programación de Microcontroladores y Protocolos de Comunicación de Sistemas Embebidos (CESE). Es un termómetro digital con alarma configurable, implementado en C sobre una placa **NUCLEO-F446RE** (STM32F446RE).

## Qué hace

El sistema mide la temperatura ambiente con un sensor **DHT11**, la muestra en un display de 4 dígitos **TM1637** y activa un **buzzer** cuando la temperatura alcanza o supera un umbral configurado. El umbral por defecto es de 35 °C y puede ajustarse entre 0 y 99 °C. De todas formas, el DHT11 no mide más de 50 °C.

## Funcionamiento

El programa corre en un superloop no bloqueante y se organiza en dos modos:

- **Modo display**: cada 3 segundos se lee el DHT11 y se actualiza el display con la temperatura medida.
- **Modo configuración**: el display muestra el umbral de la alarma, que se incrementa o decrementa con los botones UP y DOWN.

El botón **CONFIG** alterna entre ambos modos, y el botón **ALARM** activa o desactiva la alarma en cualquier momento.

La alarma es una máquina de estados con tres estados: `ALARM_DISABLED` (buzzer apagado), `ALARM_CONFIGURED` (armada, esperando que la temperatura llegue al umbral) y `ALARM_ON` (buzzer sonando). Cuando la temperatura baja del umbral, vuelve a `ALARM_CONFIGURED`, de modo que se rearma sola mientras siga habilitada.

## Hardware y conexiones

| Periférico | Pin |
|---|---|
| DHT11 | PB6 |
| TM1637 CLK | PA6 |
| TM1637 DIO | PA9 |
| Buzzer | PA7 |
| Botón UP | PB10 |
| Botón DOWN | PB5 |
| Botón ALARM | PA10 |
| Botón CONFIG | PA8 |

## Organización del código

- `Src/main.c`: inicialización de periféricos y máquina de estados principal (display / configuración), además del despacho de eventos de botones.
- `Src/alarm.c` / `Inc/alarm.h`: módulo de alarma. Maneja el umbral, el buzzer y la máquina de estados de la alarma.
- `Src/app_button.c` / `Inc/app_button.h`: módulo de botones. Gestiona los cuatro pulsadores con una máquina de estados y expone los eventos (`BTN_ALARM`, `BTN_UP`, `BTN_DOWN`, `BTN_CONFIG`).
- Drivers reutilizados de prácticas anteriores: `tm1637` (display), `dht11` (sensor) y `delay` (retardos no bloqueantes + retardos bloqueantes de microsegundos).

Los módulos reciben el puerto y el pin por parámetro al inicializarse, por lo que el hardware puede reasignarse sin modificar su lógica interna.
