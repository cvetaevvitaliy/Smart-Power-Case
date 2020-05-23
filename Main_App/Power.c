/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "Power.h"
#include "bq2589x_charger.h"

extern TIM_HandleTypeDef htim2;

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
    static uint32_t time_auto_off = 0;
    static bool start_timer_auto_off = false;

    if (HAL_GetTick() - time_delay_task > 500) {

        Data->Battery_Info.temperature = BQ27441_temperature(BATTERY) / 100.0;
        Data->Battery_Info.capacity = BQ27441_capacity(REMAIN);
        Data->Battery_Info.capacity_full = BQ27441_capacity(FULL);
        Data->Battery_Info.Vbat = BQ27441_voltage() / 1000.0;
        Data->Battery_Info.percent = BQ27441_soc(FILTERED);
        Data->Battery_Info.percent_unfiltered = BQ27441_soc(UNFILTERED);
        Data->Battery_Info.current = BQ27441_current(AVG);
        Data->Battery_Info.power = BQ27441_power();
        Data->Battery_Info.health = BQ27441_soh(PERCENT);
        Data->Battery_Info.charge_detect = BQ27441_fcFlag();
        Data->Battery_Info.fast_charge = BQ27441_chgFlag();
        Data->Battery_Info.battery_discharging = BQ27441_dsgFlag();

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

        if ((Data->Battery_Info.Vbat * 100) < Data->Device_Settings.low_volt && Data->ADC_Data.Vbus < 2.0 && Data->hiz_mode == false){
            bq2589x_adc_start(false);
            bq2589x_enter_ship_mode();
            bq2589x_enter_hiz_mode();
            Data->hiz_mode = true;
        } else if (Data->ADC_Data.Vbus > 2.0) {
            bq2589x_exit_ship_mode();
            bq2589x_adc_start(true);
            Data->hiz_mode = false;
        }

        if (Data->Battery_Info.current < -Data->Device_Settings.current_max){
            bq2589x_adc_start(false);
            bq2589x_enter_ship_mode();
            bq2589x_enter_hiz_mode();
        }

        if (Data->Battery_Info.current > -300 && Data->Battery_Info.current < 100 && Data->ChargeChip.Vbus < 3500)
        {
            if (start_timer_auto_off == false) {
                time_auto_off = HAL_GetTick();
                start_timer_auto_off = true;
            }
            if (HAL_GetTick() - time_auto_off > 120000){
                TIM2->ARR = 30000;
                HAL_TIM_Base_Start_IT(&htim2);
                Power_Off();
            }

        } else {
            start_timer_auto_off = false;
            time_auto_off = HAL_GetTick();
        }



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

    bq2589x_enter_ship_mode();
    bq2589x_enter_hiz_mode();
    ssd1306_DisplayOff();
    Power_OLED_On(false);
    HAL_Delay(100);

    PWR->CSR |= PWR_CSR_EWUP;
    PWR->CR  |= PWR_CR_CWUF;
    PWR->CR = PWR_CR_PDDS | PWR_CR_CWUF;
    //HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    HAL_PWR_EnterSTANDBYMode();

}

bool Power_Charger_Init(void){

    if (bq2589x_init_device() == true) {
        bq2589x_exit_ship_mode();
        bq2589x_exit_hiz_mode();
        bq2589x_set_charge_current(4000);
        bq2589x_adc_start(true);
        bq2589x_set_prechg_current(2048);
        bq2589x_set_bat_limit(2800);
        bq2589x_set_chargevoltage(4200);
        bq2589x_enable_max_charge(true);
        return true;
    } else
        return false;

}


void Power_Charger_Task(ChargeChip_t *Data){

    static uint32_t time_delay_task = 0;

    if (HAL_GetTick() - time_delay_task > 500) {

        Data->Vbat = bq2589x_adc_read_battery_volt();
        Data->Vbus = bq2589x_adc_read_vbus_volt();
        Data->vbus_type = bq2589x_get_vbus_type();
        Data->charge_done = bq2589x_is_charge_done();
        Data->charging_status = bq2589x_get_charging_status();
        
//        if (Data->vbus_type == BQ2589X_VBUS_USB_SDP && Data->usb_detect == false) {
//            Power_USB_Enable(true);
//            Data->usb_detect = true;
//            MX_USB_DEVICE_Init();
//        } else if (Data->vbus_type == BQ2589X_VBUS_NONE && Data->usb_detect == true) {
//            Data->usb_detect = false;
//            Power_USB_Enable(false);
//        }



        time_delay_task = HAL_GetTick();
    }
}