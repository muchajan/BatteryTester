#ifndef BATTERY_TESTER_H
#define BATTERY_TESTER_H

#include "stdint.h"
#include "RingBuffer.hpp"

class BatteryTester
{
public:
    static void Init();
    static void Task();
    static void Smaple();

private:
    static uint32_t const LOG_SAMPLE_PERIOD_ms = 0;
    static uint32_t m_tick_ms;
    static uint32_t m_nextTick_ms;
    static uint32_t const SAMPLE_BUFFER_SIZE = 100;
    // static RingBuffer batteryVoltageSampleBuffer<SAMPLE_BUFFER_SIZE>();
    // constexpr static RingBuffer resistorVoltageSampleBuffer = RingBuffer<SAMPLE_BUFFER_SIZE>();
    // static RingBuffer resistorVoltageSampleBuffer;
};

#endif /* BATTERY_TESTER_H */