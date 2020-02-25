/*
 * uart_tester.c
 *
 *  Created on: Feb 24, 2020
 *      Author: trieu
 */

#include <uart_tester.h>
#include <rover_queues.h>

static TimerHandle_t uartTimer_handle;
static char speed = 0;

void uartTimer_setup() {

    uartTimer_handle = xTimerCreate("uart_timer", pdMS_TO_TICKS(1000), pdTRUE, (void*)0, uarttimerCallback);

    if (uartTimer_handle == NULL) {
        stop_all(FAIL_TIMER_INIT);
    }
    if (xTimerStart(uartTimer_handle, 0) != pdPASS) {
        stop_all(FAIL_TIMER_START);
    }
}

void uarttimerCallback(TimerHandle_t uartTimer_handle) {

    speed = (speed + 4) % 48;

    sendMsgToMotorsQ(128, 0, speed);
    sendMsgToMotorsQ(129, 1, speed);
    //sendMsgToMotorsQ(130, 0, speed);

}
