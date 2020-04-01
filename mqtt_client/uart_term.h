#ifndef __UART_IF_H__
#define __UART_IF_H__

// TI-Driver includes
#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

//Defines

#define UART_PRINT Message

/* API */

UART_Handle InitTerm(void);

//int Report(const char *pcFormat,
//           ...);

int TrimSpace(char * pcInput);

int GetCmd(char *pcBuffer,
           unsigned int uiBufLen);

void Message(const char *str);

void ClearTerm();

char getch(void);

void putch(char ch);

void stop_all();

#endif // __UART_IF_H__
