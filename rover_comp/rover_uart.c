#include <rover_uart.h>

/* TODO:
 *
 * check uart parameters for motor controller
 *
 */

static UART_Handle uart = NULL;
static UART_Params uartParams;

void uart_setup()
{
    /*****************************/
    dbgOutputLoc(UART_MOTOR_INITIALIZE);
    /*****************************/

    UART_init();
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    /*****************************/
    dbgOutputLoc(UART_MOTOR_OPENING);
    /*****************************/

    uart = UART_open(CONFIG_UART_0, &uartParams);
    if (uart == NULL)
        stop_all(FAIL_UART_INIT);

}

void uart_close() {
    /*****************************/
    dbgOutputLoc(UART_CLOSING);
    /*****************************/
    UART_close(uart);
}

// Output UART to rover motors
void *uartThread(void *arg0)
{
    struct motorData uartData = {0,0,0};

    while(1)
    {
        bool outVal = receiveFromMotorsQ(&uartData);

        if (!outVal)
            stop_all(FAIL_UART_RECEIVE);
        
        dbgOutputLoc(UART_MOTOR_WRITING);

        const char checkSum[1] = {(uartData.address + uartData.command + uartData.speed) & 0b01111111};

        UART_write(uart, &uartData.address, 1);
        UART_write(uart, &uartData.command, 1);
        UART_write(uart, &uartData.speed, 1);
        UART_write(uart, checkSum, 1);
    }
}
