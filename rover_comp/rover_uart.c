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
    dbgOutputLoc(UART_INITIALIZE);
    /*****************************/

    UART_init();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_TEXT;
    uartParams.readDataMode = UART_DATA_TEXT;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 9600;

    /*****************************/
    dbgOutputLoc(UART_OPENING);
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
    struct motorData curData = {0,0,0};

    while(1)
    {
        bool outVal = receiveFromMotorsQ(&curData);

        if (!outVal)
            stop_all(FAIL_UART_RECEIVE);
        
        dbgOutputLoc(UART_WRITING);

        const char checkSum[1] = {(curData.address[0] + curData.command[0] + curData.speed[0]) & 0b01111111};

        UART_write(uart, curData.address, 1);
        UART_write(uart, curData.command, 1);
        UART_write(uart, curData.speed, 1);
        UART_write(uart, checkSum, 1);
    }
}
