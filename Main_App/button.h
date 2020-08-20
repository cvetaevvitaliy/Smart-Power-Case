/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_BUTTON_H
#define FATSHARK_POWER_V1_BUTTON_H
#include "main.h"
#include "stm32f1xx_hal.h"
#include "main_app.h"
#include "Power.h"

void Button_Task(Button_t *Data, const Device_Settings_t *Settings, uint8_t* time);

#endif //FATSHARK_POWER_V1_BUTTON_H
