/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#ifndef FATSHARK_POWER_V1_POWER_H
#define FATSHARK_POWER_V1_POWER_H
#include "main.h"
#include "stm32f1xx_hal.h"
#include "main_app.h"

void Power_OLEDOn(bool state);
void Power_BoostEnable(bool state);
void Power_BoostEnable12V(bool state);
void Power_USBEnable(bool state);
void Power_USBResetGPIO(void );
void Power_SystemOn(bool state);

void Power_BatteryTask(Device_Status_t *Data);

bool Power_ChargerInit(void);
void Power_ChargerTask(ChargeChip_t *Data);


void Power_DevicePowerOffTimer(const Device_Status_t *Data);
void Power_Off(void);

#endif //FATSHARK_POWER_V1_POWER_H
