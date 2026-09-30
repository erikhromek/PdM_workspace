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

#define MAX_COMMAND_LENGTH 64

static cmdParserState_t FSMcurrentState;
static uint8_t buf[CMD_MAX_LINE];
static uint8_t commandBuf[MAX_COMMAND_LENGTH];
static uint8_t currentIndex = 0;
static int8_t currentCommandIndex = -1;
static uint8_t COMMAND_HELP[] =
		"Command list: HELP,LED ON,LED OFF,LED TOGGLE,LED STATUS\r\n";
static char *COMMANDS[] =
		{ "HELP", "LED ON", "LED OFF", "LED TOGGLE", "LED STATUS" };
static ledState_t ledState;

static const tick_t TOGGLE_CYCLE = 500;
delay_t delay;

void cmdParserInit() {
	FSMcurrentState = CMD_IDLE;
	ledState = OFF;
	delayInit(&delay, TOGGLE_CYCLE);
	uartInit();
}
int8_t cmdProcessLine() {
	static uint8_t arraySize = sizeof(COMMANDS) / sizeof(COMMANDS[0]);
	int8_t indexFound = -1;
	if (buf[0] == '#')
		indexFound = -2; // Es un comentario. Ignorar línea.

	for (uint8_t i = 0; i < arraySize; i++) {
		if (strstr(buf, COMMANDS[i]) != NULL) {
			indexFound = i;
			break;
		}
	}
	return indexFound;

}

static void sendCMDStatus(cmd_status_t status) {
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

void resetState() {
	FSMcurrentState = CMD_IDLE;
	for (uint8_t i = 0; i < CMD_MAX_LINE; i++)
		buf[i] = '\0';

	for (uint8_t i = 0; i < MAX_COMMAND_LENGTH; i++)
		commandBuf[i] = '\0';

	currentIndex = 0;
	currentCommandIndex = -1;
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

void sendHelp() {
	sendCMDStatus(CMD_OK);
	uartSendString(COMMAND_HELP);
}

void enableLED() {
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	ledState = ON;
	sendCMDStatus(CMD_OK);
	uartSendString("LED IS ON\r\n");
}

void disableLED() {
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	ledState = OFF;
	sendCMDStatus(CMD_OK);
	uartSendString("LED IS OFF\r\n");

}

void toggleLED() {
	ledState = TOGGLE;
	sendCMDStatus(CMD_OK);
	uartSendString("LED IS BLINKING\r\n");
}

void sendLEDStatus() {
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

void cmdPoll(void) {
	if (ledState == TOGGLE) {
		if (!delayIsRunning(&delay)) {
			delayWrite(&delay, TOGGLE_CYCLE);
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		}
	}

	uint8_t c;
	uartReceiveStringSize(&c, 1);
	if (c != '\0') {
		switch (FSMcurrentState) {
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

			} else {
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
				sendCMDStatus(CMD_ERR_SYNTAX);
				resetState();
				break;
			}
			break;
		case CMD_ERROR:
			if (currentIndex >= CMD_MAX_LINE)
				sendCMDStatus(CMD_ERR_OVERFLOW);
			else
				sendCMDStatus(CMD_ERR_SYNTAX);
			resetState();
			break;
		default:
			cmdParserInit();
			break;
		}

	}
}
