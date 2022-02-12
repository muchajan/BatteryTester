#include "BatteryTester.hpp"
#include "stm32l0xx_ll_adc.h"

void BatteryTester::Init()
{
    // BatteryTester::m_tick_ms = 0;
    // BatteryTester::m_nextTick_ms = 0;

    // batteryVoltageSampleBuffer = RingBuffer<SAMPLE_BUFFER_SIZE>();

    // batteryVoltageSampleBuffer = RingBuffer();

    LL_ADC_StartCalibration(ADC1);
    while (LL_ADC_IsCalibrationOnGoing(ADC1))
    {
    }

    LL_ADC_Enable(ADC1);
    while (!(LL_ADC_IsActiveFlag_ADRDY(ADC1)))
    {
    }
}

void BatteryTester::Task()
{
    uint32_t tick_ms = HAL_GetTick();
    if (m_tick_ms >= m_nextTick_ms)
    {

        uint8_t timeString[12];
        uint8_t voltageString[10];

        // itoa(tick_ms, (char*) timeString, 10);
        // itoa(adc_chan0_mV, (char*) voltageString, 10);

        // HAL_UART_Transmit(&huart2, timeString, strlen((const char *)timeString), 100);
        // HAL_UART_Transmit(&huart2, (uint8_t *)&", ", 2, 100);
        // HAL_UART_Transmit(&huart2, voltageString, strlen((const char *)voltageString), 100);
        // HAL_UART_Transmit(&huart2, (uint8_t *)&"\r\n", 2, 100);

        m_nextTick_ms = tick_ms + LOG_SAMPLE_PERIOD_ms;
    }
}

void BatteryTester::Smaple()
{
    LL_ADC_REG_StartConversion(ADC1);
    while (!LL_ADC_IsActiveFlag_EOC(ADC1))
    {
    }
    uint32_t adc_chan0_val = LL_ADC_REG_ReadConversionData32(ADC1);

    LL_ADC_REG_StartConversion(ADC1);
    while (!LL_ADC_IsActiveFlag_EOC(ADC1))
    {
    }
    uint32_t adc_chan1_val = LL_ADC_REG_ReadConversionData32(ADC1);

    LL_ADC_REG_StartConversion(ADC1);
    while (!(LL_ADC_IsActiveFlag_EOC(ADC1) && LL_ADC_IsActiveFlag_EOS(ADC1)))
    {
    }
    uint32_t adc_vrefint_val = LL_ADC_REG_ReadConversionData32(ADC1);

    uint32_t psuVoltage_mV = __LL_ADC_CALC_VREFANALOG_VOLTAGE(adc_vrefint_val, LL_ADC_RESOLUTION_12B);
    uint32_t batteryVoltage_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(psuVoltage_mV, adc_chan0_val, LL_ADC_RESOLUTION_12B);
    uint32_t resistorVoltage_mV = __LL_ADC_CALC_DATA_TO_VOLTAGE(psuVoltage_mV, adc_chan1_val, LL_ADC_RESOLUTION_12B);


    // batteryVoltageSampleBuffer[sampleBufferIndex] = batteryVoltage_mV;
    // resistorVoltageSampleBuffer[sampleBufferIndex] = resistorVoltage_mV;

    // batteryVoltageSampleBuffer.Add(batteryVoltage_mV);


    // sampleBufferIndex = (sampleBufferIndex + 1) % SAMPLE_BUFFER_SIZE;
}