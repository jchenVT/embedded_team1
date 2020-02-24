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

static movqData_t off_pos = {1200, 1000, 1000, 800};
static movqData_t start_pos = {1200, 1000, 600, 800};

static movqData_t current_pos, goal_pos;
static bool done = false;

/*
 *  ======== mainThread ========
 */
void *armDebugThread(void *arg0)
{
    char        input;
    const char  echoPrompt[] = "Echoing characters:\r\n";

    dState inState = WaitingForAngles;

    createMovQ();
    current_pos = off_pos;

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

    UART_write(uart, echoPrompt, sizeof(echoPrompt));

    /* Loop forever */
    while (1) {
        if (inState == WaitingForAngles) {
            UART_read(uart, &input, 1);
            inState = WaitingForAck;
        }
        if (inState == WaitingForAck) {
            if (done) {
                inState = WaitingForAngles;
            }
        }
    }
}

void *mainArmThread(void *arg0) {
    TimerHandle_t timer10ms = xTimerCreate("9ms", pdMS_TO_TICKS(10), pdFALSE, NULL, movementCallback);
    aState armState = NotMoving;

    PWM_Params pwmParams;

    PWM_init();
    PWM_Params_init(&pwmParams);
    pwmParams.dutyUnits = PWM_DUTY_US;
    pwmParams.dutyValue = 0;
    pwmParams.periodUnits = PWM_PERIOD_US;
    pwmParams.periodValue = 100000;

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

    while (1) {
        if (armState == Moving) {
            if (xTimerIsTimerActive(timer10ms) == pdFALSE) {
                xTimerStart(timer10ms, 0);
            }
        }
        else if (armState == NotMoving) {
            if (xTimerIsTimerActive(timer10ms) == pdTRUE) {
                xTimerStop(timer10ms, 0);
            }
            movqData_t tempM;
            receiveFromMovQ(&tempM);
            goal_pos = tempM;
            armState = Moving;
        }
    }
}

void movementCallback(TimerHandle_t xTimer) {

}
