#ifndef _DEBUG_UART_
#define _DEBUG_UART_

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <string.h>

#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"

#include "queues.h"

void uart_debug_init();

void *uartDebugThread(void *arg0);

#endif
