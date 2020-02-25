/*
 * uart_tester.h
 *
 *  Created on: Feb 24, 2020
 *      Author: trieu
 */

#ifndef UART_TESTER_H_
#define UART_TESTER_H_

#include <FreeRTOS.h>
#include <timers.h>

void uartTimer_setup();
void uarttimerCallback(TimerHandle_t timer0);

#endif /* UART_TESTER_H_ */
