/*
 * API_cmdparser.c
 *
 *  Created on: 24 sept 2026
 *      Author: Erik Hromek
 */

#include "API_cmdparser.h"
#include "API_uart.h"
#include <string.h>
#include "API_delay.h"
#include "stm32f4xx_hal.h"

#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA

typedef enum {
	CMD_IDLE, CMD_RECEIVING, CMD_ERROR, CMD_PROCESS, CMD_EXEC
} cmdParserState_t;

typedef enum {
	ON, OFF, TOGGLE
} ledState_t;

static cmdParserState_t FSMcurrentState;

#define MAX_COMMAND_LENGTH 64

// Buffer para almacenar caracteres recibidos
static uint8_t buf[CMD_MAX_LINE];

// Almacena la posición actual del buffer
static uint8_t currentIndex = 0;

/*
 * String de comandos, para fácil edición contra el listado real.
 * Podría armarse un string de forma dinámica.
 */
static uint8_t COMMAND_HELP[] =
		"Command list: HELP,LED ON,LED OFF,LED TOGGLE,LED STATUS\r\n";
static char *COMMANDS[] = { "HELP", "LED ON", "LED OFF", "LED TOGGLE",
		"LED STATUS" };

// Comando actual detectado. Se usa para recorrer COMMANDS[]
static int8_t currentCommandIndex = -1;

static ledState_t ledState;

static const tick_t TOGGLE_CYCLE = 500;
static delay_t delay;

static bool_t uartEnabled;

/*
 * @brief 	Inicializa parser de comandos, UART y delays internos para
 * 			controlar el led
 */
void cmdParserInit() {
	FSMcurrentState = CMD_IDLE;
	ledState = OFF;
	delayInit(&delay, TOGGLE_CYCLE);
	uartEnabled = uartInit();
}

/**
 * @fn uint8_t cmdProcessLine()
 * @brief Procesa una línea e intenta obtener el comando recibido
 *
 * @return Devuelve el índice del comando en la posición de COMMANDS[]
 * 			-2 si es un comentario y se debe ignorar
 * 			-1 si no lo encontró (comando inválido(
 */
static uint8_t cmdProcessLine() {

	static uint8_t arraySize = sizeof(COMMANDS) / sizeof(COMMANDS[0]);
	int8_t indexFound = -1;
	if (buf[0] == '#')
		indexFound = -2; // Es un comentario, ignorar línea.

	for (uint8_t i = 0; i < arraySize; i++) {
		if (strcmp(buf, COMMANDS[i]) == 0) {
			indexFound = i;
			break;
		}
	}
	return indexFound;

}

bool isValid(uint8_t c) {
	/* Permite:
	 * a-z
	 * A-Z
	 * #
	 * CR
	 * LF
	 * SP
	 */

	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == ' ')
			|| (c == '\r') || (c == '\n') || (c == '#') || (c == '/'))
		return true;
	else
		return false;
}

/**
 * @fn void sendCMDStatus(cmd_status_t)
 * @brief Envía por UART el resultado del comando
 *
 * @param status	Estados del parser
 */
static void sendCMDStatus(cmd_status_t status) {
	if (!uartEnabled) {
		return;
	}
	switch (status) {
	case CMD_OK:
		uartSendString("COMMAND OK\r\n");
		break;
	case CMD_ERR_OVERFLOW:
		uartSendString("COMMAND TOO LONG\r\n");
		break;
	case CMD_ERR_SYNTAX:
		uartSendString("COMMAND INVALID\r\n");
		break;
	case CMD_ERR_UNKOWN:
		uartSendString("ERROR UNKOWN\r\n");
		break;
	case CMD_ERR_ARG:
		uartSendString("ARGUMENTS INVALID\r\n");
		break;
	default:
		break;
	}
}

/*
 * @brief 	Reinicializa el estado interno del parser, limpia buffers internos
 */
void resetState() {
	FSMcurrentState = CMD_IDLE;
	for (uint8_t i = 0; i < CMD_MAX_LINE; i++)
		buf[i] = '\0';

	currentIndex = 0;
	currentCommandIndex = -1;
}

void sendHelp() {
	if (!uartEnabled) {
		return;
	}
	sendCMDStatus(CMD_OK);
	uartSendString(COMMAND_HELP);
}

void enableLED() {
	if (!uartEnabled) {
		return;
	}
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	ledState = ON;
	sendCMDStatus(CMD_OK);
	uartSendString("LED IS ON\r\n");
}

void disableLED() {
	if (!uartEnabled) {
		return;
	}
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	ledState = OFF;
	sendCMDStatus(CMD_OK);
	uartSendString("LED IS OFF\r\n");

}

void toggleLED() {
	if (!uartEnabled) {
		return;
	}
	ledState = TOGGLE;
	sendCMDStatus(CMD_OK);
	uartSendString("LED IS BLINKING\r\n");
}

void sendLEDStatus() {
	if (!uartEnabled) {
		return;
	}
	switch (ledState) {
	case ON:
		sendCMDStatus(CMD_OK);
		uartSendString("LED IS ON\r\n");
		break;
	case OFF:
		sendCMDStatus(CMD_OK);
		uartSendString("LED IS OFF\r\n");
		break;
	case TOGGLE:
		sendCMDStatus(CMD_OK);
		uartSendString("LED IS BLINKING\r\n");
		break;
	default:
		sendCMDStatus(CMD_ERR_UNKOWN);
	}
}

/**
 * @fn void cmdPoll(void)
 * @brief Chequea por caracteres recibidos del buffer y se encarga del loop
 *			del blink del LED
 *
 * @pre
 * @post
 */
void cmdPoll(void) {
	if (!uartEnabled) {
		return;
	}

	// Chequea por el estado del LED
	if (ledState == TOGGLE) {
		if (!delayIsRunning(&delay)) {
			delayWrite(&delay, TOGGLE_CYCLE);
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		}
	}

	uint8_t c = '\0';
	uartReceiveStringSize(&c, 1);
	/*
	 * Recibe un caracter y realiza lo siguiente:
	 * 1. chequea que no sea nulo, si no, lo ignora
	 * 2. chequea que sea un caracter válido antes de guardarlo en el buffer e
	 *    incrementar el índide
	 * 3. si detecta que es un CR o LF, pasa al siguiente estado
	 * 4. luego, determina a qué comando corresponde y en caso de éxito, pasa
	 *    al siguiente estado, si no, da error
	 * 5. ejecuta el comando corresponde y reinicia el estado del parser
	 */
	switch (FSMcurrentState) {
	case CMD_ERROR:
		if (currentIndex >= CMD_MAX_LINE) {
			sendCMDStatus(CMD_ERR_OVERFLOW);
		} else
			sendCMDStatus(CMD_ERR_SYNTAX);
		uartSendString(buf);
		resetState();
		break;
	case CMD_IDLE:
		if (c != '\r' && c != '\n' && isValid(c)) {
			FSMcurrentState = CMD_RECEIVING;
			buf[currentIndex] = c;
			currentIndex++;
		}
		break;
	case CMD_RECEIVING:
		if (c != '\r' && c != '\n' && isValid(c)) {
			if (currentIndex < CMD_MAX_LINE) {
				buf[currentIndex] = c;
				currentIndex++;
			} else {
				FSMcurrentState = CMD_ERROR;
			}

		} else if (c == '\r' || c == '\n') {
			buf[currentIndex] = '\0';
			FSMcurrentState = CMD_PROCESS;
		}
		break;
	case CMD_PROCESS:
		currentCommandIndex = cmdProcessLine();
		if (currentCommandIndex != -1) {
			FSMcurrentState = CMD_EXEC;
		} else {
			FSMcurrentState = CMD_ERROR;
		}
		break;
	case CMD_EXEC:
		switch (currentCommandIndex) {
		case -2:
			resetState();
			break;
		case 0:
			sendHelp();
			resetState();
			break;
		case 1:
			// Activar LED
			enableLED();
			resetState();
			break;
		case 2:
			disableLED();
			resetState();
			break;
		case 3:
			toggleLED();
			resetState();
			break;
		case 4:
			sendLEDStatus();
			resetState();
			break;
		default:
			FSMcurrentState = CMD_ERROR;
			break;
		}
		break;
	default:
		cmdParserInit();
		break;
	}
}
