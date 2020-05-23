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

void Power_Battery_Task(Device_Status_t *Data);

bool Power_Charger_Init(void);
void Power_Charger_Task(ChargeChip_t *Data);


void Power_Device_Off_On(Device_Status_t *Data);
void Power_Off(void);

#endif //FATSHARK_POWER_V1_POWER_H
