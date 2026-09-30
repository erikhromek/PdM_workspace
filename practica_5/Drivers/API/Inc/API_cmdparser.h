/*
 * API_cmdparser.h
 *
 *  Created on: 24 sept 2026
 *      Author: erik
 */

#ifndef API_INC_API_CMDPARSER_H_
#define API_INC_API_CMDPARSER_H_

#define CMD_MAX_LINE 64
#define CMD_MAX_TOKENS 3

typedef enum {
	CMD_OK = 0, CMD_ERR_OVERFLOW, CMD_ERR_SYNTAX, CMD_ERR_UNKOWN, CMD_ERR_ARG
} cmd_status_t;

/**
 * @fn void cmdParserInit(void)
 * @brief	Inicializa el módulo parser de comandos
 *
 * @pre
 * @post
 */
void cmdParserInit(void);

/**
 * @fn void cmdPoll(void)
 * @brief Máquina de estados del parser. Debe ser llamada periódicamente desde
 * 	      el bucle. Procesa hasta 16 bytes por invocación.
 *
 * @pre
 * @post
 */
void cmdPoll(void);

/**
 * @fn void cmdPrintHelp(void)
 * @brief Imprime por UART la lista de comandos disponibles
 *
 * @pre
 * @post
 */
void cmdPrintHelp(void);

#endif /* API_INC_API_CMDPARSER_H_ */
