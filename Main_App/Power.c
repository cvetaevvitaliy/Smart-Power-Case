/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "Power.h"
#include "bq2589x_charger.h"

extern TIM_HandleTypeDef htim2;

void Power_OLEDOn(bool state){

    if (state)
        HAL_GPIO_WritePin(LCD_En_GPIO_Port, LCD_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(LCD_En_GPIO_Port, LCD_En_Pin, GPIO_PIN_RESET);

    HAL_Delay(200);
}

void Power_BoostEnable(bool state){

    if (state)
        HAL_GPIO_WritePin(Bust_En_GPIO_Port, Bust_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(Bust_En_GPIO_Port, Bust_En_Pin, GPIO_PIN_RESET);

}

void Power_BoostEnable12V(bool state){

    if (state)
        HAL_GPIO_WritePin(Vout_En_12V_GPIO_Port, Vout_En_12V_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(Vout_En_12V_GPIO_Port, Vout_En_12V_Pin, GPIO_PIN_RESET);

}


void Power_USBEnable(bool state){

    if (state)
        HAL_GPIO_WritePin(USB_En_GPIO_Port, USB_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(USB_En_GPIO_Port, USB_En_Pin, GPIO_PIN_RESET);

}

void Power_USBResetGPIO(void){
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);

    HAL_Delay(100);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12);

}


void Power_SystemOn(bool state){

    if (state)
        HAL_GPIO_WritePin(SYS_On_GPIO_Port, SYS_On_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(SYS_On_GPIO_Port, SYS_On_Pin, GPIO_PIN_RESET);

}



void Power_BatteryTask(Device_Status_t *Data){

    static uint32_t time_delay_task = 0;
    static uint32_t time_auto_off = 0;
    static bool start_timer_auto_off = false;

    if (HAL_GetTick() - time_delay_task > 500) {
        Data->Battery_Info.temperature = BQ27441_temperature(BATTERY) / 100.0;
        Data->Battery_Info.capacity = BQ27441_capacity(REMAIN_F);
        Data->Battery_Info.capacity_full = BQ27441_capacity(FULL_F);
        Data->Battery_Info.design_capacity = BQ27441_capacity(FULL_F);
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
}


void Power_DevicePowerOffTimer(const Device_Status_t *Data){

    if (Data->time_for_auto_off != 0 && Data->ChargeChip.charging_status == 0 ) {
        if (Data->time_for_auto_off > Data->Device_Settings.time_auto_off)
            Power_Off();
    }

}


void Power_Off(void){

    ssd1306_Clear();
    ssd1306_Draw_String("Power Off", 20, 10, &Font_8x10);
    ssd1306_UpdateScreen();
    TIM2->ARR = 30000;
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_Delay(300);

    bq2589x_enter_ship_mode();
    bq2589x_enter_hiz_mode();
    ssd1306_DisplayOff();
    Power_OLEDOn(false);
    BQ27441_SET_HIBERNATE();
    HAL_Delay(100);

    PWR->CSR |= PWR_CSR_EWUP;
    PWR->CR  |= PWR_CR_CWUF;
    PWR->CR = PWR_CR_PDDS | PWR_CR_CWUF;
    HAL_PWR_EnterSTANDBYMode();

}

bool Power_ChargerInit(void){

    if (bq2589x_init_device() == true) {
        bq2589x_exit_ship_mode();
        bq2589x_exit_hiz_mode();
        bq2589x_set_charge_current(4000);
        bq2589x_adc_start(true);
        bq2589x_set_prechg_current(1024);
        bq2589x_set_bat_limit(2800);
        bq2589x_set_chargevoltage(4140);
        bq2589x_set_term_current(64);
        //bq2589x_set_IR_compensation_resistor(1);
        bq2589x_enable_max_charge(true);
        bq2589x_enable_charger();
        return true;
    } else
        return false;

}


void Power_ChargerTask(ChargeChip_t *Data){

    static uint32_t time_delay_task = 0;
    static bool usb_enable = false;

    if (HAL_GetTick() - time_delay_task > 500) {

        Data->Vbat = bq2589x_adc_read_battery_volt();
        Data->Vbus = bq2589x_adc_read_vbus_volt();
        Data->vbus_type = bq2589x_get_vbus_type();
        Data->charge_done = bq2589x_is_charge_done();
        Data->charging_status = bq2589x_get_charging_status();

#ifdef USE_USB_DEBUG_PRINTF

        if (Data->vbus_type == BQ2589X_VBUS_USB_SDP  || Data->vbus_type == BQ2589X_VBUS_USB_CDP) {
            if (usb_enable == false) {
                usb_enable = true;
                Enable_USBDebug(true);
                Power_USBResetGPIO();
                Power_USBEnable(true);
                HAL_Delay(200);
                MX_USB_DEVICE_Init();
                HAL_Delay(200);  /// need this time for reset and reinit USB
            }
        }

        if (Data->vbus_type == BQ2589X_VBUS_NONE && usb_enable == true) {
            Power_USB_Enable(false);
            MX_USB_DEVICE_DeInit();
            usb_enable = false;
            Enable_USB_Debug(false);
            HAL_Delay(200);
        }
#endif

        time_delay_task = HAL_GetTick();
    }
}