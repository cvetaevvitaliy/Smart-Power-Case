/********************************
* Created by Vitaliy on 5/28/20.
********************************/
#include "Settings_Eeprom.h"
#include "bq27441.h"

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
        BQ27441_setDesignEnergy((capacity * 3.7f));  // 3.7f bq27441-G1A 3.8f bq27441-G1B
 /***********************************************************
 *  https://datasheetspdf.com/datasheet/ICR18650-26F.html
 *  Samsung ICR18650-26F: Li-ion type 18650
 *  Nominal Capacity            2600mAh
 *  Charging Voltage            4.2 ±0.05 V
 *  Nominal Voltage             3.7V
 *  Discharge Cut-off Voltage   2.75V
 *  Max. Charge Current         2600mA
 *  Max. Discharge Current      5200mA
 ***********************************************************/
        BQ27441_setTaperRateTime(capacity / ( 0.1f * TAPER_CURRENT ) );
        BQ27441_setTaperRateVoltage(MAX_VOLTAGE_LIION - 90); // The Taper Voltage threshold defines the minimum voltage necessary as a qualifier for detection of charge termination.
        BQ27441_setChargeVChgTermination(MAX_VOLTAGE_LIION);
        BQ27441_setTerminateVoltageMin(2950);
 /**************************************************
 *  todo Discharge Cut-off voltage 2950 ? 2750
 *  needs to be fully charged and discharged battery
 *  and and compare capacity
 ****************************************************/

      
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

