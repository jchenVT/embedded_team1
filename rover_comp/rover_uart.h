#ifndef ROVER_UART_H_
#define ROVER_UART_H_

#include <ti/drivers/UART.h>

#include <stdbool.h>
#include <stdio.h>
#include "ti_drivers_config.h"
#include <rover_queues.h>
#include <rover_debug.h>

// Set up function
void uart_setup();
void uart_close();
void *uartThread(void *arg0);

#endif
