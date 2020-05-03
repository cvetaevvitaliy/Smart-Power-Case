/********************************
* Created by Vitaliy on 5/28/20.
********************************/
#include "Settings_Eeprom.h"

#define START_VIRTUAL_ADDRESS      1


void Settings_Get(Device_Settings_t *Data){
    EE_Read(START_VIRTUAL_ADDRESS + 0, (uint32_t *) &Data->current_max);
    EE_Read(START_VIRTUAL_ADDRESS + 1, (uint32_t *) &Data->low_volt);
    EE_Read(START_VIRTUAL_ADDRESS + 2, (uint32_t *) &Data->design_capacity);
    EE_Read(START_VIRTUAL_ADDRESS + 3, (uint32_t *) &Data->time_auto_off);
    EE_Read(START_VIRTUAL_ADDRESS + 4, (uint32_t *) &Data->buzzer_enable);
    EE_Read(START_VIRTUAL_ADDRESS + 5, (uint32_t *) &Data->Boost_mode);
}


void Settings_Set(Device_Settings_t *Data){
    EE_Write(START_VIRTUAL_ADDRESS + 0, (uint32_t) Data->current_max);
    EE_Write(START_VIRTUAL_ADDRESS + 1, (uint32_t) Data->low_volt);
    EE_Write(START_VIRTUAL_ADDRESS + 2, (uint32_t) Data->design_capacity);
    EE_Write(START_VIRTUAL_ADDRESS + 3, (uint32_t) Data->time_auto_off);
    EE_Write(START_VIRTUAL_ADDRESS + 4, (uint32_t) Data->buzzer_enable);
    EE_Write(START_VIRTUAL_ADDRESS + 5, (uint32_t) Data->Boost_mode);
}


void Settings_Set_Default(Device_Settings_t *Data) {
    Data->current_max = 666;
    Data->low_volt = 332;
    Data->design_capacity = 2540;
    Data->Boost_mode = Boost_12V;
    Data->time_auto_off = 4440;
    Data->buzzer_enable = false;
    Data->Boost_mode = true;
    Settings_Set(Data);
}