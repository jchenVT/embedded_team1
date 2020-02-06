#ifndef _SQ_Q1_SEND_SENSORTEAM1_DEBUG_
#define _TEAM1_DEBUG_

#include <FreeRTOS.h>
#include <task.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

#include <stdbool.h>

#include "ti_drivers_config.h"

// Debug Output Location Macros Definition
// Used in dbgOUtputLoc(_MACRO_)
#define FAILURE 0x78

//star debug locations
#define STAR_MAIN_START 0x00
#define STAR_WHILE_BEGIN 0x01
#define STAR_WAIT_MESSAGE 0x02
#define STAR_RECEIVE_MESSAGE 0x03
#define STAR_UPDATE_TASK 0x04

// sensor queue locations
#define SQ_Q1_CREATE 0x10
#define SQ_Q1_SEND_TIME 0x11
#define SQ_Q1_SEND_SENSOR 0x12
#define SQ_Q1_RECEIVE 0x13

// timer one locations
#define T1_TIMER_INITIALIZE 0x20
#define T1_TIMER_OPENED 0x21
#define T1_CALLBACK 0x22

// timer two locations
#define T2_TIMER_INITIALIZE 0x30
#define T2_TIMER_OPENED 0x31
#define T2_CALLBACK_BEGIN 0x32
#define T2_CALLBACK_ADC_READ 0x33

// uart locations
#define UART_INITIALIZE 0x60
#define UART_OPENING 0x61
#define UART_WRITING 0x62
#define UART_CLOSING 0x63

// Set up function
void debug_setup();

// Send a value through UART
void dbgUARTVal(unsigned char outVal);

// Debug function that outputs to GPIO pins
// Use logic analyzer to determine location
// outLoc must be <= 127
void dbgOutputLoc(unsigned int outLoc);

void stop_all(void);

#endif
