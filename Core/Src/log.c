#include "log.h"
#include "string.h"

static const uint16_t TRANSMIT_TIMEOUT = 100;
static UART_HandleTypeDef* m_huart;

void Log_Init(UART_HandleTypeDef* huart)
{
    m_huart = huart;
}

void Log_Print(uint8_t* pBuffer)
{
    HAL_UART_Transmit(m_huart, pBuffer, strlen((const char*)pBuffer), TRANSMIT_TIMEOUT);
}