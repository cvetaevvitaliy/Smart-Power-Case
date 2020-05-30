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
void Settings_Set_BQ27441_Set_Capacity(uint16_t capacity);
void Settings_Set_BQ27441_Set_Min_Liion_Volt(uint16_t volt);
void Settings_Set_BQ27441_Set_Max_Liion_Volt(uint16_t volt);


#endif //FATSHARK_POWER_V1_SETTINGS_EEPROM_H

