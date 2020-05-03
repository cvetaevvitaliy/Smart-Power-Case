/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_POWER_H
#define FATSHARK_POWER_V1_POWER_H
#include "main.h"
#include "stm32f1xx_hal.h"
#include "main_app.h"

void Power_OLED_On(bool state);
void Power_Boost_Enable(bool state);
void Power_Boost_Enable_12V(bool state);
void Power_USB_Enable(bool state);
void Power_System_On(bool state);


#endif //FATSHARK_POWER_V1_POWER_H
