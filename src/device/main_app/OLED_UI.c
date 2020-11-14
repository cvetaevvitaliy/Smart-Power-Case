/********************************
* Created by Vitaliy on 4/28/20.
********************************/

#include "OLED_UI.h"
#include "button.h"
#include "ssd1306.h"
#include <stdio.h>

extern TIM_HandleTypeDef htim2;
char print_oled_string[15] = {0};

static enum Current_Menu_e {
    Current_Screen_Main_Progress = 0,
    Current_Screen_Menu_Page_1,
    Current_Screen_Menu_Page_2,
    Current_Screen_Low_Volt,
    Current_Screen_I_Max,
    Current_Screen_Vout,
    Current_Screen_Buzzer,
    Current_Screen_Time_Off,
    Current_Screen_Set_Capacity,
    Current_Screen_Get_Info,

}Current_Menu;

static enum Active_Main_Screen_e{
    Active_Screen_1 = 0,
    Active_Screen_2,
    Active_Screen_3,

}Active_Main_Screen;


static uint16_t ScaleBat (uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

static void OLED_UI_DrawFrame (void);
static void OLED_UI_PrintMainScreen (Device_Status_t *Data);
static void OLED_UI_PrintMenuPage_1 (Device_Status_t *Data);
static void OLED_UI_PrintMenuPage_2 (Device_Status_t *Data);
static void Draw_Battery (Device_Status_t *Data);

static void OLED_UI_MainScreen_1(Device_Status_t *Data);
static void OLED_UI_MainScreen_2(Device_Status_t *Data);

static void OLED_UI_ScreenSetLowVolt (Device_Status_t *Data);
static void OLED_UI_ScreenSetIMax (Device_Status_t *Data);
static void OLED_UI_ScreenSetVout (Device_Status_t *Data);
static void OLED_UI_ScreenSetBuzzer (Device_Status_t *Data);
static void OLED_UI_ScreenSetTimeOff (Device_Status_t *Data);
static void OLED_UI_ScreenSetCapacity (Device_Status_t *Data);
static void OLED_UI_ScreenGetInfo (Device_Status_t *Data);

static void OLED_UI_DrawWorkTime (Device_Status_t *Data);
static void OLED_UI_DrawTypeCharger(Device_Status_t *Data);


void OLED_UI_Task(Device_Status_t *Data){

    static bool calibrate = false;
    if (Data->need_calibrate == true && calibrate == false) {
        Current_Menu = Current_Screen_Set_Capacity;
        calibrate = true;
    }

    ssd1306_Clear();

    if (Current_Menu == Current_Screen_Main_Progress)
        Data->Device_Settings.locked_power_off = false;
    else
        Data->Device_Settings.locked_power_off = true;

    switch (Current_Menu) {
        case Current_Screen_Main_Progress:
            Button_GetState(Button_select);
            OLED_UI_PrintMainScreen(Data);
            break;
        case Current_Screen_Menu_Page_1:
            OLED_UI_PrintMenuPage_1(Data);
            break;
        case Current_Screen_Menu_Page_2:
            OLED_UI_PrintMenuPage_2(Data);
            break;
        case Current_Screen_Low_Volt:
            OLED_UI_ScreenSetLowVolt(Data);
            break;
        case Current_Screen_I_Max:
            OLED_UI_ScreenSetIMax(Data);
            break;
        case Current_Screen_Vout:
            OLED_UI_ScreenSetVout(Data);
            break;
        case Current_Screen_Buzzer:
            OLED_UI_ScreenSetBuzzer(Data);
            break;
        case Current_Screen_Time_Off:
            OLED_UI_ScreenSetTimeOff(Data);
            break;
        case Current_Screen_Set_Capacity:
            OLED_UI_ScreenSetCapacity(Data);
            break;
        case Current_Screen_Get_Info:
            OLED_UI_ScreenGetInfo(Data);
            break;

        default:

            break;
    }

    ssd1306_UpdateScreen();

}


static inline void OLED_UI_DrawFrame(void ){

    for (uint8_t i = 0; i < 64; i++)
        if (i % 2)
            ssd1306_DrawPixel(i,0);

    for (uint8_t i = 0; i < 64; i++)
        if (i % 2)
            ssd1306_DrawPixel(i,30);

    for (uint8_t i = 2; i < 62; i++)
        if (i % 2)
            ssd1306_DrawPixel(i,15);

    for (uint8_t i = 0; i < 30; i++)
        if (i % 2)
            ssd1306_DrawPixel(0,i);

    for (uint8_t i = 0; i < 30; i++)
        if (i % 2)
            ssd1306_DrawPixel(64,i);


    for (uint8_t i = 0; i < 30; i++)
        if (i % 2)
            ssd1306_DrawPixel(69,i);

    for (uint8_t i = 70; i < 127; i++)
        if (i % 2)
            ssd1306_DrawPixel(i,30);

    for (uint8_t i = 0; i < 16; i++)
        if (i % 2)
            ssd1306_DrawPixel(92,i);

    for (uint8_t i = 70; i < 92; i++)
        if (i % 2)
            ssd1306_DrawPixel(i,0);

    for (uint8_t i = 92; i < 126; i++)
        if (i % 2)
            ssd1306_DrawPixel(i,16);

    for (uint8_t i = 16; i < 30; i++)
        if (i % 2)
            ssd1306_DrawPixel(126,i);

}


static void OLED_UI_PrintMainScreen(Device_Status_t *Data){

    if (Button_GetState(Button_menu))
        Current_Menu = Current_Screen_Menu_Page_1;

    static uint32_t time_screen = 0;
    if (HAL_GetTick() - time_screen > 3000){
        time_screen = HAL_GetTick();
        Active_Main_Screen++;

        if (Active_Main_Screen > Active_Screen_2) // todo - needs Active_Screen_3
            Active_Main_Screen = 0;
    }

    OLED_UI_DrawFrame();

    switch (Active_Main_Screen) {

        case Active_Screen_1:
            OLED_UI_MainScreen_1(Data);
            break;
        case Active_Screen_2:
            OLED_UI_MainScreen_2(Data);
            break;
        case Active_Screen_3:
            break;
        default:
            break;
    }

    Draw_Battery(Data);

}


void OLED_UI_PrintMenuPage_1(Device_Status_t *Data){

    static uint8_t ptr = 0;

    if (Button_GetState(Button_menu)) {
        ptr++;
        if (ptr > 3) {
            Current_Menu = Current_Screen_Menu_Page_2;
            ptr = 0;
            return;
        }
    }

    if (Button_GetState(Button_select)) {
        if (ptr == 0 )
            Current_Menu = Current_Screen_Low_Volt;
        if (ptr == 1 )
            Current_Menu = Current_Screen_I_Max;
        if (ptr == 2 )
            Current_Menu = Current_Screen_Vout;
        if (ptr == 3)
            Current_Menu = Current_Screen_Buzzer;
    }

    ssd1306_Draw_Bitmap_Mono(3 + POSITION_ICONS * ptr,3,&Image_Frame_27x27);

    ssd1306_Draw_Bitmap_Mono(4,4,&Image_Set_Low_Volt);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS,4,&Image_Set_I_Max);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS * 2,4,&Image_Set_V_Out);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS * 3,4,&Image_Buzzer);


}


void OLED_UI_PrintMenuPage_2(Device_Status_t *Data){

    static uint8_t ptr_p2 = 0;

    if (Button_GetState(Button_menu)){
        ptr_p2++;
        if (ptr_p2 > 3) {
            Current_Menu = Current_Screen_Menu_Page_1;
            ptr_p2 = 0;
            return;
        }
    }

    if (Button_GetState(Button_select)) {
        if (ptr_p2 == 0 )
            Current_Menu = Current_Screen_Time_Off;
        if (ptr_p2 == 1 )
            Current_Menu = Current_Screen_Set_Capacity;
        if (ptr_p2 == 2 )
            Current_Menu = Current_Screen_Get_Info;
        if (ptr_p2 == 3) {
            Current_Menu = Current_Screen_Main_Progress;
            ptr_p2 = 0;
        }
    }

    ssd1306_Draw_Bitmap_Mono(3 + POSITION_ICONS * ptr_p2,3,&Image_Frame_27x27);

    ssd1306_Draw_Bitmap_Mono(4,4,&Image_Set_Timer_Off);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS,4,&Image_Set_Capasity_Design);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS * 2,4,&Image_Get_Status_Info);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS * 3,4,&Image_Exit_Menu);


}


static inline void Draw_Battery(Device_Status_t *Data) {

    uint16_t print_percent;
    static uint32_t blink_low = 0;
    static uint32_t time_print_icon = 0;
    static bool print_low = false;
    static bool print_icon_bat = true;

    ssd1306_SetColor(Black);
    ssd1306_FillRect(2 + POSITION_BATTERY_X, 2 + POSITION_BATTERY_Y, 21, 8);
    ssd1306_SetColor(White);

    if (Data->Battery_Info.percent <= 5 && Data->ChargeChip.charging_status == 0) {
        if (print_low == false) {
            print_low = true;
            blink_low = HAL_GetTick();
            ssd1306_Draw_String("LOW", POSITION_BATTERY_X, 2 + POSITION_BATTERY_Y, &Font_8x10);
            if (Data->Device_Settings.buzzer_enable == true) {
                TIM2->ARR = 10000;
                HAL_TIM_Base_Start_IT(&htim2);
            }
        } else if (HAL_GetTick() - blink_low > 500 && print_low) {
            print_low = false;
            blink_low = HAL_GetTick();
            ssd1306_Draw_String("LOW", POSITION_BATTERY_X, 2 + POSITION_BATTERY_Y, &Font_8x10);
        }
    }


    if (Data->ChargeChip.charging_status > 1 && Data->ChargeChip.charging_status < 3 && HAL_GetTick() - time_print_icon > 1000) {
        print_icon_bat = false;
        time_print_icon = HAL_GetTick();
    }else
        print_icon_bat = true;

    if (print_icon_bat) {
        /** battery icon **/
        ssd1306_DrawHorizontalLine(1 + POSITION_BATTERY_X, 0 + POSITION_BATTERY_Y, 23);
        ssd1306_DrawHorizontalLine(1 + POSITION_BATTERY_X, 11 + POSITION_BATTERY_Y, 23);
        ssd1306_DrawVerticalLine(0 + POSITION_BATTERY_X, 1 + POSITION_BATTERY_Y, 10);
        ssd1306_DrawVerticalLine(24 + POSITION_BATTERY_X, 1 + POSITION_BATTERY_Y, 10);

        ssd1306_DrawRect(24 + POSITION_BATTERY_X, 3 + POSITION_BATTERY_Y, 3, 6);
        /** battery icon finish  **/
    }


    print_percent = ScaleBat(Data->Battery_Info.percent, 0, 91, 0, 21);
    ssd1306_FillRect(2 + POSITION_BATTERY_X, 2 + POSITION_BATTERY_Y, print_percent, 8);


}


static inline void OLED_UI_MainScreen_1(Device_Status_t *Data){

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Current_mAh_Ico);
    ssd1306_SetColor(Black);
    if (Data->Battery_Info.capacity < 10)
        sprintf(print_oled_string, "   %d", Data->Battery_Info.capacity);
    else if (Data->Battery_Info.capacity < 100)
        sprintf(print_oled_string, "  %d", Data->Battery_Info.capacity);
    else if (Data->Battery_Info.capacity < 1000)
        sprintf(print_oled_string, " %d", Data->Battery_Info.capacity);
    else
        sprintf(print_oled_string, "%d", Data->Battery_Info.capacity);
    ssd1306_Draw_String(print_oled_string, 30, 3, &Font_8x10);

    ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(3, 18, &Image_Full_mAh_Ico);
    ssd1306_SetColor(Black);
    if (Data->Battery_Info.capacity_full < 10)
        sprintf(print_oled_string, "   %d", Data->Battery_Info.capacity_full);
    else if (Data->Battery_Info.capacity_full < 100)
        sprintf(print_oled_string, "  %d", Data->Battery_Info.capacity_full);
    else if (Data->Battery_Info.capacity_full < 1000)
        sprintf(print_oled_string, " %d", Data->Battery_Info.capacity_full);
    else
        sprintf(print_oled_string, "%d", Data->Battery_Info.capacity_full);
    ssd1306_Draw_String(print_oled_string, 30, 18, &Font_8x10);


    if (Data->ChargeChip.charging_status == 0)
        OLED_UI_DrawWorkTime(Data);
    else
        OLED_UI_DrawTypeCharger(Data);

}


static inline void OLED_UI_MainScreen_2(Device_Status_t *Data){

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Battery_Type_Ico);
    ssd1306_SetColor(Black);
    sprintf(print_oled_string, "%.2fV", Data->Battery_Info.Vbat );
    ssd1306_Draw_String(print_oled_string, 22, 3, &Font_8x10);

    ssd1306_SetColor(White);
    if (Data->Battery_Info.current < 0) {
        ssd1306_Draw_Bitmap_Mono(3, 18, &Image_Current_I);
        sprintf(print_oled_string, "%.2fA", (Data->Battery_Info.current / 1000.0) * -1);
    }
    else {
        ssd1306_Draw_Bitmap_Mono(3, 18, &Image_Current_In);
        sprintf(print_oled_string, "%.2fA", (Data->Battery_Info.current / 1000.0));
    }
    ssd1306_SetColor(Black);
    ssd1306_Draw_String(print_oled_string, 22, 18, &Font_8x10);



    ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(72, 3, &Image_Remaining_Time);

    if (Data->Battery_Info.power > 0) {
        if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_SDP || Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_CDP)
            ssd1306_Draw_Bitmap_Mono(72, 3, &Image_USB_Ico);
        else if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_DCP)
            ssd1306_Draw_Bitmap_Mono(72, 3, &Image_5V_Ico);
        else if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_MAXC)
            ssd1306_Draw_Bitmap_Mono(72, 3, &Image_QC_Ico);
        else if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_UNKNOWN ||
                 Data->ChargeChip.vbus_type == BQ2589X_VBUS_NONSTAND)
            ssd1306_Draw_Bitmap_Mono(72, 3, &Image_Err_Ico);

        sprintf(print_oled_string, "%.1fW", Data->Battery_Info.power * 0.001f);

        ssd1306_Draw_String(print_oled_string, POSITION_WORK_TIME_X + 5, POSITION_WORK_TIME_Y, &Font_8x10);

    } else {
        if (Data->Battery_Info.time_to_empty < 600)
            sprintf(print_oled_string, " %dh%dm", (Data->Battery_Info.time_to_empty) / 60,
                    (Data->Battery_Info.time_to_empty % 60));
        else
            sprintf(print_oled_string, "%dh%dm", (Data->Battery_Info.time_to_empty) / 60,
                    (Data->Battery_Info.time_to_empty % 60));

        if (Data->Battery_Info.charge_flag && Data->Battery_Info.current < 650)
            strcpy(print_oled_string, " -----");

        ssd1306_Draw_String(print_oled_string, POSITION_WORK_TIME_X, POSITION_WORK_TIME_Y, &Font_8x10);
    }


}


static void OLED_UI_ScreenSetLowVolt (Device_Status_t *Data){

    HAL_Delay(100);
    static uint8_t ptr_low = 0;
    static uint8_t shift_frame;
    static bool read_settings = false;
    static uint16_t min_volt = 0;
    static uint8_t first_position = 0;
    static uint8_t second_position = 0;
    static uint8_t third_position = 0;

    if (ptr_low > 3)
        ptr_low = 0;

    if (read_settings == false) {
        read_settings = true;
        min_volt = Data->Device_Settings.low_volt;
        first_position = min_volt / 100 ;
        second_position = (min_volt / 10 ) % 10;
        third_position = min_volt % 10;
    }

    if (Button_GetState(Button_select) ) {
        if ( ptr_low == 0 ) {
            first_position++;
            if (first_position == 4)
                first_position = 2;
        }
        if ( ptr_low == 1 ){
            second_position++;
            if (second_position == 10)
                second_position = 0;
        }
        if ( ptr_low == 2 ){
            third_position++;
            if (third_position == 10)
                third_position = 0;
        }
        if (ptr_low == 3 ){

            if (Data->need_calibrate)
                Current_Menu = Current_Screen_Main_Progress;
            else
                Current_Menu = Current_Screen_Menu_Page_1;

            Data->Device_Settings.low_volt = (first_position * 100) + (second_position * 10) + third_position;
            Settings_Set(&Data->Device_Settings);
            Settings_SetMinVoltPowerOff(Data->Device_Settings.low_volt);
            ptr_low = 0;
            Data->need_calibrate = false;
            read_settings = false;
        }
    }


    if (Button_GetState(Button_menu) ) {
        ptr_low++;
        if (ptr_low == 4)
            ptr_low = 0;
    }

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(4, 4, &Image_Set_Low_Volt);

    sprintf(print_oled_string, "%d.", first_position);
    ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X + POSITION_SHIFT, POSITION_TEXT_Y, &Font_8x10);
    sprintf(print_oled_string, "%d", second_position);
    ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X + POSITION_SHIFT + 15, POSITION_TEXT_Y, &Font_8x10);
    sprintf(print_oled_string, "%dV", third_position);
    ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X + POSITION_SHIFT + 23, POSITION_TEXT_Y, &Font_8x10);

    if (ptr_low == 0)
        shift_frame = POSITION_SHIFT + 0;
    if (ptr_low == 1)
        shift_frame = POSITION_SHIFT + 15;
    if (ptr_low == 2)
        shift_frame = POSITION_SHIFT+ 23;
    if (ptr_low < 3)
        ssd1306_DrawHorizontalLine(POSITION_TEXT_X + shift_frame, POSITION_TEXT_Y + 10, 7);
    else
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);

    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X , POSITION_ICO_APPLY_Y, &Image_Apply);

}


static void OLED_UI_ScreenSetIMax (Device_Status_t *Data) {

    static uint8_t ptr_max = 0;
    static uint8_t shift_frame;
    static uint16_t I_set = 0;
    static uint8_t set_I_max[4] = {0};
    static bool read_settings_i = false;

    if (!read_settings_i) {
        I_set = Data->Device_Settings.current_max;
        set_I_max[0] = I_set / 1000;
        set_I_max[1] = (I_set / 100) % 10;
        set_I_max[2] = (I_set / 10) % 10;
        set_I_max[3] = I_set % 10;
        read_settings_i = true;
    }

    if (Button_GetState(Button_menu) ) {
        ptr_max++;
        if (ptr_max == 5)
            ptr_max = 0;
    }

    if (Button_GetState(Button_select) ) {
        switch (ptr_max) {
            case 0:
                set_I_max[0]++;
                if (set_I_max[0] == 4)
                    set_I_max[0] = 0;
                break;
            case 1:
                set_I_max[1]++;
                if (set_I_max[1] == 10)
                    set_I_max[1] = 0;
                break;
            case 2:
                set_I_max[2]++;
                if (set_I_max[2] == 10)
                    set_I_max[2] = 0;
                break;
            case 3:
                set_I_max[3]++;
                if (set_I_max[3] == 10)
                    set_I_max[3] = 0;
                break;
            case 4:
                Current_Menu = Current_Screen_Menu_Page_1;
                Data->Device_Settings.current_max =
                        (set_I_max[0] * 1000) + (set_I_max[1] * 100) + (set_I_max[2] * 10) + set_I_max[3];
                Settings_Set(&Data->Device_Settings);
                Power_ChargerInit();
                ptr_max = 0;
                read_settings_i = false;
                break;
            default:
                ptr_max = 0;
        }
    }

    ssd1306_Draw_Bitmap_Mono(0, 2, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(1, 3, &Image_Set_I_Max);
    for (uint8_t i = 0; i < 4; i++) {
        sprintf(print_oled_string, "%d", set_I_max[i]);
        if (ptr_max == i )
            ssd1306_SetColor(Black);
        ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X - 4  + POSITION_SHIFT - 4 + (i * 10), POSITION_TEXT_Y,
                            &Font_8x10);
        ssd1306_SetColor(White);
    }

    if (ptr_max == 4)
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);

    ssd1306_Draw_String("mA", POSITION_TEXT_X + 67, POSITION_TEXT_Y, &Font_8x10);
    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);

}


static void OLED_UI_ScreenSetVout (Device_Status_t *Data){

    static uint8_t ptr_v = 0;

    if (ptr_v > 3)
        ptr_v = 0;

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(4, 4, &Image_Set_V_Out);

    if (Button_GetState(Button_menu) ) {
        ptr_v++;
        if (ptr_v == 3)
            ptr_v = 0;
    }

    if (ptr_v == 0)
        ssd1306_Draw_Bitmap_Mono(37, 6, &Image_Frame_22x20);

    if (ptr_v == 1)
        ssd1306_Draw_Bitmap_Mono(64, 6, &Image_Frame_22x20);

    if (ptr_v == 2)
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);

    ssd1306_SetColor(White);

    if (Data->Device_Settings.Boost_mode == Boost_8V)
        ssd1306_SetColor(Black);
    else
        ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(39, 8, &Image_Set_8V);
    ssd1306_SetColor(White);

    if (Data->Device_Settings.Boost_mode == Boost_12V)
        ssd1306_SetColor(Black);
    else
        ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(66, 8, &Image_Set_12V);
    ssd1306_SetColor(White);

    if (Button_GetState(Button_select) ) {
        if (ptr_v == 0)
            Data->Device_Settings.Boost_mode = Boost_8V;
        if (ptr_v == 1)
            Data->Device_Settings.Boost_mode = Boost_12V;
        if (ptr_v == 2) {
            Current_Menu = Current_Screen_Menu_Page_1;
            if (Data->Device_Settings.Boost_mode == Boost_12V)
                Power_BoostEnable12V(true);
            else
                Power_BoostEnable12V(false);
            Settings_Set(&Data->Device_Settings);
            ptr_v = 0;
        }

    }

    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);
}


static void OLED_UI_ScreenSetBuzzer (Device_Status_t *Data){

    volatile static uint8_t ptr_buzz = 0;

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(4, 4, &Image_Buzzer);

    if (ptr_buzz > 2) {
        if (Data->Device_Settings.buzzer_enable)
            ptr_buzz = 0;
        else
            ptr_buzz = 1;
    }


    if (Button_GetState(Button_menu) ) {
        ptr_buzz++;
        if (ptr_buzz == 3)
            ptr_buzz = 0;
    }

    if (ptr_buzz == 0)
        ssd1306_Draw_Bitmap_Mono(37, 6, &Image_Frame_22x20);

    if (ptr_buzz == 1)
        ssd1306_Draw_Bitmap_Mono(64, 6, &Image_Frame_22x20);

    if (ptr_buzz == 2)
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);

    ssd1306_SetColor(White);

    if (Data->Device_Settings.buzzer_enable == true)
        ssd1306_SetColor(Black);
    else
        ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(39, 8, &Image_Buzzer_On_Ico);
    ssd1306_SetColor(White);

    if (Data->Device_Settings.buzzer_enable == false)
        ssd1306_SetColor(Black);
    else
        ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(66, 8, &Image_Buzzer_Off_Ico);
    ssd1306_SetColor(White);

    if (Button_GetState(Button_select) ) {
        if (ptr_buzz == 0)
            Data->Device_Settings.buzzer_enable = true;
        if (ptr_buzz == 1)
            Data->Device_Settings.buzzer_enable = false;
        if (ptr_buzz == 2) {
            Current_Menu = Current_Screen_Menu_Page_1;
            Settings_Set(&Data->Device_Settings);
            ptr_buzz = 0;
        }

    }
    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);
}


static void OLED_UI_ScreenSetTimeOff (Device_Status_t *Data) {

    static uint8_t ptr_t = 0;
    static uint8_t time_minutes = 0;
    static bool read_settings = false;

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(4, 4, &Image_Set_Timer_Off);

    if (read_settings == false) {
        read_settings = true;
        time_minutes = (uint8_t) Data->Device_Settings.time_auto_off;
    }


    if (Button_GetState(Button_menu)) {
        ptr_t++;
        if (ptr_t == 2)
            ptr_t = 0;
    }

    if (Button_GetState(Button_select)) {
        if (ptr_t == 0) {
            time_minutes += 5;
            if (time_minutes > 60)
                time_minutes = 0;
        }

        if (ptr_t == 1) {
            Current_Menu = Current_Screen_Menu_Page_2;
            Data->Device_Settings.time_auto_off = (uint16_t)time_minutes;
            Settings_Set(&Data->Device_Settings);
            ptr_t = 0;
            read_settings = false;
        }

    }

    if (ptr_t == 0)
        ssd1306_SetColor(Black);
    else
        ssd1306_SetColor(White);

    if (time_minutes == 0)
        sprintf(print_oled_string, "   off");
    else
        sprintf(print_oled_string, "%d min", time_minutes);

    ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X + 37, POSITION_TEXT_Y, &Font_8x10);
    ssd1306_SetColor(White);

    if (ptr_t == 1)
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);

}


static void OLED_UI_ScreenSetCapacity (Device_Status_t *Data){


    static uint8_t ptr_c = 0;
    static uint8_t shift_frame;
    static uint16_t capacity = 0;
    static uint8_t set_capacity[4] = {0};
    static bool read_settings_c = false;

    if (!read_settings_c) {
        capacity = Data->Device_Settings.design_capacity;
        set_capacity[0] = capacity / 1000;
        set_capacity[1] = (capacity / 100) % 10;
        set_capacity[2] = (capacity / 10) % 10;
        set_capacity[3] = capacity % 10;
        read_settings_c = true;
    }

    if (Button_GetState(Button_select) ) {
        if ( ptr_c == 0 ) {
            set_capacity[0]++;
            if (set_capacity[0] == 8)
                set_capacity[0] = 0;
        }
        if ( ptr_c == 1 ){
            set_capacity[1]++;
            if (set_capacity[1] == 10)
                set_capacity[1] = 0;
        }
        if ( ptr_c == 2 ){
            set_capacity[2]++;
            if (set_capacity[2] == 10)
                set_capacity[2] = 0;
        }
        if ( ptr_c == 3 ){
            set_capacity[3]++;
            if (set_capacity[3] == 10)
                set_capacity[3] = 0;
        }
        if (ptr_c == 4 ){
            if (Data->need_calibrate == true)
                Current_Menu = Current_Screen_Low_Volt;
            else
                Current_Menu = Current_Screen_Menu_Page_2;

            Data->Device_Settings.design_capacity = (set_capacity[0] * 1000) + (set_capacity[1] * 100) + (set_capacity[2] * 10) + set_capacity[3];
            Settings_Set(&Data->Device_Settings);
            Settings_SetBQ27441SetCapacity(Data->Device_Settings.design_capacity);
            ptr_c = 0;
            read_settings_c = false;
        }
    }

    if (Button_GetState(Button_menu) ) {
        ptr_c++;
        if (ptr_c == 5)
            ptr_c = 0;
    }

    ssd1306_Draw_Bitmap_Mono(0, 2, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(1, 3, &Image_Set_Capasity_Design);
    for (uint8_t i = 0; i < 4; i++) {
        sprintf(print_oled_string, "%d", set_capacity[i]);
        if (ptr_c == i )
            ssd1306_SetColor(Black);
        ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X - 4  + POSITION_SHIFT - 4 + (i * 10), POSITION_TEXT_Y,
                            &Font_8x10);
        ssd1306_SetColor(White);
    }

    if (ptr_c == 4)
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);

    ssd1306_Draw_String("mAh", POSITION_TEXT_X + 67, POSITION_TEXT_Y, &Font_8x10);
    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);

}


static void OLED_UI_ScreenGetInfo (Device_Status_t *Data){

    ssd1306_Draw_Bitmap_Mono(2, 5, &Image_Battery_Type_Ico);
    ssd1306_Draw_Bitmap_Mono(2, 18, &Image_Battery_Life_Ico);

    ssd1306_Draw_Bitmap_Mono(64, 5, &Image_Used_mAh);
    ssd1306_Draw_Bitmap_Mono(64, 18, &Image_Charge_Count_Ico);

    sprintf(print_oled_string,"%.2fV", Data->Battery_Info.Vbat);
    ssd1306_Draw_String(print_oled_string, 22, 6,&Font_8x10);
    sprintf(print_oled_string,"%d", Data->Battery_Info.capacity);
    ssd1306_Draw_String(print_oled_string, 84, 6,&Font_8x10);

    sprintf(print_oled_string,"%d%%", Data->Battery_Info.health);
    ssd1306_Draw_String(print_oled_string, 22, 19,&Font_8x10);


    if (Button_GetState(Button_select) || Button_GetState(Button_menu) )
        Current_Menu = Current_Screen_Menu_Page_2;
}


static void OLED_UI_DrawWorkTime(Device_Status_t *Data){

    ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(72, 3, &Image_Power_On_Time);
    if (Data->work_time_second < 10){
        sprintf(print_oled_string, "%d:%d:0%d", Data->work_time_hours, Data->work_time_minute, Data->work_time_second);
        if (Data->work_time_minute < 10)
            sprintf(print_oled_string, "%d:0%d:0%d", Data->work_time_hours, Data->work_time_minute, Data->work_time_second);
    } else if (Data->work_time_minute < 10)
        sprintf(print_oled_string, "%d:0%d:%d", Data->work_time_hours, Data->work_time_minute, Data->work_time_second);
    else
        sprintf(print_oled_string, "%d:%d:%d", Data->work_time_hours, Data->work_time_minute, Data->work_time_second);

    ssd1306_Draw_String(print_oled_string, POSITION_WORK_TIME_X, POSITION_WORK_TIME_Y, &Font_8x10);

}


static void OLED_UI_DrawTypeCharger(Device_Status_t *Data){
    ssd1306_SetColor(White);

    if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_SDP || Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_CDP)
        ssd1306_Draw_Bitmap_Mono(72, 3, &Image_USB_Ico);
    else if ( Data->ChargeChip.vbus_type == BQ2589X_VBUS_USB_DCP)
        ssd1306_Draw_Bitmap_Mono(72, 3, &Image_5V_Ico);
    else if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_MAXC)
        ssd1306_Draw_Bitmap_Mono(72, 3, &Image_QC_Ico);
    else if (Data->ChargeChip.vbus_type == BQ2589X_VBUS_UNKNOWN || Data->ChargeChip.vbus_type == BQ2589X_VBUS_NONSTAND)
        ssd1306_Draw_Bitmap_Mono(72, 3, &Image_Err_Ico);

    sprintf(print_oled_string, " %.1fV", (float )(Data->ChargeChip.Vbus/1000.0));
    if (Data->ChargeChip.Vbus > 10000)
        ssd1306_Draw_String(print_oled_string, POSITION_WORK_TIME_X, POSITION_WORK_TIME_Y, &Font_8x10);
    else
        ssd1306_Draw_String(print_oled_string, POSITION_WORK_TIME_X + 5, POSITION_WORK_TIME_Y, &Font_8x10);

}


static uint16_t ScaleBat(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    if (x < in_min)
        return out_min;

    if (x > in_max)
        return out_max;

    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
