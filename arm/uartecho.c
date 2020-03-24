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

uint16_t angleXToPWM(uint16_t x) {
    uint16_t degreePWM = (X_HIGH - X_LOW) / 180;
    return x * degreePWM + X_LOW;
}

uint16_t angleYToPWM(uint16_t y) {
    uint16_t degreePWM = (Y_HIGH - Y_LOW) / 180;
    return y * degreePWM + Y_LOW;
}

uint16_t angleZToPWM(uint16_t z) {
    uint16_t degreePWM = (Z_HIGH - Z_LOW) / 180;
    return z * degreePWM + Z_LOW;
}

uint16_t angleClawToPWM(uint16_t c) {
    uint16_t degreePWM = (CLAW_HIGH - CLAW_LOW) / 180;
    return c * degreePWM + CLAW_LOW;
}

uint16_t pwmXToAngle(uint16_t x) {
    uint16_t pwmDegree = 180 / (X_HIGH - X_LOW);
    return (x - X_LOW) * pwmDegree;
}

uint16_t pwmYToAngle(uint16_t y) {
    uint16_t pwmDegree = 180 / (Y_HIGH - Y_LOW);
    return (y - Y_LOW) * pwmDegree;
}

uint16_t pwmZToAngle(uint16_t z) {
    uint16_t pwmDegree = 180 / (Z_HIGH - Z_LOW);
    return (z - Z_LOW) * pwmDegree;
}

uint16_t pwmClawToAngle(uint16_t c) {
    uint16_t pwmDegree = 180 / (CLAW_HIGH - CLAW_LOW);
    return (c - CLAW_LOW) * pwmDegree;
}

void convertCurrentStructToPWM() {
    current_pos.yee_value = angleXToPWM(current_pos.yee_value) / 10 * 10;
    current_pos.haw_value = angleYToPWM(current_pos.haw_value) / 10 * 10;
    current_pos.cow_value = angleZToPWM(current_pos.cow_value) / 10 * 10;
    current_pos.boy_value = angleClawToPWM(current_pos.boy_value) / 10 * 10;
}

void convertGoalStructToPWM() {
    goal_pos.yee_value = angleXToPWM(goal_pos.yee_value) / 10 * 10;
    goal_pos.haw_value = angleYToPWM(goal_pos.haw_value) / 10 * 10;
    goal_pos.cow_value = angleZToPWM(goal_pos.cow_value) / 10 * 10;
    goal_pos.boy_value = angleClawToPWM(goal_pos.boy_value) / 10 * 10;
}

void convertToDegree() {
    goal_pos.yee_value = pwmXToAngle(goal_pos.yee_value) / 10 * 10;
    goal_pos.haw_value = pwmYToAngle(goal_pos.haw_value) / 10 * 10;
    goal_pos.cow_value = pwmZToAngle(goal_pos.cow_value) / 10 * 10;
    goal_pos.boy_value = pwmClawToAngle(goal_pos.boy_value) / 10 * 10;

}
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
    //timer10ms = xTimerCreate("10ms", pdMS_TO_TICKS(10), pdTRUE, NULL, movementCallback);
}

// checks for valid char and sends the corresponding message to the queue
bool validChar(char in) {
    movqData_t tosend;
    tosend = current_pos;
    switch (in) {
    case 'o':
        tosend = off_pos;
        break;
    case 's':
        tosend = start_pos;
        break;
    case 'c':
        if (current_pos.boy_value == CLAW_LOW) {
            tosend.boy_value = CLAW_HIGH;
        }
        else {
            tosend.boy_value = CLAW_LOW;
        }
        break;
    case 'l':
        tosend.yee_value = X_LOW;
        break;
    case 'r':
        tosend.yee_value = X_HIGH;
        break;
    case 'd':
        if (current_pos.cow_value == Z_HIGH) {
            tosend.cow_value = Z_LOW;
        }
        else {
            tosend.cow_value = Z_HIGH;
        }
        break;
    case 'f':
        if (current_pos.haw_value == Y_HIGH) {
            tosend.haw_value = Y_LOW;
        }
        else {
            tosend.haw_value = Y_HIGH;
        }
        break;
    case 'm':
        tosend.haw_value = 1500;
        break;
    default:
        return false;
    }
    return sendMsgToMovQ(tosend);

}
/*
 *  ========mainThread ========
 */
void *armDebugThread(void *arg0)
{
    char input;

    // set the current position of the arm to its off position
    current_pos = start_pos;
    // set the current goal position of the arm to the start position
    goal_pos = off_pos;

    /* Loop forever */
    while (1) {
        UART_read(uart, &input, 1);
        UART_write(uart, &input, 1);
        if (!validChar(input)) {
            continue;
        }

        uint8_t a;
        receiveFromAckQ(&a);

        char buf[2];
        snprintf(buf, 2, "%d\n", a);
        UART_write(uart, buf, 2);
    }
}

void *mainArmThread(void *arg0) {

    while (1) {
        movqData_t m;
        receiveFromMovQ(&m);

        char buf[40];
        int ret;
        ret = snprintf(buf, 40, "%d %d %d %d\r\n", m.yee_value, m.haw_value, m.cow_value, m.boy_value);
        UART_write(uart, buf, ret);

        sendMsgToAckQ(1);
    }
}

