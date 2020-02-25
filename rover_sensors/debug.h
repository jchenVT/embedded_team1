#ifndef _DEBUG_
#define _DEBUG_

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <stdbool.h>
#include <stdio.h>

#include "ti_drivers_config.h"
#include <ti/drivers/GPIO.h>

// Debug Output Location Macros Definition
// Used in dbgOUtputLoc(_MACRO_)

#define INIT_ALL 0x00 
#define INIT_UART_DEBUG 0x01
#define INIT_GPIO_DEBUG 0x02
#define INIT_UART_DEBUG_Q 0x03

#define UART_DEBUG_OPEN 0x10
#define UART_DEBUG_INITIALIZED 0x11
#define UART_DEBUG_WRITE 0x12
#define UART_DEBUG_TEST 0x13
#define UART_DEBUG_TEST_DONE 0x14

#define UART_LIDAR_OPEN 0x15
#define UART_LIDAR_OPEN_DONE 0x16

#define SPI_SPI_OPEN 0x20
#define SPI_TIMER_OPEN 0x21
#define SPI_PIXY_CALLBACK 0x22
#define SPI_TIMER_CALLBACK 0x23
#define SPI_SEND_TRANSACTION 0x24

#define FAIL_UART_INIT 0x70
#define FAIL_SPI_INIT 0x71
#define FAIL_SPI_TIMER_INIT 0x72
#define FAIL_SPI_TRANSACTION 0x73
#define FAIL_UNKNOWN_CODE 0x7F



// Set up function
void debug_init();

// Debug function that outputs to GPIO pins
// Use logic analyzer to determine location
// outLoc must be <= 127
void dbgOutputLoc(unsigned int outLoc);

void stop_all(unsigned int FAILURE_CODE);

#endif
