/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_OLED_UI_H
#define FATSHARK_POWER_V1_OLED_UI_H
#include "main.h"
#include "stm32f1xx_hal.h"
#include "main_app.h"
#include "Settings_Eeprom.h"
#include "Power.h"

#include <tinyprintf.h>


#define POSITION_BATTERY_X                  97
#define POSITION_BATTERY_Y                  1
#define POSITION_WORK_TIME_X                70
#define POSITION_WORK_TIME_Y                19

#define POSITION_TEXT_X                     3
#define POSITION_TEXT_Y                     10
#define POSITION_ICO_APPLY_X                95
#define POSITION_ICO_APPLY_Y                4
#define POSITION_SHIFT                      37

#define POSITION_ICONS                      30


void OLED_UI_Task(Device_Status_t *Data);


#endif //FATSHARK_POWER_V1_OLED_UI_H
