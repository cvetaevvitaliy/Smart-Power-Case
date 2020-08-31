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


void Settings_SetDefault(Device_Settings_t *Data) {
    Data->current_max = 2000;
    Data->low_volt = 332;
    Data->design_capacity = 5000;
    Data->Boost_mode = Boost_8V;
    Data->time_auto_off = 0;
    Data->buzzer_enable = false;
    Data->Boost_mode = false;
    Settings_Set(Data);
}


void Settings_SetBQ27441SetCapacity(uint16_t capacity){


    if (BQ27441_enterConfig(true)){
#ifdef USE_USB_DEBUG_PRINTF
        printf("BQ27441_enterConfig\n");
#endif
        BQ27441_setCapacity(capacity);
        BQ27441_setDesignEnergy((capacity * 3.7f));
        BQ27441_setTaperRateTime(capacity / ( 0.1f * TAPER_CURRENT ) );
        BQ27441_setTaperRateVoltage(4100);
        BQ27441_setTerminateVoltageMin(2850);

        if (BQ27441_exitConfig(true)){
#ifdef USE_USB_DEBUG_PRINTF
            printf("BQ27441_exitConfig\n");
#endif
        } else {
#ifdef USE_USB_DEBUG_PRINTF
            printf("BQ27441_exitConfig false\n");
#endif
        }
    } else {
#ifdef USE_USB_DEBUG_PRINTF
        printf("BQ27441_enterConfig false\n");
#endif
    }

}


void Settings_SetMinVoltPowerOff(uint16_t volt){
    bq2589x_set_bat_limit(volt);
}
