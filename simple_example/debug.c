#include "debug.h"


void debug_setup()
{
    GPIO_init();
}

void dbgUARTVal(unsigned char outVal)
{
    static UART_Handle uart;
    static UART_Params uartParams;
    static bool initialized = false;

    if (!initialized)
    {
        UART_init();
        UART_Params_init(&uartParams);
        uartParams.writeDataMode = UART_DATA_BINARY;
        uartParams.readDataMode = UART_DATA_BINARY;
        uartParams.readReturnMode = UART_RETURN_FULL;
        uartParams.readEcho = UART_ECHO_OFF;
        uartParams.baudRate = 115200;
        
        uart = UART_open(CONFIG_UART_0, &uartParams);
        if (uart == NULL) // uart failed to initialize
            stop_all();

        initialized = true;
    }
    
    UART_write(uart, outVal, sizeof(outVal));
}

void stop_all()
{
    taskDISABLE_INTERRUPTS();
    while(1)
    {
        // blink LED forever                
    }
}
