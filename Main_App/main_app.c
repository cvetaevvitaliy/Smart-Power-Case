/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "main_app.h"
#include "adc.h"
#include "OLED_UI.h"
#include "button.h"
#include "Power.h"
#include "Settings_Eeprom.h"
#include "Debug.h"

extern DMA_HandleTypeDef hdma_adc1;
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;

static Device_Status_t Device_Status = {0};

static void Time_Task(Device_Status_t *Data);
static void Need_Reset(Device_Status_t *Data);

void App_Setup(void){

    //Settings_Set_Default(&Device_Status.Device_Settings);
    Settings_Get(&Device_Status.Device_Settings);
    if (Device_Status.Device_Settings.low_volt == 0xFFFF) {
        Settings_Set_Default(&Device_Status.Device_Settings);
    }
}

void App_Init(void){

    if (Power_Charger_Init() == false){
        Device_Status.Device_Error = Device_Error_BQ25895;
        Device_Status.system_critical_error = true;
#ifdef USE_USB_DEBUG_PRINTF
        printf("ADC BQ25895\n");
#endif
    }

    if (ADC_Init() == false){
        Device_Status.Device_Error = Device_Error_ADC;
        Device_Status.system_critical_error = true;
#ifdef USE_USB_DEBUG_PRINTF
        printf("ADC Error\n");
#endif
    }
    if (BQ27441_init() == false) {
        Device_Status.Device_Error = Device_Error_BQ27441;
        Device_Status.system_critical_error = true;
#ifdef USE_USB_DEBUG_PRINTF
        printf("BQ27441 not found\n");
#endif
    }

    Power_OLED_On(true);
    Power_Boost_Enable(true);
    if (ssd1306_Init() == false) {
        Device_Status.Device_Error = Device_Error_SSD1306;
        Device_Status.system_critical_error = true;
#ifdef USE_USB_DEBUG_PRINTF
        printf("ssd1306 not found\n");
#endif
        Power_OLED_On(false);
        Power_System_On(false);
        Power_Boost_Enable(false);
    } else {
        //ssd1306_InvertDisplay();
        ssd1306_Draw_String("System Init", 20, 10, &Font_8x10);
        ssd1306_Draw_String(SOFTWARE_VERSION, 40, 20,&Font_8x10);
        ssd1306_UpdateScreen();
        HAL_Delay(1500);
        ssd1306_Clear();
    }

}

void App_Check_StartUp(void){

    if (Device_Status.system_critical_error == true){
#ifdef USE_USB_DEBUG_PRINTF
        printf("Error %d\n", Device_Status.Device_Error);
#endif
        ssd1306_Draw_String("Error System", 0, 0, &Font_8x10);
        ssd1306_Draw_String(print_error[Device_Status.Device_Error], 0, 10, &Font_8x10);
        ssd1306_UpdateScreen();
        HAL_Delay(2000);
        ssd1306_Clear();
    } else {
        Power_System_On(true);
        Power_Boost_Enable(true);
        if (Device_Status.Device_Settings.Boost_mode == Boost_12V)
            Power_Boost_Enable_12V(true);
        else
            Power_Boost_Enable_12V(false);
    }
    if (BQ27441_itporFlag() ) {
        ssd1306_Draw_String("Find NEW BAT", 0, 0, &Font_8x10);
        ssd1306_Draw_String("Please", 0, 10, &Font_8x10);
        ssd1306_Draw_String("Recalibrate", 0, 20, &Font_8x10);
        ssd1306_UpdateScreen();
        //Settings_Set_BQ27441_Set_Max_Liion_Volt(4145);
       // Settings_Set_BQ27441_Set_Min_Liion_Volt(2900);
        //BQ27441_setTaperRateVoltage(4100);
        //Settings_Set_BQ27441_Set_Capacity(6000);
        Device_Status.need_calibrate = true;
        //HAL_RTCEx_BKUPWrite(&hrtc,1,1);
        HAL_Delay(2000);
    } else {
        Device_Status.need_calibrate = false;
        BQ27441_CLEAR_HIBERNATE();
        //Settings_Set_BQ27441_Set_Min_Liion_Volt(Device_Status.Device_Settings.low_volt);
    }

}


void App_Loop(void){

    Time_Task(&Device_Status);
    ADC_Task(&Device_Status.ADC_Data);
    Button_Task(&Device_Status.State_Button, &Device_Status.Device_Settings);
    OLED_UI_Task(&Device_Status);
    Power_Battery_Task(&Device_Status);
    Power_Charger_Task(&Device_Status.ChargeChip);
#ifdef USE_USB_DEBUG_PRINTF
    Debug_Task(&Device_Status);
#endif

    Need_Reset(&Device_Status);


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

static void Need_Reset(Device_Status_t *Data){

    if (HAL_GPIO_ReadPin (Button1_GPIO_Port, Button1_Pin) && HAL_GPIO_ReadPin (Button2_GPIO_Port, Button2_Pin)) {
        if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_SDP || Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_CDP ) {
            ssd1306_Clear();
            ssd1306_Draw_String("DFU 3.0", 30, 10, &Font_8x10);
            ssd1306_UpdateScreen();
            uint16_t data = 0x424C;
            HAL_RTCEx_BKUPWrite(&hrtc, 4, (uint32_t) data);
            HAL_Delay(1000);
        }
       // SET_BIT(PWR->CR, PWR_CR_DBP);
        //WRITE_REG(BKP->DR4, 0x424C);
        //CLEAR_BIT(PWR->CR, PWR_CR_DBP);
       // HAL_Delay(1000);
        //if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_SDP || Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_CDP )
        //NVIC_SystemReset();
    }
}
