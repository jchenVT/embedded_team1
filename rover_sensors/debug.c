#include "debug.h"

UART_Handle uart = NULL;
UART_Params uart_debug_params;
static QueueHandle_t uart_debug_q= NULL;

void debug_setup()
{
    GPIO_init();
    GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_1, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_2, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_3, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_4, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_5, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_6, GPIO_CFG_OUTPUT);
    GPIO_setConfig(CONFIG_GPIO_7, GPIO_CFG_OUTPUT);

    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_1, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_2, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_3, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_4, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_5, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_6, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_7, GPIO_CFG_OUT_LOW);

    /*****************************/
    dbgOutputLoc(UART_INITIALIZE);
    /*****************************/

    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
    UART_Params_init(&uart_debug_params);
    uart_debug_params.writeDataMode = UART_DATA_TEXT;
    uart_debug_params.readDataMode = UART_DATA_TEXT;
    uart_debug_params.readReturnMode = UART_RETURN_FULL;
    uart_debug_params.readEcho = UART_ECHO_OFF;
    uart_debug_params.baudRate = 9600;

    /*****************************/
    dbgOutputLoc(UART_OPENING);
    /*****************************/

    uart = UART_open(CONFIG_UART_0, &uart_debug_params);
    if (uart == NULL)
        stop_all(FAIL_UART_INIT);

    // creating uart queue
    uart_debug_q = xQueueCreate( 64, 51);
    if (uart_debug_q == NULL) stop_all(FAIL_UART_INIT);
}

void dbgOutputLoc(unsigned int outLoc)
{
    if (outLoc > 0x7B)
        stop_all(FAIL_UNKNOWN_CODE);
    
    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_1, outLoc & 0x01 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_2, outLoc & 0x02 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_3, outLoc & 0x04 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_4, outLoc & 0x08 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_5, outLoc & 0x10 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_6, outLoc & 0x20 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_7, outLoc & 0x40 ? GPIO_CFG_OUT_HIGH : GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_0, GPIO_CFG_OUT_HIGH);
}

void dbgUARTVal(unsigned char outVal)
{
    /*****************************
    if (uart == NULL)
        stop_all(FAIL_UART_INIT);

    const char e[0] = outVal;
    //snprintf(e, 5, "%u\n", outVal);

    dbgOutputLoc(UART_WRITING); 

    UART_write(uart, e, sizeof(e));

    *****************************/
    xQueueSendToBackFromISR( uart_debug_q, &outVal, 0);
}

void stop_all(unsigned int FAILURE_CODE)
{
    //taskDISABLE_INTERRUPTS();
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /*****************************/
    dbgOutputLoc(UART_CLOSING);
    /*****************************/
    UART_close(uart);
    /*****************************/
    dbgOutputLoc(FAILURE_CODE);
    /*****************************/

    while(1)
    {
        GPIO_toggle(CONFIG_GPIO_LED_0);
        // blink LED forever
        int i = 0;
        for (;i<100000;i++) ;
    }
}


void *uartThread(void *arg0)
{
    struct uart_message msg;

    while(1)
    {

        xQueueReceive( uart_debug_q, &msg, portMAX_DELAY );

        dbgOutputLoc(UART_WRITING); 

        UART_write(uart, msg.msg, msg.array_len);
    }
}
