/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "Power.h"


void Power_OLED_On(bool state){

    if (state)
        HAL_GPIO_WritePin(LCD_En_GPIO_Port, LCD_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(LCD_En_GPIO_Port, LCD_En_Pin, GPIO_PIN_RESET);

    HAL_Delay(200);
}

void Power_Boost_Enable(bool state){

    if (state)
        HAL_GPIO_WritePin(Bust_En_GPIO_Port, Bust_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(Bust_En_GPIO_Port, Bust_En_Pin, GPIO_PIN_RESET);

}

void Power_Boost_Enable_12V(bool state){

    if (state)
        HAL_GPIO_WritePin(Vout_En_12V_GPIO_Port, Vout_En_12V_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(Vout_En_12V_GPIO_Port, Vout_En_12V_Pin, GPIO_PIN_RESET);

}


void Power_USB_Enable(bool state){

    if (state)
        HAL_GPIO_WritePin(USB_En_GPIO_Port, USB_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(USB_En_GPIO_Port, USB_En_Pin, GPIO_PIN_RESET);

}


void Power_System_On(bool state){

    if (state)
        HAL_GPIO_WritePin(SYS_On_GPIO_Port, SYS_On_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(SYS_On_GPIO_Port, SYS_On_Pin, GPIO_PIN_RESET);

}



void Power_Battery_Task(Device_Status_t *Data){

    static uint32_t time_delay_task = 0;

    if (HAL_GetTick() - time_delay_task > 500) {

        Data->Battery_Info.temperature = BQ27441_temperature(BATTERY) / 100.0;
        Data->Battery_Info.capacity = BQ27441_capacity(REMAIN);
        Data->Battery_Info.capacity_full = BQ27441_capacity(FULL_UF);
        Data->Battery_Info.Vbat = BQ27441_voltage() / 1000.0;
        Data->Battery_Info.percent = BQ27441_soc(FILTERED);
        Data->Battery_Info.percent_unfiltered = BQ27441_soc(UNFILTERED);
        Data->Battery_Info.current = BQ27441_current(AVG);
        Data->Battery_Info.power = BQ27441_power();

        if (Data->ADC_Data.Vbus > 3.5)
            Data->Battery_Info.charge_flag = true;
        else
            Data->Battery_Info.charge_flag = false;

        if (Data->Battery_Info.current < 0)
            Data->Battery_Info.time_to_empty =
                    (float) ((float) Data->Battery_Info.capacity / (Data->Battery_Info.current * -1)) * 60;
        else
            Data->Battery_Info.time_to_empty =
                    (float) ((float) (Data->Battery_Info.capacity_full - Data->Battery_Info.capacity) /
                             (Data->Battery_Info.current)) * 60;

        time_delay_task = HAL_GetTick();
    }
    //Power_Off();
}


void Power_Device_Off_On(Device_Status_t *Data){

    if (Data->work_time_minute != 0 && Data->Battery_Info.charge_flag == false) {
        if (Data->work_time_minute > Data->Device_Settings.time_auto_off)
            Power_Off();
    }

}


void Power_Off(void){

    ssd1306_DisplayOff();
    PWR->CSR |= PWR_CSR_EWUP;
    PWR->CR  |= PWR_CR_CWUF;
    PWR->CR = PWR_CR_PDDS | PWR_CR_CWUF;
    HAL_PWR_EnterSTANDBYMode();

}