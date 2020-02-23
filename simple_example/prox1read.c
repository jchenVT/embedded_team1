/*
 *  prox1read.c
 *
 *  Created on: Feb 23, 2020
 *      Author: giffen
 */

#include "prox1read.h"

/*
 *  @function   readProximitySensor
 *              Main thread that will perform a blocking read on
 *              the UART0 pin to read the proximity sensor 1 data.
 *
 *  @params     arg0
 *  @return     void
 */
void *readProximity1Thread(void *arg0) {

    dbgOutputLoc(INIT_UART0);

    /* One-time initialization of UART driver */
    UART_init();

    /* Initialize UART parameters */
    UART_Params params;
    UART_Params_init(&params);
    params.baudRate = 9600;
    params.readMode = UART_MODE_BLOCKING;
    params.readTimeout = UART_WAIT_FOREVER;

    /* Open the UART */
    UART_Handle uart;
    uart = UART_open(CONFIG_UART_0, &params);

    /* Check if UART is open */
    if (uart == NULL)
        stop_all(FAILED_UART0_INIT);

    int32_t readCount;
    uint8_t buffer[8];

    while (1) {

        /* Read from the UART */
        dbgOutputLoc(WAITING_READ_UART0);
        readCount = UART_read(uart, buffer, 8);

        /* Error handle the value read */


        /* Send to queue */
        dbgOutputLoc(SEND_PROX1Q);
        sendToProx1Q(readCount);
    }
}
