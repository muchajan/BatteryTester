#include "battery_tester.h"
#include "log.h"
#include "stm32l0xx_ll_adc.h"
#include "ring_buffer.h"
#include "stdlib.h"
#include "main.h"

static const uint32_t RESISTOR_VALUE_mohm = 8100U; //9800U;
static const uint32_t BATTERY_CUTOFF_mV = 1000U;

static const uint32_t LOG_SAMPLE_PERIOD_ms = 30000U;
static uint32_t m_nextTick_ms = 0U;

#define SAMPLE_BUFFER_SIZE 100U
#define RUN 1
#define STOP 0

static const uint8_t SANITIZE_BUFFER_ITERATIONS = 10U;
static uint32_t sampleBufferIndex = 0U;
static uint32_t batteryVoltageSampleBuffer[SAMPLE_BUFFER_SIZE];
static uint32_t resistorVoltageSampleBuffer[SAMPLE_BUFFER_SIZE];
static uint32_t batteryCapacity_uAh = 0U;

void Sample(void);
void SanitizeBuffer(uint32_t* pBuffer, const uint16_t bufferSize);
void SanitizeBuffers(void);
void GetMeasurements(uint32_t* batteryVoltage_mV, uint32_t* resistorVoltage_mV,
    uint32_t* batteryCurrent_uA);
void LogMeasurement(uint32_t time_ms, uint32_t batteryVoltage_mV,
    uint32_t resistorVoltage_mV, uint32_t batteryCurrent_uA, uint32_t batteryCapacity_uAh);
void CalibrateADC(void);

uint8_t running = STOP;

void BatteryTester_Init(void)
{
    CalibrateADC();
    batteryCapacity_uAh = 0;
}

int8_t BatteryTester_Start(void)
{
    uint32_t batteryVoltage_mV;
    uint32_t resistorVoltage_mV;
    uint32_t batteryCurrent_uA;

    if (running == RUN)
    {
        Log_Print("Already running");
        return -2;
    }

    HAL_Delay(3);

    for (uint16_t i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        Sample();
    }
    SanitizeBuffers();
    GetMeasurements(&batteryVoltage_mV, &resistorVoltage_mV, &batteryCurrent_uA);

    if (batteryVoltage_mV <= BATTERY_CUTOFF_mV)
    {
        Log_Print((uint8_t*)"Battery Level low");
        return -1;
    }

    Log_Print((uint8_t*)"Starting Battery Test\r\n");
    Log_Print((uint8_t*)"Time [ms], Battery Voltage [mV], Resistor Voltage [ms],\
     Resistor Current [uA]\r\n");

    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SWITCH0_GPIO_Port, SWITCH0_Pin, GPIO_PIN_SET);

    HAL_Delay(200);

    CalibrateADC();

    for (uint16_t i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        Sample();
    }

    SanitizeBuffers();
    GetMeasurements(&batteryVoltage_mV, &resistorVoltage_mV, &batteryCurrent_uA);
    LogMeasurement(HAL_GetTick(), batteryVoltage_mV, resistorVoltage_mV,
        batteryCurrent_uA, batteryCapacity_uAh);

    running = RUN;
    return 0;
}

void BatteryTester_Stop(void)
{
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SWITCH0_GPIO_Port, SWITCH0_Pin, GPIO_PIN_RESET);

    Log_Print("Test finished\r\n");
    running = STOP;
}

void BatteryTester_Task(void)
{
    if (running == STOP)
    {
        return;
    }

    Sample();

    uint32_t tick_ms = HAL_GetTick();
    if (tick_ms >= m_nextTick_ms)
    {
        SanitizeBuffers();

        uint32_t batteryVoltage_mV;
        uint32_t resistorVoltage_mV;
        uint32_t batteryCurrent_uA;

        GetMeasurements(&batteryVoltage_mV, &resistorVoltage_mV, &batteryCurrent_uA);

        batteryCapacity_uAh += batteryCurrent_uA / 120;

        LogMeasurement(tick_ms, batteryVoltage_mV, resistorVoltage_mV, batteryCurrent_uA, batteryCapacity_uAh);

        m_nextTick_ms = tick_ms + LOG_SAMPLE_PERIOD_ms;

        if (batteryVoltage_mV < BATTERY_CUTOFF_mV)
        {
            BatteryTester_Stop();
        }
    }
}

void Sample(void)
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

    batteryVoltageSampleBuffer[sampleBufferIndex] = batteryVoltage_mV;
    resistorVoltageSampleBuffer[sampleBufferIndex] = resistorVoltage_mV;

    sampleBufferIndex = (sampleBufferIndex + 1) % SAMPLE_BUFFER_SIZE;
}

void SanitizeBuffer(uint32_t* pBuffer, const uint16_t bufferSize)
{
    for (uint8_t i = 0; i < SANITIZE_BUFFER_ITERATIONS; i++)
    {
        uint16_t minIndex = RingBuffer_FindMin(pBuffer, bufferSize);
        RingBuffer_ReplaceElementByNeighbourAverage(pBuffer, bufferSize, minIndex);

        uint16_t maxIndex = RingBuffer_FindMax(pBuffer, bufferSize);
        RingBuffer_ReplaceElementByNeighbourAverage(pBuffer, bufferSize, maxIndex);
    }
}

void SanitizeBuffers(void)
{
    SanitizeBuffer(batteryVoltageSampleBuffer, SAMPLE_BUFFER_SIZE);
    SanitizeBuffer(resistorVoltageSampleBuffer, SAMPLE_BUFFER_SIZE);
}

void GetMeasurements(uint32_t* batteryVoltage_mV, uint32_t* resistorVoltage_mV,
    uint32_t* batteryCurrent_uA)
{
    *batteryVoltage_mV =
        RingBuffer_AverageValue(batteryVoltageSampleBuffer, SAMPLE_BUFFER_SIZE);
    *resistorVoltage_mV =
        RingBuffer_AverageValue(resistorVoltageSampleBuffer, SAMPLE_BUFFER_SIZE);
    *batteryCurrent_uA = ((*resistorVoltage_mV) * 1000000) / RESISTOR_VALUE_mohm;
}

void LogMeasurement(uint32_t time_ms, uint32_t batteryVoltage_mV,
    uint32_t resistorVoltage_mV, uint32_t batteryCurrent_uA, uint32_t batteryCapacity_uAh)
{
    uint8_t timeString[12];
    uint8_t batteryVoltageString[11];
    uint8_t resistorVoltageString[11];
    uint8_t batteryCurrentString[11];
    uint8_t batteryCapacityString[11];

    itoa(time_ms, (char*)timeString, 10);
    itoa(batteryVoltage_mV, (char*)batteryVoltageString, 10);
    itoa(resistorVoltage_mV, (char*)resistorVoltageString, 10);
    itoa(batteryCurrent_uA, (char*)batteryCurrentString, 10);
    itoa(batteryCapacity_uAh, (char*)batteryCapacityString, 10);

    Log_Print(timeString);
    Log_Print((uint8_t*)&", ");
    Log_Print(batteryVoltageString);
    Log_Print((uint8_t*)&", ");
    Log_Print(resistorVoltageString);
    Log_Print((uint8_t*)&", ");
    Log_Print(batteryCurrentString);
    Log_Print((uint8_t*)&", ");
    Log_Print(batteryCapacityString);
    Log_Print((uint8_t*)&"\r\n");
}

void CalibrateADC(void)
{
    if (LL_ADC_IsEnabled(ADC1))
    {
        LL_ADC_Disable(ADC1);
        while (LL_ADC_IsEnabled(ADC1))
        {
        }
    }

    LL_ADC_StartCalibration(ADC1);
    while (LL_ADC_IsCalibrationOnGoing(ADC1))
    {
    }

    LL_ADC_Enable(ADC1);
    while (!(LL_ADC_IsActiveFlag_ADRDY(ADC1)))
    {
    }
}