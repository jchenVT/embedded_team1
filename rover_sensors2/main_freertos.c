/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>

#include "debug.h"
#include "queues.h"
#include "spi_pixy.h"
#include "uart_debug.h"
#include "uart_lidar.h"
#include "mqtt_queue.h"

extern void * mainThread(void *arg0);
extern void * starThread(void *arg0);
extern void *uartDebugThread(void *arg0);
extern void *uartLidarThread(void *arg0);
extern void *spiThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE   1024

/*
 *  ======== main ========
 */
int main(void)
{
    /* initialize the system locks */
#ifdef __ICCARM__
    __iar_Initlocks();
#endif

    int retc;

    /* Call driver init functions */
    Board_init();
    GPIO_init();
    Timer_init();
    UART_init();
    SPI_init();
    debug_init();
    queues_init();
    uart_debug_init();
    uart_lidar_init();



    pthread_t           thread1, thread2, thread3;
    pthread_attr_t      attrs1, attrs2, attrs3;

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs1);
    pthread_attr_init(&attrs2);
    pthread_attr_init(&attrs3);

    retc  = pthread_create(&thread1, &attrs1, uartDebugThread, NULL);
    retc |= pthread_create(&thread2, &attrs2, uartLidarThread, NULL);
    retc |= pthread_create(&thread3, &attrs3, spiThread, NULL);

    if (retc != 0) {
        /* pthread_create() failed */
        while (1) {}
    }


    /*************/
    // This is the mqtt stuff
    
    /*
    pthread_t thread;
    pthread_t sThread;
    pthread_attr_t pAttrs;
    pthread_attr_t sAttrs;
    struct sched_param priParam;
    struct sched_param starParam;
    int starRetc;
    int detachState;

    createQs();

    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;
    pthread_attr_init(&sAttrs);
    priParam.sched_priority = 1;

    detachState = PTHREAD_CREATE_DETACHED;
    retc = pthread_attr_setdetachstate(&pAttrs, detachState);
    starRetc = pthread_attr_setdetachstate(&sAttrs, detachState);

    if(retc != 0)
    {
        // pthread_attr_setdetachstate() failed 
        while(1)
        {
            ;
        }
    }

    pthread_attr_setschedparam(&pAttrs, &priParam);
    pthread_attr_setschedparam(&sAttrs, &starParam);


    retc |= pthread_attr_setstacksize(&pAttrs, THREADSTACKSIZE);
    starRetc |= pthread_attr_setstacksize(&sAttrs, THREADSTACKSIZE/4);
    if(retc != 0)
    {
        // pthread_attr_setstacksize() failed
        while(1)
        {
            ;
        }
    }

    retc = pthread_create(&thread, &pAttrs, mainThread, (void*)0);
    starRetc = pthread_create(&sThread, &sAttrs, starThread, (void*)0);

    if(retc != 0)
    {
        // pthread_create() failed 
        while(1)
        {
            ;
        }
    }
    */

    /*************/


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
