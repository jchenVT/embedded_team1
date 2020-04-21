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
#include <mqtt_queue.h>

extern void *mainRoverThread(void *arg0);
extern void *uartThread(void *arg0);
extern void *spiThread(void *arg0);
extern void *mainMQTTThread(void *args);

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
    pthread_t           mqttThread;

    pthread_attr_t      mainControlsAttrs;
    pthread_attr_t      motorAttrs;
    pthread_attr_t      encoderAttrs;
    pthread_attr_t      mqttAttrs;

    int                 retcMainControls;
    int                 retcMotors;
    int                 retcEncoders;
    int                 retcMQTT;

    struct sched_param priParam;
    int detachState;

    /* initialize the system locks */
#ifdef __ICCARM__
    __iar_Initlocks();
#endif

    /* Call driver init functions */
    Board_init();
    debug_setup();
    spi_setup();
    uart_setup();
    createQs();

    if (!createMotorQ()) {
        stop_all(FAIL_MotorQ_INIT);
    }
    if (!createEncoderQ()) {
        stop_all(FAIL_EncoderQ_INIT);
    }
    if (!createMQTTReceiveQ()) {
        stop_all(FAIL_MQTTRecvQ_INIT);
    }

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&mainControlsAttrs);
    pthread_attr_init(&motorAttrs);
    pthread_attr_init(&encoderAttrs);
    pthread_attr_init(&mqttAttrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;

    retcMainControls = pthread_create(&mainControlsThread, &mainControlsAttrs, mainRoverThread, NULL);
    retcMotors = pthread_create(&motorThread, &motorAttrs, uartThread, NULL);
    retcEncoders = pthread_create(&encoderThread, &encoderAttrs, spiThread, NULL);

    retcMQTT = pthread_attr_setdetachstate(&mqttAttrs, detachState);
    pthread_attr_setschedparam(&mqttAttrs, &priParam);
    retcMQTT |= pthread_attr_setstacksize(&mqttAttrs, THREADSTACKSIZE);
    retcMQTT = pthread_create(&mqttThread, &mqttAttrs, mainMQTTThread, NULL);

    if (retcMainControls != 0) {
        stop_all(FAIL_MainThread_INIT);
    }
    if (retcMotors != 0) {
        stop_all(FAIL_MotorsThread_INIT);
    }

    if (retcEncoders != 0) {
        stop_all(FAIL_EncoderThread_INIT);
    }
    else if (retcMQTT != 0) {
        stop_all(FAIL_MQTTRecvThread_INIT);
    }


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
