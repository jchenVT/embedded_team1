#ifndef _SQ_Q1_SEND_SENSORTEAM1_DEBUG_
#define _TEAM1_DEBUG_

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <stdbool.h>
#include <stdio.h>
#include "ti_drivers_config.h"

///// Debug output location macro definitions /////

/* Main thread */
#define INIT_THREADS                0x01

/* Proximity 1 Thread */
#define INIT_UART0                  0x11
#define WAITING_READ_UART0          0x12
#define SEND_PROX1Q                 0x13

/* Proximity 2 Thread */
#define INIT_UART1                  0x21
#define WAITING_READ_UART1          0x22
#define SEND_PROX2Q                 0x23

/* RGB Callback */

/* Failure */
#define FAILED_INIT_THREADS         0x71
#define FAILED_INIT_QUEUES          0x72
#define FAILED_UART0_INIT           0x73
#define FAILED_UART0_READ           0x74
#define FAILED_UART0_BAD_VAL        0x75
#define FAILED_UART1_INIT           0x76
#define FAILED_UART1_READ           0x77
#define FAILED_UART1_BAD_VAL        0x78
#define FAILED_SEND_PROX1Q          0x79
#define FAILED_SEND_PROX2Q          0x7A
#define FAILED_SEND_RGBQ            0x7B
#define FAILED_UNKNOWN_CODE         0x7F

///////////////////////////////////////////////////

//extern UART_Handle uart;

void debug_setup();
void dbgOutputLoc(unsigned int outLoc);
void stop_all(unsigned int FAILURE_CODE);

#endif
