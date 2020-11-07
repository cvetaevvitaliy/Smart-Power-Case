/********************************
* Created by Vitaliy on 5/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_SETTINGS_EEPROM_H
#define FATSHARK_POWER_V1_SETTINGS_EEPROM_H
#include "eeprom.h"
#include "main_app.h"
#include "Power.h"

void Settings_Get(Device_Settings_t *Data);
void Settings_Set(Device_Settings_t *Data);
void Settings_SetDefault(Device_Settings_t *Data);
void Settings_SetBQ27441SetCapacity(uint16_t capacity);
void Settings_SetMinVoltPowerOff(uint16_t volt);

#endif //FATSHARK_POWER_V1_SETTINGS_EEPROM_H

