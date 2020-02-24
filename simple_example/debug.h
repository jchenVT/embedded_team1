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

/* Proximity 1 */
#define INIT_TIMER1                 0x11
#define TIMER1_CALLBACK             0x12
#define SEND_PROX1Q                 0x13

/* Proximity 2 */
#define INIT_TIMER2                 0x21
#define TIMER2_CALLBACK             0x22
#define SEND_PROX2Q                 0x23

/* RGB */

/* Failure */
#define FAILED_INIT_THREADS         0x71
#define FAILED_INIT_QUEUES          0x72
#define FAILED_TIMER1_INIT          0x73
#define FAILED_TIMER1_BADVAL        0x74
#define FAILED_TIMER2_INIT          0x75
#define FAILED_TIMER2_BADVAL        0x76
#define FAILED_SEND_PROX1Q          0x77
#define FAILED_SEND_PROX2Q          0x78
#define FAILED_SEND_RGBQ            0x79
#define FAILED_UNKNOWN_CODE         0x7F

///////////////////////////////////////////////////

extern UART_Handle uart;

void debug_setup();
void dbgOutputLoc(unsigned int outLoc);
void stop_all(unsigned int FAILURE_CODE);

#endif
