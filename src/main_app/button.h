/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_BUTTON_H
#define FATSHARK_POWER_V1_BUTTON_H
#include "main.h"
#include "stm32f1xx_hal.h"
#include "main_app.h"
#include "Power.h"

enum Button_e{
    Button_menu,
    Button_select,
};


void Button_Task(const Device_Settings_t *Settings);
bool Button_GetState (enum Button_e button);

#endif //FATSHARK_POWER_V1_BUTTON_H
