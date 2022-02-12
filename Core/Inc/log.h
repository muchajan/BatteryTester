#ifndef LOG_H
#define LOG_H

#include "stm32l0xx_hal.h"

void Log_Init(UART_HandleTypeDef* huart);
void Log_Print(uint8_t* pBuffer);

#endif /* LOG_H */