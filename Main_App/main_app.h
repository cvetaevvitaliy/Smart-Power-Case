/********************************
* Created by Vitaliy on 4/28/20.
********************************/

#ifndef FATSHARKPOWER_SMART_POWER_APP_H
#define FATSHARKPOWER_SMART_POWER_APP_H
#include "main.h"
#include "stm32f1xx_hal.h"
#ifdef USE_USB_DEBUG_PRINTF
#include "usb_device.h"
#include "printf_USB.h"
#endif
#include "ssd1306.h"
#include "stdbool.h"

typedef enum {
    Boost_8V = 0,
    Boost_12V,
}Boost_mode_e;

static char print_error[8][15] = {
        "",
        "BQ27441",
        "SSD1306",
        "ADC",
        "Temp",
        "Batt",
        "Booster",
        "Booster_Set_12V",};

typedef enum {
    Device_OK,
    Device_Error_BQ27441,
    Device_Error_SSD1306,
    Device_Error_ADC,
    Device_Error_Temp,
    Device_Error_Batt,
    Device_Error_Booster,
    Device_Error_Set_12V_Booster,
}Device_Error_e;

typedef struct {
    bool Button_menu_pushed;
    bool Button_select_pushed;
    bool Button_menu_pressed;
    bool Button_select_pressed;
}Button_t;

typedef struct {
    float Vbus;
    float Vout;
    float temperature;
    uint16_t percent;

}ADC_Voltage_Data_t;

typedef struct {
    float Vbat;
    uint16_t percent;

}Battery_Status_t;

typedef struct {
    uint16_t current_max;
    uint16_t low_volt;
    uint16_t design_capacity;
    uint16_t time_auto_off;
    bool buzzer_enable;
    bool Boost_mode;

}Device_Settings_t;

typedef struct {
    Device_Settings_t Device_Settings;
    ADC_Voltage_Data_t ADC_Data;
    Battery_Status_t Battery_Info;
    Button_t State_Button;
    Device_Error_e Device_Error;
    uint8_t work_time_minute;
    uint8_t work_time_second;
    uint8_t work_time_hours;
    bool system_critical_error;
}Device_Status_t;


void App_Setup(void);
void App_Init(void);
void App_Check_StartUp(void);
void App_Loop(void);

#endif //FATSHARKPOWER_SMART_POWER_APP_H
