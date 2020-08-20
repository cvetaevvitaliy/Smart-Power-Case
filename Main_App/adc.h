/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_ADC_H
#define FATSHARK_POWER_V1_ADC_H
#include "main.h"
#include "stm32f1xx_hal.h"
#include "main_app.h"

bool ADC_Init(void);
void ADC_Task(ADC_Voltage_Data_t *Data);

#endif //FATSHARK_POWER_V1_ADC_H
