/********************************
* Created by Vitaliy on 5/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_SETTINGS_EEPROM_H
#define FATSHARK_POWER_V1_SETTINGS_EEPROM_H
#include "eeprom.h"
#include "main_app.h"

void Settings_Get(Device_Settings_t *Data);
void Settings_Set(Device_Settings_t *Data);
void Settings_Set_Default(Device_Settings_t *Data);


#endif //FATSHARK_POWER_V1_SETTINGS_EEPROM_H

