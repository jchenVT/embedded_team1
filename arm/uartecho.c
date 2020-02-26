/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uartecho.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Driver configuration */
#include "ti_drivers_config.h"
#include "arm.h"

static UART_Handle uart;
static PWM_Handle pwm_x, pwm_y, pwm_z, pwm_claw;
static TimerHandle_t timer10ms;

static movqData_t off_pos = {1600, 1100, 1000, 800};
static movqData_t start_pos = {1600, 1500, 1800, 800};

static movqData_t current_pos, goal_pos;

static aState a;
static dState d;

// instantiates the uart for the arm messaging queue
void arm_init() {
    UART_Params uartParams;

    /* Call driver init functions */
    UART_init();

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.baudRate = 115200;

    uart = UART_open(CONFIG_UART_0, &uartParams);

    if (uart == NULL) {
        /* UART_open() failed */
        while (1);
    }

    PWM_Params pwmParams;

    PWM_init();
    PWM_Params_init(&pwmParams);
    pwmParams.dutyUnits = PWM_DUTY_US;
    pwmParams.dutyValue = 0;
    pwmParams.periodUnits = PWM_PERIOD_US;
    pwmParams.periodValue = 20000;

    pwm_x = PWM_open(CONFIG_PWM_0, &pwmParams);
    pwm_y = PWM_open(CONFIG_PWM_1, &pwmParams);
    pwm_z = PWM_open(CONFIG_PWM_2, &pwmParams);
    pwm_claw = PWM_open(CONFIG_PWM_3, &pwmParams);

    if (!pwm_x) {
        UART_write(uart, "pwm x failed\r\n", 14);
    }
    if (!pwm_y) {
        UART_write(uart, "pwm y failed\r\n", 14);
    }
    if (!pwm_z) {
        UART_write(uart, "pwm z failed\r\n", 14);
    }
    if (!pwm_claw) {
        UART_write(uart, "pwm c failed\r\n", 14);
    }

    PWM_start(pwm_x);
    PWM_start(pwm_y);
    PWM_start(pwm_z);
    PWM_start(pwm_claw);

    // initialize software timer for smooth arm movement
    timer10ms = xTimerCreate("10ms", pdMS_TO_TICKS(10), pdTRUE, NULL, movementCallback);
}

// checks for valid char and sends the corresponding message to the queue
bool validChar(char in) {
    switch (in) {
    case 'o':
        goal_pos = off_pos;
        return true;
    case 's':
        goal_pos = start_pos;
        return true;
    case 'c':
        if (current_pos.boy_value == 800) {
            goal_pos.boy_value = 1800;
        }
        else {
            goal_pos.boy_value = 800;
        }
        return true;
    case 'l':
        goal_pos.yee_value = 380;
        return true;
    case 'r':
        goal_pos.yee_value = 2430;
        return true;
    case 'd':
        if (current_pos.cow_value == 1800) {
            goal_pos.cow_value = 1100;
        }
        else {
            goal_pos.cow_value = 1800;
        }
        return true;
    case 'f':
        if (current_pos.haw_value <= 1700 && current_pos.haw_value > 1500) {
            goal_pos.haw_value = 1100;
        }
        else {
            goal_pos.haw_value = 1700;
        }
        return true;
    }
    return false;
}

int doneStruct(movqData_t s) {
    return s.yee_value == 0 && s.haw_value == 0 && s.cow_value == 0 && s.boy_value == 0;
}

int armDone() {
    return memcmp(&current_pos, &goal_pos, sizeof(current_pos)) == 0;
}
/*
 *  ========mainThread ========
 */
void *armDebugThread(void *arg0)
{
    char input;
    d = WaitingForAngles;
    movqData_t a1 = {1200, 1500, 1800, 1600};
    movqData_t b = {1200, 1500, 1800, 800};
    // set the current position of the arm to its off position
    current_pos = start_pos;
    // set the current goal position of the arm to the start position
    goal_pos = off_pos;

    /* Loop forever */
    while (1) {
        if (d == WaitingForAngles) {
            UART_read(uart, &input, 1);
            UART_write(uart, &input, 1);
            if (validChar(input)) {
                UART_write(uart, "valid\r\n", 7);
                d = WaitingForAck;
                a = Moving;
            }
        }
        if (d == WaitingForAck) {
            UART_write(uart, "ack\r\n", 5);
            if (a == NotMoving) {
                UART_write(uart, "angle\r\n", 7);
                d = WaitingForAngles;
            }
        }
    }
}

void *mainArmThread(void *arg0) {

    while (1) {
        if (a == Moving) {
            UART_write(uart, "move\r\n", 6);
            // start timer if it is not active
            if (xTimerIsTimerActive(timer10ms) == pdFALSE) {
                xTimerStart(timer10ms, 0);
            }
            if (armDone()) {
                char buf[40];
                int ret = snprintf(buf, 20, "current %d %d %d %d", current_pos.yee_value,
                                   current_pos.haw_value, current_pos.cow_value, current_pos.boy_value);
                UART_write(uart, "done\r\n", 6);
                UART_write(uart, buf, ret);
                a = NotMoving;
            }
        }
        else if (a == NotMoving) {
            UART_write(uart, "no move\r\n", 9);
            if (xTimerIsTimerActive(timer10ms) == pdTRUE) {
                UART_write(uart, "stopped\r\n", 9);
                xTimerStop(timer10ms, 0);
            }
        }
    }
}

void movementCallback(TimerHandle_t xTimer) {
//    UART_write(uart, "callback\r\n", 10);
    if (current_pos.yee_value < goal_pos.yee_value) {
        current_pos.yee_value += STEPSIZE;
        PWM_setDuty(pwm_x, current_pos.yee_value);
    }
    else if (current_pos.yee_value > goal_pos.yee_value) {
        current_pos.yee_value -= STEPSIZE;
        PWM_setDuty(pwm_x, current_pos.yee_value);
    }
    else {
        PWM_setDuty(pwm_x, current_pos.yee_value);
    }

    if (current_pos.haw_value < goal_pos.haw_value) {
        current_pos.haw_value += STEPSIZE;
        PWM_setDuty(pwm_y, current_pos.haw_value);
    }
    else if (current_pos.haw_value > goal_pos.haw_value) {
        current_pos.haw_value -= STEPSIZE;
        PWM_setDuty(pwm_y, current_pos.haw_value);
    }
    else {
        PWM_setDuty(pwm_y, current_pos.haw_value);
    }

    if (current_pos.cow_value < goal_pos.cow_value) {
        current_pos.cow_value += STEPSIZE;
        PWM_setDuty(pwm_z, current_pos.cow_value);
    }
    else if (current_pos.cow_value > goal_pos.cow_value) {
        current_pos.cow_value -= STEPSIZE;
        PWM_setDuty(pwm_z, current_pos.cow_value);
    }
    else {
        PWM_setDuty(pwm_z, current_pos.cow_value);
    }

    if (current_pos.boy_value < goal_pos.boy_value) {
        current_pos.boy_value += STEPSIZE;
        PWM_setDuty(pwm_claw, current_pos.boy_value);
    }
    else if (current_pos.boy_value > goal_pos.boy_value) {
        current_pos.boy_value -= STEPSIZE;
        PWM_setDuty(pwm_claw, current_pos.boy_value);
    }
    else {
        PWM_setDuty(pwm_claw, current_pos.boy_value);
    }
}
