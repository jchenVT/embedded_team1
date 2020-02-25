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
//star debug locations
#define STAR_MAIN_START         0x00
#define STAR_WHILE_BEGIN        0x01
#define STAR_WAIT_MESSAGE       0x02
#define STAR_RECEIVE_MESSAGE    0x03
#define STAR_UPDATE_TASK        0x04

// sensor queue locations
#define RQ_MotorQ_CREATE        0x10
#define RQ_MotorQ_SEND          0x11
#define RQ_MotorQ_RECEIVE       0x12

#define RQ_EncoderQ_CREATE      0x13
#define RQ_EncoderQ_SEND        0x14
#define RQ_EncoderQ_RECEIVE     0x15

// sensor queue locations
#define RQ_MQTTReceive_CREATE   0x20
#define RQ_MQTTReceive_spiSEND  0x21
#define RQ_MQTTReceive_senSEND  0x22
#define RQ_MQTTReceive_RECEIVE  0x23

// sensor queue locations
#define RQ_MQTTSend_CREATE      0x30
#define RQ_MQTTSend_stateSEND   0x31
#define RQ_MQTTSend_encodSEND   0x32
#define RQ_MQTTSend_RECEIVE     0x33

// uart locations
#define UART_INITIALIZE         0x40
#define UART_OPENING            0x41
#define UART_WRITING            0x42
#define UART_CLOSING            0x43

// spi locations
#define SPI_INITIALIZE          0x50
#define SPI_OPENING             0x51
#define SPI_READING_128         0x52
#define SPI_READING_129         0x53
#define SPI_READING_130         0x54
#define SPI_SENDING             0x55
#define SPI_CLOSING             0x56

#define SPI_ENCODER_INIT        0x57
#define SPI_ENCODER_CLEARING    0x47

// timer locations
#define SPI_TIMER_INITIALIZE    0x60
#define SPI_TIMER_OPENING       0x61
#define SPI_TIMER_CLOSING       0x62

// FAILURE Locations
#define FAIL_UART_INIT          0x70
#define FAIL_SPI_INIT           0x71
#define FAIL_TIMER_INIT         0x72
#define FAIL_TIMER_START        0x73

#define FAIL_MotorQ_INIT        0x80
#define FAIL_EncoderQ_INIT      0x81
#define FAIL_MQTTRecvQ_INIT     0x82
#define FAIL_MQTTSendQ_INIT     0x83
#define FAIL_MainThread_INIT    0x84
#define FAIL_MotorsThread_INIT  0x85
#define FAIL_EncoderThread_INIT 0x86
#define FAIL_MQTTRecvThread_INIT 0x87
#define FAIL_MQTTSendThread_INIT 0x86

#define FAIL_UART_RECEIVE       0x90
#define FAIL_SPI_READING_128    0x91
#define FAIL_SPI_READING_129    0x92
#define FAIL_SPI_READING_130    0x93
#define FAIL_SPI_SEND_TO_Q      0x94

#define FAIL_UNKNOWN_CODE       0xFF

#ifndef ROVER_DEBUG_H_
#define ROVER_DEBUG_H_

void debug_setup();

// Debug function that outputs to GPIO pins
// Use logic analyzer to determine location
// outLoc must be <= 127
void dbgOutputLoc(unsigned int outLoc);
void stop_all(unsigned int FAILURE_CODE);

#endif /* ROVER_DEBUG_H_ */
