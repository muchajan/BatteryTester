#ifndef BATTERY_TESTER_H
#define BATTERY_TESTER_H

#include "stdint.h"

void BatteryTester_Init(void);
void BatteryTester_Task(void);
int8_t BatteryTester_Start(void);
void BatteryTester_Stop(void);

#endif /* BATTERY_TESTER_H */