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

    // one-time initialization of UART driver
    UART_init();

    // initialize UART parameters
    UART_Params params;
    UART_Params_init(&params);
    params.baudRate = 9600;
    params.readMode = UART_MODE_BLOCKING;
    params.readTimeout = UART_WAIT_FOREVER;

    // open the UART
    UART_Handle uart;
    uart = UART_open(CONFIG_UART_0, &params);

    // check if UART is open
    if (uart == NULL)
        stop_all(FAIL_UART_INIT);

    int32_t readCount;
    uint8_t buffer[8];

    while (1) {

        // Read from the UART
        readCount = UART_read(uart, buffer, 8);

        // error handle


        // send to queue
        sendToProx1Q(readCount);
    }
}
