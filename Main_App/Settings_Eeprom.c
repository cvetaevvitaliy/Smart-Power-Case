/********************************
* Created by Vitaliy on 5/28/20.
********************************/
#include "Settings_Eeprom.h"
#include "BQ27441.h"

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
    Data->Boost_mode = Boost_8V;
    Data->time_auto_off = 4440;
    Data->buzzer_enable = false;
    Data->Boost_mode = false;
    Settings_Set(Data);
}


void Settings_Set_BQ27441_Set_Capacity(uint16_t capacity){

    if (BQ27441_enterConfig(true)){
        printf("BQ27441_enterConfig\n");
        BQ27441_setDesignEnergy(capacity);
        BQ27441_setCapacity(capacity);
        if (BQ27441_exitConfig(true)){
            printf("BQ27441_exitConfig\n");
        } else
            printf("BQ27441_exitConfig false\n");
    } else
        printf("BQ27441_enterConfig false\n");

}


void Settings_Set_BQ27441_Set_Min_Liion_Volt(uint16_t volt){
    if (BQ27441_enterConfig(true)){
        printf("BQ27441_enterConfig\n");
        BQ27441_setTerminateVoltageMin(volt);
        if (BQ27441_exitConfig(true)){
            printf("BQ27441_exitConfig\n");
        } else
            printf("BQ27441_exitConfig false\n");
    } else
        printf("BQ27441_enterConfig false\n");

}

void Settings_Set_BQ27441_Set_Max_Liion_Volt(uint16_t volt){
    if (BQ27441_enterConfig(true)){
        printf("BQ27441_enterConfig\n");
        BQ27441_setTerminateVoltageMax(volt);
        if (BQ27441_exitConfig(true)){
            printf("BQ27441_exitConfig\n");
        } else
            printf("BQ27441_exitConfig false\n");
    } else
        printf("BQ27441_enterConfig false\n");

}