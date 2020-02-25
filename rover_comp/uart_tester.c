/*
 * uart_tester.c
 *
 *  Created on: Feb 24, 2020
 *      Author: trieu
 */

#include <uart_tester.h>
#include <rover_queues.h>
#include <rover_debug.h>

static TimerHandle_t uartTimer_handle;
static char speed = 48;
static int counter = 0;

void uartTimer_setup() {

    uartTimer_handle = xTimerCreate("uart_timer", pdMS_TO_TICKS(100), pdTRUE, (void*)0, uarttimerCallback);

    if (uartTimer_handle == NULL) {
        stop_all(FAIL_TIMER_INIT);
    }
    if (xTimerStart(uartTimer_handle, 0) != pdPASS) {
        stop_all(FAIL_TIMER_START);
    }
}

void testing_stop(){
    xTimerStop(uartTimer_handle,0);
}

void uarttimerCallback(TimerHandle_t uartTimer_handle) {

    if (speed < 36) {

        speed = speed + 4;

        sendMsgToMotorsQ(128, 0, speed);
        sendMsgToMotorsQ(129, 0, speed);
        sendMsgToMotorsQ(130, 0, speed);
    }
    else {
        if (counter < 20) {
            sendMsgToMotorsQ(128, 1, speed);
            sendMsgToMotorsQ(129, 1, speed);
            sendMsgToMotorsQ(130, 1, speed);
        }
        else if (speed > 0) {
            speed -= 4;
            sendMsgToMotorsQ(128, 0, speed);
            sendMsgToMotorsQ(129, 1, speed);
            sendMsgToMotorsQ(130, 0, 0);
        }
    }

    if (counter > 50) {
        stop_all(0xFF);
    }

    counter++;
}
