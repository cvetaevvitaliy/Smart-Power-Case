/********************************
* Created by Vitaliy on 4/28/20.
********************************/

#ifndef FATSHARKPOWER_SMART_POWER_APP_H
#define FATSHARKPOWER_SMART_POWER_APP_H
#include "main.h"
#include "stm32f1xx_hal.h"
//#include "tinyprintf.h"
#ifdef USE_USB_DEBUG
#include "usb_device.h"
#include "printf_USB.h"
#endif
#include "ssd1306.h"
#include "stdbool.h"
#include "bq27441.h"
#include "bq2589x_charger.h"
#include "CLI_comands.h"


typedef enum {
    Boost_8V = 0,
    Boost_12V,
}Boost_mode_e;

static char print_error[7][15] = {
        "BQ27441",
        "SSD1306",
        "ADC",
        "BQ25895"
        "Temp",
        "Batt",
        "Booster",
        "Booster_Set_12V",};

typedef enum {
    Device_Error_BQ27441,
    Device_Error_SSD1306,
    Device_Error_ADC,
    Device_Error_BQ25895,
    Device_Error_Temp,
    Device_Error_Batt,
    Device_Error_Booster,
    Device_Error_Set_12V_Booster,
}Device_Error_e;

typedef struct {
    float Vbus;
    float Vout;
    float internal_temperature_stm;

}ADC_Voltage_Data_t;

typedef struct {
    float Vbat;
    uint16_t percent;
    uint16_t percent_unfiltered;
    int16_t current;
    int16_t power;
    uint16_t capacity;
    uint16_t capacity_full;
    uint16_t design_capacity;
    float temperature;
    uint16_t time_to_empty;
    bool charge_flag;
    uint8_t health;
    bool fast_charge;
    bool charge_detect;
    bool battery_discharging;

}Battery_Status_t;

typedef struct {
    uint16_t current_max;
    uint16_t low_volt;
    uint16_t design_capacity;
    uint16_t time_auto_off;
    bool buzzer_enable;
    bool Boost_mode;
    bool locked_power_off;

}Device_Settings_t;

typedef struct {
    bool chip_sleep;
    bool charge_done;
    bool usb_detect;
    uint16_t charging_status;
    uint16_t Vbus;
    uint16_t Vbat;
    uint16_t Vsys;
    int16_t temp;
    bq2589x_vbus_type vbus_type;

}ChargeChip_t;

typedef struct {
    Device_Settings_t Device_Settings;
    ADC_Voltage_Data_t ADC_Data;
    Battery_Status_t Battery_Info;
    Device_Error_e Device_Error;
    ChargeChip_t ChargeChip;
    uint8_t time_for_auto_off;
    uint8_t work_time_minute;
    uint8_t work_time_second;
    uint8_t work_time_hours;
    bool system_critical_error;
    bool hiz_mode;
    bool need_calibrate;
    bool usb_detect;
}Device_Status_t;


void App_Setup(void);
void App_Init(void);
bool App_Check_StartUp(void);
void App_Loop(void);
Device_Status_t* Get_Device_Status(void);
void Activate_DFU(void);
bool Settings_Get_Buzzer(void);

#endif //FATSHARKPOWER_SMART_POWER_APP_H
