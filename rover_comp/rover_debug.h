/*
 * rover_debug.h
 *
 *  Created on: Feb 20, 2020
 *      Author: trieu
 */

/* TO DO:
 *
 * Rewrite debug codes
*/
#include <FreeRTOS.h>
#include <task.h>
#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"

#include <stdbool.h>
#include <stdio.h>

#include <rover_uart.h>
#include <rover_spi.h>

// Debug Output Location Macros Definition
// Used in dbgOUtputLoc(_MACRO_)
#define FAIL_MotorQ_INIT        0x80
#define FAIL_EncoderQ_INIT      0x81
#define FAIL_MQTTRecvQ_INIT     0x82
#define FAIL_MQTTSendQ_INIT     0x83
#define FAIL_MainThread_INIT    0x84
#define FAIL_MotorsThread_INIT  0x85
#define FAIL_EncoderThread_INIT 0x86
#define FAIL_MQTTRecvThread_INIT 0x87
#define FAIL_MQTTSendThread_INIT 0x88


#define FAIL_Q1_READ            0x71
#define FAIL_Q1_WRONG_DATA      0x72
#define FAIL_T1_INIT            0x73
#define FAIL_T1_START           0x74
#define FAIL_T1_SEND_TO_Q1      0x75
#define FAIL_T2_INIT            0x76
#define FAIL_T2_START           0x77
#define FAIL_T2_SEND_TO_Q1      0x78
#define FAIL_ADC_INIT           0x79
#define FAIL_UART_INIT          0xFA
#define FAIL_UNKNOWN_CODE       0xFB

//star debug locations
#define STAR_MAIN_START         0x00
#define STAR_WHILE_BEGIN        0x01
#define STAR_WAIT_MESSAGE       0x02
#define STAR_RECEIVE_MESSAGE    0x03
#define STAR_UPDATE_TASK        0x04

// sensor queue locations
#define SQ_Q1_CREATE            0x10
#define SQ_Q1_SEND_TIME         0x11
#define SQ_Q1_SEND_SENSOR       0x12
#define SQ_Q1_RECEIVE           0x13

// timer one locations
#define T1_TIMER_INITIALIZE     0x20
#define T1_TIMER_OPENED         0x21
#define T1_CALLBACK             0x22

// timer two locations
#define T2_TIMER_INITIALIZE     0x30
#define T2_TIMER_OPENED         0x31
#define T2_CALLBACK_BEGIN       0x32
#define T2_CALLBACK_ADC_READ    0x33

// uart locations
#define UART_INITIALIZE         0x40
#define UART_OPENING            0x41
#define UART_WRITING            0x42
#define UART_CLOSING            0x43


#ifndef ROVER_DEBUG_H_
#define ROVER_DEBUG_H_

void debug_setup();

// Debug function that outputs to GPIO pins
// Use logic analyzer to determine location
// outLoc must be <= 127
void dbgOutputLoc(unsigned int outLoc);
void stop_all(unsigned int FAILURE_CODE);

#endif /* ROVER_DEBUG_H_ */
