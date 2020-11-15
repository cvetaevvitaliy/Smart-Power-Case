//
// Created by Vitaliy on 6/4/20.
//
#include "Debug.h"

void Debug_Task(Device_Status_t *Data){

    static uint32_t time_task = 0;

    if (HAL_GetTick() - time_task > 500){
        uint32_t tmp = time_task;
        time_task = HAL_GetTick();

        LOG_DEBUG("variable time_delay_task = %lums\n\r", HAL_GetTick() - tmp);
        LOG_DEBUG("Work Time = %dh:%dm:%ds\n\r",Data->work_time_hours,Data->work_time_minute,Data->work_time_second);
        LOG_DEBUG("Battery_Info.internal_temperature_stm = %.2fC\n\r",Data->Battery_Info.temperature);
        LOG_DEBUG("Battery_Info.Vbat = %.2fV\n\r",Data->Battery_Info.Vbat);
        LOG_DEBUG("Battery_Info.capacity = %dmA*H\n\r",Data->Battery_Info.capacity);
        LOG_DEBUG("Battery_Info.capacity_full = %dmA*H\n\r",Data->Battery_Info.capacity_full);
        LOG_DEBUG("Battery_Info.design_capacity = %dmA*H\n\r",Data->Battery_Info.design_capacity);
        LOG_DEBUG("Battery_Info.percent = %d%%\n\r",Data->Battery_Info.percent);
        LOG_DEBUG("Battery_Info.percent_unfiltered = %d%%\n\r",Data->Battery_Info.percent_unfiltered);
        LOG_DEBUG("Battery_Info.current = %dmA\n\r",Data->Battery_Info.current);
        LOG_DEBUG("Battery_Info.power = %dmW\n\r",Data->Battery_Info.power);
        LOG_DEBUG("Battery_Info.health = %d%%\n\r",Data->Battery_Info.health);
        LOG_DEBUG("Battery_Info.charge_detect = %d\n\r",Data->Battery_Info.charge_detect);
        LOG_DEBUG("Battery_Info.fast_charge = %d\n\r",Data->Battery_Info.fast_charge);
        LOG_DEBUG("Battery_Info.battery_discharging = %d\n\r",Data->Battery_Info.battery_discharging);
        LOG_DEBUG("Data->Device_Settings.low_volt = %d\n\r",Data->Device_Settings.low_volt);
        LOG_DEBUG("Data->time_for_auto_off = %d\n\r",Data->time_for_auto_off);
        LOG_DEBUG("Data->Device_Settings.time_auto_off = %d\n\r",Data->Device_Settings.time_auto_off);

    }
}
