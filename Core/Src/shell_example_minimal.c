#include <string.h>
#include <stdio.h>
#include "lwshell/lwshell.h"
#include "shell_example_minimal.h"
#include "main.h"
#include "battery_tester.h"

void lwshell_usart_printer(const char* str, struct lwshell* lw);

/* Command to get called */
int32_t
mycmd_fn(int32_t argc, char** argv) {
    printf("mycmd_fn called. Number of argv: %d\r\n", (int)argc);
    for (int32_t i = 0; i < argc; ++i) {
        printf("ARG[%d]: %s\r\n", (int)i, argv[i]);
    }

    /* Successful execution */
    return 0;
}

int32_t help(int32_t argc, char** argv)
{
    lwshell_usart_printer("\r\nCreature Battery Tester online (=^-ω-^=)\r\n", 0);
    return lwshellOK;
}


int32_t shell_cmd_start(int32_t argc, char** argv)
{
    BatteryTester_Start();
    return lwshellOK;
}

int32_t shell_cmd_stop(int32_t argc, char** argv)
{
    BatteryTester_Stop();
    return lwshellOK;
}

UART_HandleTypeDef* m_phuart;

void shell_example_init(UART_HandleTypeDef* phuart)
{
    m_phuart = phuart;
    lwshell_init();
    lwshell_set_output_fn(lwshell_usart_printer);
}

void shell_example_input_char(uint8_t inputData)
{
    lwshell_input((const char*) &inputData, sizeof(uint8_t));
}

void lwshell_usart_printer(const char* str, struct lwshell* lw)
{
    HAL_UART_Transmit(m_phuart, (uint8_t*) str, strlen(str), 100);
}

/* Example code */
void
shell_example_minimal_init(UART_HandleTypeDef* phuart) {
    // const char* input_str = "mycmd param1 \"param 2 with space\"";

    /* Init library */
    shell_example_init(phuart);

    /* Define shell commands */
    // lwshell_register_cmd("mycmd", mycmd_fn, "Adds 2 integer numbers and prints them");
    lwshell_register_cmd("help", help, "Help");
    lwshell_register_cmd("start", shell_cmd_start, "Start battery test");
    lwshell_register_cmd("stop", shell_cmd_stop, "Stop battery test");

    lwshell_usart_printer("\r\nCreature Battery Tester online (=^-ω-^=)\r\n", 0);

    /* User input to process every character */

    /* Now insert input */
    // lwshell_input(input_str, strlen(input_str));
}
