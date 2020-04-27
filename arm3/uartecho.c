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

static movqData_t off_pos = {0, 1600, 1100, 1000, 800};
static movqData_t start_pos = {0, 1600, 1500, 1800, 800};

static movqData_t current_pos, goal_pos;


uint16_t angleXToPWM(uint16_t x) {
    uint16_t degreePWM = (X_HIGH - X_LOW) / 180;
    switch(x) {
    case 0:
        return X_LOW;
    case 90:
        return X_MIDDLE;
    case 180:
        return X_HIGH;
    }
    return x * degreePWM + X_LOW;
}

uint16_t angleYToPWM(uint16_t y) {
    uint16_t degreePWM = (Y_HIGH - Y_LOW) / 180;
    switch(y) {
    case 0:
        return Y_LOW;
    case 90:
        return Y_MIDDLE;
    case 180:
        return Y_HIGH;
    }
    return y * degreePWM + Y_LOW;
}

uint16_t angleZToPWM(uint16_t z) {
    uint16_t degreePWM = (Z_HIGH - Z_LOW) / 180;
    switch(z) {
    case 0:
        return Z_LOW;
    case 90:
        return Z_MIDDLE;
    case 180:
        return Z_HIGH;
    }
    return z * degreePWM + Z_LOW;
}

uint16_t angleClawToPWM(uint16_t c) {
    uint16_t degreePWM = (CLAW_HIGH - CLAW_LOW) / 180;
    switch(c) {
    case 0:
        return CLAW_LOW;
    case 90:
        return CLAW_MIDDLE;
    case 180:
        return CLAW_HIGH;
    }
    return c * degreePWM + CLAW_LOW;
}

void convertStructToPWM(movqData_t *deg) {
    deg->yee_value = angleXToPWM(deg->yee_value) / 10 * 10;
    deg->haw_value = angleYToPWM(deg->haw_value) / 10 * 10;
    deg->cow_value = angleZToPWM(deg->cow_value) / 10 * 10;
    deg->boy_value = angleClawToPWM(deg->boy_value) / 10 * 10;
}

// instantiates the uart for the arm messaging queue
void arm_init() {
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
    timer10ms = xTimerCreate("20ms", pdMS_TO_TICKS(20), pdTRUE, NULL, movementCallback);
}
/*
 *  ========mainThread ========
 */

int armDone() {
    return memcmp(&current_pos, &goal_pos, sizeof(current_pos)) == 0;
}an

void movementHelper() {
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

void *mainArmThread(void *arg0) {
    while (1) {
        movqData_t m;
        receiveFromMovQ(&m);
        char buf[25];
        snprintf(buf, 25, "%d %d %d %d %d\n\r", m.type, m.yee_value, m.haw_value, m.cow_value, m.boy_value);
//        UART_PRINT(buf);

        if (m.type) {
            movementHelper();
        }
        else {
            goal_pos = m;
            if (xTimerIsTimerActive(timer10ms) == pdFALSE) {
                xTimerStart(timer10ms, 0);
            }
        }

        if (armDone()) {
            UART_PRINT("arm done\n\r");
            sendMsgToAckQ(1);
            if (xTimerIsTimerActive(timer10ms) == pdTRUE) {
                xTimerStop(timer10ms, 0);
            }
        }
    }
}

void movementCallback(TimerHandle_t xTimer) {
    movqData_t m = {1, 0, 0, 0, 0};
    m.type = 1;
    sendMsgToMovQ(m);
}

void *readMQTTThread(void *arg0) {
    TimerHandle_t timerPub = xTimerCreate("PublishTimer", pdMS_TO_TICKS(5000), pdTRUE, NULL, timerCallback);
    xTimerStart(timerPub, 0);

    struct qArmPosMsg data = {};
    current_pos = off_pos;
    goal_pos = start_pos;

    while (1) {
        UART_PRINT("waiting for message\n\r");
        receiveFromSubArmPosQ(&data);
        UART_PRINT("received message\n\r");

        movqData_t recvQ = {0, data.xPos, data.yPos, data.zPos, data.clawPos};
        convertStructToPWM(&recvQ);
        sendMsgToMovQ(recvQ);

        uint8_t a;
        receiveFromAckQ(&a);
        UART_PRINT("received ack\n");

    }
}

void timerCallback(TimerHandle_t xTimer) {

}
