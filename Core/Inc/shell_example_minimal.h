#ifndef SHELL_EXAMPLE_MINIMAL_H
#define SHELL_EXAMPLE_MINIMAL_H

// #include "stm32l0xx_hal_def.h"
// #include "stm32l0xx_hal_uart.h"
#include "stm32l0xx_hal.h"

void shell_example_minimal_init(UART_HandleTypeDef* phuart);
void shell_example_input_char(uint8_t inputData);

#endif /* SHELL_EXAMPLE_MINIMAL_H */