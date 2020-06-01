//
// Created by Vitaliy on 6/4/20.
//
#include "Debug.h"

void Debug_Task(Device_Status_t *Data){
#ifdef USE_USB_DEBUG_PRINTF
    static uint32_t time_task = 0;

    if (HAL_GetTick() - time_task > 500){
        uint32_t tmp = time_task;
        time_task = HAL_GetTick();
        clrscr(); /// clear console

        printf("variable time_delay_task = %lums\n\r", HAL_GetTick() - tmp);
        printf("Work Time = %dh:%dm:%ds\n\r",Data->work_time_hours,Data->work_time_minute,Data->work_time_second);
        printf("Battery_Info.temperature = %.2fC\n\r",Data->Battery_Info.temperature);
        printf("Battery_Info.Vbat = %.2fV\n\r",Data->Battery_Info.Vbat);
        printf("Battery_Info.capacity = %dmA*H\n\r",Data->Battery_Info.capacity);
        printf("Battery_Info.capacity_full = %dmA*H\n\r",Data->Battery_Info.capacity_full);
        printf("Battery_Info.design_capacity = %dmA*H\n\r",Data->Battery_Info.design_capacity);
        printf("Battery_Info.percent = %d%%\n\r",Data->Battery_Info.percent);
        printf("Battery_Info.percent_unfiltered = %d%%\n\r",Data->Battery_Info.percent_unfiltered);
        printf("Battery_Info.current = %dmA\n\r",Data->Battery_Info.current);
        printf("Battery_Info.power = %dmW\n\r",Data->Battery_Info.power);
        printf("Battery_Info.health = %d%%\n\r",Data->Battery_Info.health);
        printf("Battery_Info.charge_detect = %d\n\r",Data->Battery_Info.charge_detect);
        printf("Battery_Info.fast_charge = %d\n\r",Data->Battery_Info.fast_charge);
        printf("Battery_Info.battery_discharging = %d\n\r",Data->Battery_Info.battery_discharging);

    }
#endif
}