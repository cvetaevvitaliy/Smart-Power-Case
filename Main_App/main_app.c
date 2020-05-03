/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "main_app.h"
#include "adc.h"
#include "OLED_UI.h"
#include "button.h"
#include "BQ27441.h"
#include "Power.h"
#include "Settings_Eeprom.h"

#define CHECK_MIN_VOUT_8V                 7


extern DMA_HandleTypeDef hdma_adc1;
extern I2C_HandleTypeDef hi2c1;

static Device_Status_t Device_Status = {0};


void App_Setup(void){
#ifdef USE_USB_DEBUG_PRINTF
    HAL_Delay(2000);  /// need this time for reset and reinit USB
#endif
    //Settings_Set_Default(&Device_Status.Device_Settings);
    Settings_Get(&Device_Status.Device_Settings);
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

    if (Device_Status.system_critical_error == false){
        Power_System_On(true);
        Power_Boost_Enable(true);
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
    }
    if (Device_Status.ADC_Data.Vout < CHECK_MIN_VOUT_8V ) {
        printf("Error DC/DC\n");
        ssd1306_Draw_String("Error HV", 10, 0, &Font_8x10);
        ssd1306_Draw_String("DC/DC ", 10, 10, &Font_8x10);
        ssd1306_UpdateScreen();
        HAL_Delay(200);
        ssd1306_Clear();
    }
}


void App_Loop(void){

    ADC_Task(&Device_Status.ADC_Data);
    Button_Task(&Device_Status.State_Button, &Device_Status.Device_Settings);
    OLED_UI_Task(&Device_Status);

}
