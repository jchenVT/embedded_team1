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

/* Proximity Sensors */
#define INIT_TIMER                  0x11
#define TIMER_CALLBACK              0x12
#define SEND_PROX1Q                 0x13
#define SEND_PROX2Q                 0x14

/* RGB */
#define INIT_I2C                    0x21
#define I2C_CALLBACK                0x22
#define SEND_RGBQ                   0x23

/* Proximity Q1 Message Packaging */
#define INIT_PQ1READ                0x31
#define WAIT_PQ1READ                0x32
#define RECV_PQ1READ                0x33
#define SEND_PQ1MQTT                0x34

/* Proximity Q2 Message Packaging */
#define INIT_PQ2READ                0x41
#define WAIT_PQ2READ                0x42
#define RECV_PQ2READ                0x43
#define SEND_PQ2MQTT                0x44

/* RGB Message Packaging */
#define INIT_RGBQREAD               0x51
#define WAIT_RGBQREAD               0x52
#define RECV_RGBQREAD               0x53
#define SEND_RGBQMQTT               0x54

/* Failure */
#define FAILED_INIT_THREADS         0x71
#define FAILED_INIT_QUEUES          0x72
#define FAILED_START_DEBUG          0x73
#define FAILED_TIMER_INIT           0x74

#define FAILED_PROX1_BADVAL         0x75
#define FAILED_PROX2_BADVAL         0x76
#define FAILED_SEND_PROX1Q          0x77
#define FAILED_SEND_PROX2Q          0x78

#define FAILED_I2C_INIT             0x79
#define FAILED_I2C_CALLBACK         0x7A
#define FAILED_RGB_BADVAL           0x7B
#define FAILED_SEND_RGBQ            0x7C

#define FAILED_PQ1_BADVAL           0x7D
#define FAILED_PQ2_BADVAL           0x7E
//#define FAILED_RGBQ_BADVAL          0x7F

#define FAILED_UNKNOWN_CODE         0x7F

///////////////////////////////////////////////////

extern UART_Handle uart;

void debug_setup();
void dbgOutputLoc(unsigned int outLoc);
void stop_all(unsigned int FAILURE_CODE);

void *uartThread(void *arg0);
unsigned char receiveFromUARTQ();
void dbgUARTVal(unsigned char outVal);

#endif
