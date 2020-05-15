/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "main_app.h"
#include "adc.h"
#include "OLED_UI.h"
#include "button.h"
#include "Power.h"
#include "Settings_Eeprom.h"
#include "HDQ.h"

#define CHECK_MIN_VOUT_8V                 7

uint8_t BYT1, BYT2;
int dev, firm, hard, major, minor;
uint32_t time_delay = 0;

extern DMA_HandleTypeDef hdma_adc1;
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;

static Device_Status_t Device_Status = {0};

static void Time_Task(Device_Status_t *Data);

void App_Setup(void){
#ifdef USE_USB_DEBUG_PRINTF
    HAL_Delay(200);  /// need this time for reset and reinit USB
#endif
    //Settings_Set_Default(&Device_Status.Device_Settings);
    Settings_Get(&Device_Status.Device_Settings);
    //Settings_Set_BQ27441();
}

void App_Init(void){

    if (ADC_Init() == false){
        Device_Status.Device_Error = Device_Error_ADC;
        Device_Status.system_critical_error = true;
        printf("ADC Error\n");
    }
    if (BQ27441_init() == false) {
        Device_Status.Device_Error = Device_Error_BQ27441;
        Device_Status.system_critical_error = true;
        printf("BQ27441 not found\n");
    }

    Power_OLED_On(true);
    if (ssd1306_Init() == false) {
        Device_Status.Device_Error = Device_Error_SSD1306;
        Device_Status.system_critical_error = true;
        printf("ssd1306 not found\n");
        Power_OLED_On(false);
        Power_System_On(false);
        Power_Boost_Enable(false);
    } else {
        //ssd1306_InvertDisplay();
        ssd1306_Draw_String("System Init", 20, 10, &Font_8x10);
        ssd1306_UpdateScreen();
        HAL_Delay(500);
        ssd1306_Clear();
    }

}

void App_Check_StartUp(void){

    if (Device_Status.system_critical_error == true){
        printf("Error %d\n", Device_Status.Device_Error);
        ssd1306_Draw_String("Error System", 0, 0, &Font_8x10);
        ssd1306_Draw_String(print_error[Device_Status.Device_Error], 0, 10, &Font_8x10);
        ssd1306_UpdateScreen();
        HAL_Delay(200);
        ssd1306_Clear();
    } else {
        Power_System_On(true);
        Power_Boost_Enable(true);
        if (Device_Status.Device_Settings.Boost_mode == Boost_12V)
            Power_Boost_Enable_12V(true);
        else
            Power_Boost_Enable_12V(false);
    }
    if (HAL_RTCEx_BKUPRead(&hrtc,1) == 0) {
        ssd1306_Draw_String("Find NEW BAT", 0, 0, &Font_8x10);
        ssd1306_Draw_String("Please", 0, 10, &Font_8x10);
        ssd1306_Draw_String("Recalibrate", 0, 20, &Font_8x10);
        ssd1306_UpdateScreen();
        Settings_Set_BQ27441_Set_Max_Liion_Volt(4180);
        Settings_Set_BQ27441_Set_Capacity(6000);
        HAL_RTCEx_BKUPWrite(&hrtc,1,1);
    }

}


void App_Loop(void){

   // clrscr();

    Time_Task(&Device_Status);
    ADC_Task(&Device_Status.ADC_Data);
    Button_Task(&Device_Status.State_Button, &Device_Status.Device_Settings);
    OLED_UI_Task(&Device_Status);
    Power_Battery_Task(&Device_Status);

}

static void Time_Task(Device_Status_t *Data){

    static uint32_t last_time = 0;
    if (HAL_GetTick() - last_time > 1000){
        last_time = HAL_GetTick();
        Data->work_time_second++;
        if (Data->work_time_second == 60){
            Data->work_time_minute++;
            Data->work_time_second = 0;
            if (Data->work_time_minute == 60) {
                Data->work_time_minute = 0;
                Data->work_time_hours++;
                if (Data->work_time_hours == 10) {
                    Data->work_time_hours = 0;
                }
            }
        }
    }
}
