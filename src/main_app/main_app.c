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
#include "cli.h"

#define MAGIC_BKP_VALUE_BOOT                   ((uint32_t)0x424C) /// this value for write to BKP RTC register, to enable bootloader after reboot
#define MAGIC_BKP_VALUE_BQ                     ((uint32_t)0x425C)

extern DMA_HandleTypeDef hdma_adc1;
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim2;

static Device_Status_t Device_Status = {0};

static void Time_Task(Device_Status_t *Data);
static void Enter_DFU_Mode(Device_Status_t *Data);

void App_Setup(void){

    HAL_TIM_Base_Start_IT(&htim2);
    //Settings_SetDefault(&Device_Status.Device_Settings);
    Settings_Get(&Device_Status.Device_Settings);
    if (Device_Status.Device_Settings.low_volt < 250 || Device_Status.Device_Settings.low_volt > 400) {
        Settings_SetDefault(&Device_Status.Device_Settings);
    }
}

void App_Init(void){

    CLI_Init(CLI_TDC_None);
    CLI_cmd_init();

    if (Power_ChargerInit() == false){
        Device_Status.Device_Error = Device_Error_BQ25895;
        Device_Status.system_critical_error = true;
#if 0
        LOG_DEBUG("ADC BQ25895\n");
#endif
    }

    if (ADC_Init() == false){
        Device_Status.Device_Error = Device_Error_ADC;
        Device_Status.system_critical_error = true;
#if 0
        LOG_DEBUG("ADC Error\n");
#endif
    }
    if (BQ27441_init() == false) {
        Device_Status.Device_Error = Device_Error_BQ27441;
        Device_Status.system_critical_error = true;
#if 0
        LOG_DEBUG("BQ27441 not found\n");
#endif
    }

    Power_OLEDOn(true);
    Power_BoostEnable(true);
    if (ssd1306_Init() == false) {
        Device_Status.Device_Error = Device_Error_SSD1306;
        Device_Status.system_critical_error = true;
#if 0
        LOG_DEBUG("ssd1306 not found\n");
#endif
        Power_OLEDOn(false);
        Power_SystemOn(false);
        Power_BoostEnable(false);
    } else {
        ssd1306_Draw_String("System Init", 20, 10, &Font_8x10);
        ssd1306_Draw_String(SOFTWARE_VERSION, 40, 20,&Font_8x10);
        ssd1306_UpdateScreen();
        HAL_Delay(1500);
        ssd1306_Clear();
    }

}

bool App_Check_StartUp(void){

    if (Device_Status.system_critical_error == true){
#if 0
        LOG_DEBUG("Error %d\n", Device_Status.Device_Error);
#endif
        if (Device_Status.Device_Error != Device_Error_SSD1306) {
            ssd1306_Draw_String("Error System", 0, 0, &Font_8x10);
            ssd1306_Draw_String(print_error[Device_Status.Device_Error], 0, 10, &Font_8x10);
            ssd1306_UpdateScreen();
            HAL_Delay(2000);
            ssd1306_Clear();
        }
        return false;
    } else {
        Power_SystemOn(true);
        Power_BoostEnable(true);
        if (Device_Status.Device_Settings.Boost_mode == Boost_12V)
            Power_BoostEnable12V(true);
        else
            Power_BoostEnable12V(false);
    }

    if (HAL_RTCEx_BKUPRead(&hrtc, 8) != MAGIC_BKP_VALUE_BQ){
        HAL_RTCEx_BKUPWrite(&hrtc, 8, MAGIC_BKP_VALUE_BQ);
        ssd1306_Draw_String("Reset BQ27441", 0, 0, &Font_8x10);
        ssd1306_UpdateScreen();
        BQ27441_Full_Reset();
        HAL_Delay(5000);
    }


    if (BQ27441_itporFlag() ) {
        ssd1306_Draw_String("Find NEW BAT", 0, 0, &Font_8x10);
        ssd1306_Draw_String("Please", 0, 10, &Font_8x10);
        ssd1306_Draw_String("Recalibrate", 0, 20, &Font_8x10);
        ssd1306_UpdateScreen();
        Device_Status.need_calibrate = true;
        HAL_Delay(2000);
    } else {
        Device_Status.need_calibrate = false;
        //BQ27441_CLEAR_HIBERNATE();
        //Settings_SetMinLiionVoltPowerOff((Device_Status.Device_Settings.low_volt * 10));
    }
    return true;

}


void App_Loop(void){

    Time_Task(&Device_Status);
    //ADC_Task(&Device_Status.ADC_Data);
    Button_Task(&Device_Status.Device_Settings);
    OLED_UI_Task(&Device_Status);
    Power_BatteryTask(&Device_Status);
    Power_ChargerTask(&Device_Status.ChargeChip);
#if 0
    Debug_Task(&Device_Status);
#endif

    Enter_DFU_Mode(&Device_Status);
    if (Get_Enable_USB())
        CLI_Service();


    if (!Device_Status.Device_Settings.locked_power_off && Device_Status.ChargeChip.charging_status == 0){
        if (Button_GetState(Button_select)){
            Power_DCDC();
            Device_Status.time_for_auto_off = 0;
        }

    }

}

static void Time_Task(Device_Status_t *Data){

    static uint32_t last_time = 0;
    if (HAL_GetTick() - last_time > 1000){
        last_time = HAL_GetTick();
        Data->work_time_second++;
        if (Data->work_time_second == 60){
            Data->work_time_minute++;
            if (Data->ChargeChip.Vbus < 3500)
                Data->time_for_auto_off++;
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
    Power_DevicePowerOffTimer(Data);
}

static void Enter_DFU_Mode(Device_Status_t *Data) {

    if (HAL_GPIO_ReadPin(ButtonMenu_GPIO_Port, Button_Menu_Pin) &&
        HAL_GPIO_ReadPin(ButtonSelect_GPIO_Port, Button_Select_Pin)) {
        if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_SDP || Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_CDP) {
            Data->ADC_Data.Vbus = 0;
            Activate_DFU();
        } else {
            ssd1306_Clear();
            ssd1306_Draw_String("Reboot", 30, 10, &Font_8x10);
            ssd1306_UpdateScreen();
            HAL_Delay(1000);
            NVIC_SystemReset(); // Uncomment this, if need reset for debug FW
        }

    }
}


Device_Status_t* Get_Device_Status(void){

    return &Device_Status;
}

void Activate_DFU(void){
    ssd1306_Clear();
    ssd1306_Draw_String("DFU 3.0", 30, 10, &Font_8x10);
    ssd1306_UpdateScreen();
    HAL_RTCEx_BKUPWrite(&hrtc, 4, MAGIC_BKP_VALUE_BOOT);
    HAL_Delay(1000);
    NVIC_SystemReset();

}

bool Settings_Get_Buzzer(void){

    Settings_Get(&Device_Status.Device_Settings);
    return Device_Status.Device_Settings.buzzer_enable;
}
