#ifndef _DEBUG_UART_
#define _DEBUG_UART_

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#include "queues.h"

static UART_Handle uart_debug;
static UART_Params uart_debug_params;

void uart_debug_init();

void *uartDebugThread(void *arg0);

#endif
