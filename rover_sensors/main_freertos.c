/* Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
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

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include "FreeRTOS.h"
#include "task.h"

/* TI-RTOS Header files */
#include <ti/drivers/Board.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>

/* TI-DRIVERS Header files */
#include "ti_drivers_config.h"

#include "debug.h"
#include "queues.h"
#include "spi_pixy.h"
#include "uart_lidar.h"
#include "uart_term.h"
#include "mqtt_queue.h"
#include "star.h"

extern void * mainThread(void *arg0);
extern void * starThread(void *arg0);
extern void *uartLidarThread(void *arg0);
extern void *spiThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE   2048

/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t pmainThread, pstarThread, plidarThread, pspiThread;
    pthread_attr_t mainAttrs, starAttrs, lidarAttrs, spiAttrs;
    struct sched_param mainParam, starParam, lidarParam, spiParam;
    int retc;
    int detachState;

    /* Call board init functions */
    Board_init();
    GPIO_init();

    createQs();
    queues_init();

    SPI_init();

    UART_Handle tUartHndl;

    /*Configure the UART                                                     */
    tUartHndl = InitTerm();
    /*remove uart receive from LPDS dependency                               */
    UART_control(tUartHndl, UART_CMD_RXDISABLE, (void*)0);
    uart_lidar_init();


    /* Set priority and stack size attributes */
    pthread_attr_init(&mainAttrs);
    mainParam.sched_priority = 1;
    pthread_attr_init(&starAttrs);
    starParam.sched_priority = 1;
    pthread_attr_init(&lidarAttrs);
    lidarParam.sched_priority = 2;
    pthread_attr_init(&spiAttrs);
    spiParam.sched_priority = 3;


    detachState = PTHREAD_CREATE_DETACHED;

    retc = pthread_attr_setdetachstate(&mainAttrs, detachState);
    retc |= pthread_attr_setdetachstate(&starAttrs, detachState);
    retc |= pthread_attr_setdetachstate(&lidarAttrs, detachState);
    retc |= pthread_attr_setdetachstate(&spiAttrs, detachState);
    if(retc != 0)
    {
        UART_PRINT("retc 1 fail :c");
        /* pthread_attr_setdetachstate() failed */
        while(1) {}
    }


    pthread_attr_setschedparam(&mainAttrs, &mainParam);
    pthread_attr_setschedparam(&starAttrs, &starParam);
    pthread_attr_setschedparam(&lidarAttrs, &lidarParam);
    pthread_attr_setschedparam(&spiAttrs, &spiParam);


    retc |= pthread_attr_setstacksize(&mainAttrs, THREADSTACKSIZE);
    retc |= pthread_attr_setstacksize(&starAttrs, THREADSTACKSIZE);
    retc |= pthread_attr_setstacksize(&lidarAttrs, THREADSTACKSIZE);
    retc |= pthread_attr_setstacksize(&spiAttrs, THREADSTACKSIZE);

    if(retc != 0)
    {
        UART_PRINT("retc 2 fail :c");
        /* pthread_attr_setstacksize() failed */
        while(1)
        {}
    }

    UART_PRINT("create main and star thread");

    retc |= pthread_create(&pmainThread, &mainAttrs, mainThread, (void*)0);
    // retc |= pthread_create(&pstarThread, &starAttrs, starThread, (void*)0);
    retc |= pthread_create(&plidarThread, &lidarAttrs, uartLidarThread, (void*)0);
    retc |= pthread_create(&pspiThread, &spiAttrs, spiThread, (void*)0);

    if(retc != 0)
    {
        UART_PRINT("retc 3 fail :c");
        /* pthread_create() failed */
        while(1){}
    }

    /*

    pthread_t           thread1, thread2;
    pthread_attr_t      attrs1, attrs2;
    pthread_attr_init(&attrs1);
    pthread_attr_init(&attrs2);

    UART_PRINT("create uart and spi thread");
    retc |= pthread_create(&thread1, &attrs1, uartLidarThread, NULL);
    retc |= pthread_create(&thread2, &attrs2, spiThread, NULL);

    if (retc != 0) {
        UART_PRINT("retc 4 fail :c");
        while (1) {}
    }
    */

    UART_PRINT("all threads made");

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
void vApplicationStackOverflowHook(TaskHandle_t pxTask,
                                   char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}

void vApplicationTickHook(void)
{
    /*
     * This function will be called by each tick interrupt if
     * configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
     * added here, but the tick hook is called from an interrupt context, so
     * code must not attempt to block, and only the interrupt safe FreeRTOS API
     * functions can be used (those that end in FromISR()).
     */
}

void vPreSleepProcessing(uint32_t ulExpectedIdleTime)
{
}

//*****************************************************************************
//
//! \brief Application defined idle task hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void
vApplicationIdleHook(void)
{
    /* Handle Idle Hook for Profiling, Power Management etc */
}

//*****************************************************************************
//
//! \brief  Overwrite the GCC _sbrk function which check the heap limit related
//!         to the stack pointer.
//!         In case of freertos this checking will fail.
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
#if defined (__GNUC__)
void * _sbrk(uint32_t delta)
{
    extern char _end;     /* Defined by the linker */
    extern char __HeapLimit;
    static char *heap_end;
    static char *heap_limit;
    char *prev_heap_end;

    if(heap_end == 0)
    {
        heap_end = &_end;
        heap_limit = &__HeapLimit;
    }

    prev_heap_end = heap_end;
    if(prev_heap_end + delta > heap_limit)
    {
        return((void *) -1L);
    }
    heap_end += delta;
    return((void *) prev_heap_end);
}

#endif
