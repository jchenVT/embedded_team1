/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
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
 *  ======== main_freertos.c ========
 */
#include <stdint.h>

#ifdef __ICCARM__
#include <DLib_Threads.h>
#endif

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>

/* Driver configuration */
#include <ti/drivers/Board.h>

#include <rover_main.h>
#include <rover_spi.h>
#include <rover_uart.h>
#include <rover_queues.h>
#include <rover_debug.h>

extern void *mainRoverThread(void *arg0);
extern void *uartThread(void *arg0);
extern void *spiThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE   1024

/*
 *  ======== main ========
 */
int main(void)
{

    pthread_t           mainControlsThread;
    pthread_t           motorThread;
    pthread_t           encoderThread;
    pthread_t           mqttRecvThread;
    pthread_t           mqttSendThread;

    pthread_attr_t      mainControlsAttrs;
    pthread_attr_t      motorAttrs;
    pthread_attr_t      encoderAttrs;
    pthread_attr_t      mqttRecvAttrs;
    pthread_attr_t      mqttSendAttrs;

    int                 retcMainControls;
    int                 retcMotors;
    int                 retcEncoders;
    int                 retcMQTTRecv;
    int                 rectMQTTSend;

    /* initialize the system locks */
#ifdef __ICCARM__
    __iar_Initlocks();
#endif

    /* Call driver init functions */
    Board_init();
    debug_setup();
    uart_setup();

    if (!createMotorQ()) {
        stop_all(FAIL_MotorQ_INIT);
    }
    if (!createEncoderQ()) {
        stop_all(FAIL_EncoderQ_INIT);
    }
    if (!createMQTTReceiveQ()) {
        stop_all(FAIL_MQTTRecvQ_INIT);
    }
    if (!createMQTTSendQ()) {
        stop_all(FAIL_MQTTSendQ_INIT);
    }

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&mainControlsAttrs);
    pthread_attr_init(&motorAttrs);
    pthread_attr_init(&encoderAttrs);
    //pthread_attr_init(&mqttRecvAttrs);
    //pthread_attr_init(&mqttSendAttrs);

    retcMainControls = pthread_create(&mainControlsThread, &mainControlsAttrs, mainRoverThread, NULL);
    retcMotors = pthread_create(&motorThread, &motorAttrs, uartThread, NULL);
    retcEncoders = pthread_create(&encoderThread, &encoderAttrs, spiThread, NULL);
    //retcMQTTRecv = pthread_create(&mqttRecvThread, &mqttRecvAttrs, mqttRecvThread, NULL);
    //rectMQTTSend = pthread_create(&mqttSendThread, &mqttSendAttrs, mqttSendThread, NULL);


    if (retcMainControls != 0) {
        stop_all(FAIL_MainThread_INIT);
    }
    if (retcMotors != 0) {
        stop_all(FAIL_MotorsThread_INIT);
    }

    if (retcEncoders != 0) {
        stop_all(FAIL_EncoderThread_INIT);
    }
/*
    else if (rectMQTTRecv != 0) {
        stop_all(FAIL_MQTTRecvThread_INIT);
    }
    else if (rectMQTTSend != 0) {
        stop_all(FAIL_MQTTSendThread_INIT);
    }
     */

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return (0);
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
