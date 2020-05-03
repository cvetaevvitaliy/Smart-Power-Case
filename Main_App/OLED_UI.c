/********************************
* Created by Vitaliy on 4/28/20.
********************************/

#include "OLED_UI.h"
#include "ssd1306.h"

extern TIM_HandleTypeDef htim1;
static char print_oled_string[30] = {0};

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


static uint16_t MinMax (uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

static void OLED_UI_Draw_Frame (void);
static void OLED_UI_Print_Main_Screen (Device_Status_t *Data);
static void OLED_UI_Print_Menu_Page_1 (Device_Status_t *Data);
static void OLED_UI_Print_Menu_Page_2 (Device_Status_t *Data);
static void Draw_Battery (Device_Status_t *Data);

static void OLED_UI_Main_Screen_1(Device_Status_t *Data);
static void OLED_UI_Main_Screen_2(Device_Status_t *Data);

static void OLED_UI_Screen_Set_Low_Volt (Device_Status_t *Data);
static void OLED_UI_Screen_Set_I_Max (Device_Status_t *Data);
static void OLED_UI_Screen_Set_Vout (Device_Status_t *Data);
static void OLED_UI_Screen_Set_Buzzer (Device_Status_t *Data);
static void OLED_UI_Screen_Set_Time_Off (Device_Status_t *Data);
static void OLED_UI_Screen_Set_Capacity (Device_Status_t *Data);
static void OLED_UI_Screen_Get_Info (Device_Status_t *Data);

static void Draw_Work_Time (Device_Status_t *Data);


void OLED_UI_Task(Device_Status_t *Data){

    ssd1306_Clear();

    switch (Current_Menu) {
        case Current_Screen_Main_Progress:
            OLED_UI_Print_Main_Screen(Data);
            break;
        case Current_Screen_Menu_Page_1:
            OLED_UI_Print_Menu_Page_1(Data);
            break;
        case Current_Screen_Menu_Page_2:
            OLED_UI_Print_Menu_Page_2(Data);
            break;
        case Current_Screen_Low_Volt:
            OLED_UI_Screen_Set_Low_Volt(Data);
            break;
        case Current_Screen_I_Max:
            OLED_UI_Screen_Set_I_Max(Data);
            break;
        case Current_Screen_Vout:
            OLED_UI_Screen_Set_Vout(Data);
            break;
        case Current_Screen_Buzzer:
            OLED_UI_Screen_Set_Buzzer(Data);
            break;
        case Current_Screen_Time_Off:
            OLED_UI_Screen_Set_Time_Off(Data);
            break;
        case Current_Screen_Set_Capacity:
            OLED_UI_Screen_Set_Capacity(Data);
            break;
        case Current_Screen_Get_Info:
            OLED_UI_Screen_Get_Info(Data);
            break;

        default:
            break;
    }

    ssd1306_UpdateScreen();

}


static void OLED_UI_Draw_Frame(void ){

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


static void OLED_UI_Print_Main_Screen(Device_Status_t *Data){

    if (Data->State_Button.Button_menu_pushed)
        Current_Menu = Current_Screen_Menu_Page_1;

    static uint32_t time_screen = 0;
    if (HAL_GetTick() - time_screen > 3000){
        time_screen = HAL_GetTick();
        Active_Main_Screen++;

        if (Active_Main_Screen > Active_Screen_2) // todo - needs Active_Screen_3
            Active_Main_Screen = 0;
    }


    OLED_UI_Draw_Frame();

    switch (Active_Main_Screen) {

        case Active_Screen_1:
            OLED_UI_Main_Screen_1(Data);
            break;
        case Active_Screen_2:
            OLED_UI_Main_Screen_2(Data);
            break;
        case Active_Screen_3:
            break;
        default:
            break;
    }

    Draw_Battery(Data);

}


static void OLED_UI_Print_Menu_Page_1(Device_Status_t *Data){

    static uint8_t ptr = 0;

    if (Data->State_Button.Button_menu_pushed) {
        ptr++;
        if (ptr > 3) {
            Current_Menu = Current_Screen_Menu_Page_2;
            ptr = 0;
            return;
        }
    }

    if (Data->State_Button.Button_select_pushed) {
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


static void OLED_UI_Print_Menu_Page_2(Device_Status_t *Data){

    static uint8_t ptr = 0;

    if (Data->State_Button.Button_menu_pushed){
        ptr++;
        if (ptr > 3) {
            Current_Menu = Current_Screen_Menu_Page_1;
            ptr = 0;
            return;
        }
    }

    if (Data->State_Button.Button_select_pushed) {
        if (ptr == 0 )
            Current_Menu = Current_Screen_Time_Off;
        if (ptr == 1 )
            Current_Menu = Current_Screen_Set_Capacity;
        if (ptr == 2 )
            Current_Menu = Current_Screen_Get_Info;
        if (ptr == 3) {
            Current_Menu = Current_Screen_Main_Progress;
            ptr = 0;
        }
    }

    ssd1306_Draw_Bitmap_Mono(3 + POSITION_ICONS * ptr,3,&Image_Frame_27x27);

    ssd1306_Draw_Bitmap_Mono(4,4,&Image_Set_Timer_Off);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS,4,&Image_Set_Capasity_Design);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS * 2,4,&Image_Get_Status_Info);
    ssd1306_Draw_Bitmap_Mono(4 + POSITION_ICONS * 3,4,&Image_Exit_Menu);


}


static void Draw_Battery(Device_Status_t *Data){

    uint16_t print_percent;
    static uint32_t blink_low = 0;
    static bool print_low = false;

    ssd1306_SetColor(Black);
    ssd1306_FillRect(2 + POSITION_BATTERY_X,2 + POSITION_BATTERY_Y ,21,8);
    ssd1306_SetColor(White);
    if (Data->Battery_Info.Vbat <= Data->Device_Settings.low_volt) {
        if (print_low == false) {
            print_low = true;
            blink_low = HAL_GetTick();
            ssd1306_Draw_String("LOW", POSITION_BATTERY_X, 2 + POSITION_BATTERY_Y, &Font_8x10);
            if (Data->Device_Settings.buzzer_enable == true)
                HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
        } else if (HAL_GetTick() - blink_low > 500 && print_low ) {
            print_low = false;
            blink_low = HAL_GetTick();
            ssd1306_Draw_String("LOW", POSITION_BATTERY_X, 2 + POSITION_BATTERY_Y, &Font_8x10);
        }
    }

    ssd1306_DrawHorizontalLine(1 + POSITION_BATTERY_X, 0 + POSITION_BATTERY_Y, 23 );
    ssd1306_DrawHorizontalLine(1 + POSITION_BATTERY_X, 11 + POSITION_BATTERY_Y, 23 );
    ssd1306_DrawVerticalLine(0 + POSITION_BATTERY_X,1 + POSITION_BATTERY_Y,10);
    ssd1306_DrawVerticalLine(24 + POSITION_BATTERY_X,1 + POSITION_BATTERY_Y,10);

    ssd1306_DrawRect(24 + POSITION_BATTERY_X,3 + POSITION_BATTERY_Y,3,6);

    print_percent = MinMax(Data->Battery_Info.percent,0,100,0,21);
    ssd1306_FillRect(2 + POSITION_BATTERY_X,2 + POSITION_BATTERY_Y, print_percent,8);

}


static void OLED_UI_Main_Screen_1(Device_Status_t *Data){

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Used_mAh);
    ssd1306_SetColor(Black);
    ssd1306_Draw_String(" 2345", 22, 3, &Font_8x10);
    ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(3, 18, &Image_Current_I);
    ssd1306_SetColor(Black);
    ssd1306_Draw_String("0.53A", 22, 18, &Font_8x10);
    ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(72, 3, &Image_Power_On_Time);
    Draw_Work_Time(Data);

}


static void OLED_UI_Main_Screen_2(Device_Status_t *Data){

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Temperature_Ico);
    ssd1306_SetColor(Black);
    ssd1306_Draw_String(" 23.6", 22, 3, &Font_8x10);
    ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(3, 18, &Image_Vout);
    ssd1306_SetColor(Black);
    if (Data->Device_Settings.Boost_mode == Boost_12V)
        ssd1306_Draw_String(" 12V ", 22, 18, &Font_8x10);
    else
        ssd1306_Draw_String(" 8.2V", 22, 18, &Font_8x10);
    ssd1306_SetColor(White);
    ssd1306_Draw_Bitmap_Mono(72, 3, &Image_Remaining_Time);
    ssd1306_Draw_String("1:23:44", POSITION_WORK_TIME_X, POSITION_WORK_TIME_Y, &Font_8x10);

}


static void OLED_UI_Screen_Set_Low_Volt (Device_Status_t *Data){

    static uint8_t ptr = 0;
    static uint8_t shift_frame;
    static bool read_settings = false;
    static uint16_t min_volt = 0;
    static uint8_t first_position = 0;
    static uint8_t second_position = 0;
    static uint8_t third_position = 0;

    if (read_settings == false) {
        read_settings = true;
        min_volt = Data->Device_Settings.low_volt;
        first_position = min_volt / 100 ;
        second_position = (min_volt / 10 ) % 10;
        third_position = min_volt % 10;
    }

    if (Data->State_Button.Button_select_pushed ) {
        if ( ptr == 0 ) {
            first_position++;
            if (first_position == 4)
                first_position = 2;
        }
        if ( ptr == 1 ){
            second_position++;
            if (second_position == 10)
                second_position = 0;
        }
        if ( ptr == 2 ){
            third_position++;
            if (third_position == 10)
                third_position = 0;
        }
        if (ptr == 3 ){
            Current_Menu = Current_Screen_Menu_Page_1;
            Data->Device_Settings.low_volt = (first_position * 100) + (second_position * 10) + third_position;
            Settings_Set(&Data->Device_Settings);
            ptr = 0;
        }
    }


    if (Data->State_Button.Button_menu_pushed ) {
        ptr++;
        if (ptr == 4)
            ptr = 0;
    }

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(4, 4, &Image_Set_Low_Volt);

    sprintf(print_oled_string, "%d.", first_position);
    ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X + POSITION_SHIFT, POSITION_TEXT_Y, &Font_8x10);
    sprintf(print_oled_string, "%d", second_position);
    ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X + POSITION_SHIFT + 15, POSITION_TEXT_Y, &Font_8x10);
    sprintf(print_oled_string, "%dV", third_position);
    ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X + POSITION_SHIFT + 23, POSITION_TEXT_Y, &Font_8x10);

    if (ptr == 0)
        shift_frame = POSITION_SHIFT + 0;
    if (ptr == 1)
        shift_frame = POSITION_SHIFT + 15;
    if (ptr == 2)
        shift_frame = POSITION_SHIFT+ 23;
    if (ptr < 3)
        ssd1306_DrawHorizontalLine(POSITION_TEXT_X + shift_frame, POSITION_TEXT_Y + 10, 7);
    else
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);

    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X , POSITION_ICO_APPLY_Y, &Image_Apply);

}


static void OLED_UI_Screen_Set_I_Max (Device_Status_t *Data) {

    static uint8_t ptr = 0;
    static uint8_t shift_frame;
    static uint16_t I_set = 0;
    static uint8_t set_I_max[4] = {0};
    static bool read_settings = false;

    if (!read_settings) {
        I_set = Data->Device_Settings.current_max;
        set_I_max[0] = I_set / 1000;
        set_I_max[1] = (I_set / 100) % 10;
        set_I_max[2] = (I_set / 10) % 10;
        set_I_max[3] = I_set % 10;
        read_settings = true;
    }

    if (Data->State_Button.Button_select_pushed ) {
        if ( ptr == 0 ) {
            set_I_max[0]++;
            if (set_I_max[0] == 3)
                set_I_max[0] = 0;
        }
        if ( ptr == 1 ){
            set_I_max[1]++;
            if (set_I_max[1] == 10)
                set_I_max[1] = 0;
        }
        if ( ptr == 2 ){
            set_I_max[2]++;
            if (set_I_max[2] == 10)
                set_I_max[2] = 0;
        }
        if ( ptr == 3 ){
            set_I_max[3]++;
            if (set_I_max[3] == 10)
                set_I_max[3] = 0;
        }
        if (ptr == 4 ){
            Current_Menu = Current_Screen_Menu_Page_1;
            Data->Device_Settings.current_max = (set_I_max[0] * 1000) + (set_I_max[1] * 100) + (set_I_max[2] * 10) + set_I_max[3];
            Settings_Set(&Data->Device_Settings);
            ptr = 0;
        }
    }

    if (Data->State_Button.Button_menu_pushed ) {
        ptr++;
        if (ptr == 5)
            ptr = 0;
    }

    ssd1306_Draw_Bitmap_Mono(0, 2, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(1, 3, &Image_Set_I_Max);
    for (uint8_t i = 0; i < 4; i++) {
        sprintf(print_oled_string, "%d", set_I_max[i]);
        if (ptr == i )
            ssd1306_SetColor(Black);
        ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X - 4  + POSITION_SHIFT - 4 + (i * 10), POSITION_TEXT_Y,
                            &Font_8x10);
        ssd1306_SetColor(White);
    }

    if (ptr == 4)
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);

    ssd1306_Draw_String("mA", POSITION_TEXT_X + 67, POSITION_TEXT_Y, &Font_8x10);
    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);

}


static void OLED_UI_Screen_Set_Vout (Device_Status_t *Data){

    static uint8_t ptr = 0;

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(4, 4, &Image_Set_V_Out);

    if (Data->State_Button.Button_menu_pushed ) {
        ptr++;
        if (ptr == 3)
            ptr = 0;
    }

    if (ptr == 0)
        ssd1306_Draw_Bitmap_Mono(37, 6, &Image_Frame_22x20);

    if (ptr == 1)
        ssd1306_Draw_Bitmap_Mono(64, 6, &Image_Frame_22x20);

    if (ptr == 2)
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

    if (Data->State_Button.Button_select_pushed ) {
        if (ptr == 0)
            Data->Device_Settings.Boost_mode = Boost_8V;
        if (ptr == 1)
            Data->Device_Settings.Boost_mode = Boost_12V;
        if (ptr == 2) {
            Current_Menu = Current_Screen_Menu_Page_1;
            Settings_Set(&Data->Device_Settings);
            ptr = 0;
        }

    }

    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);
}


static void OLED_UI_Screen_Set_Buzzer (Device_Status_t *Data){

    static uint8_t ptr = 0;

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(4, 4, &Image_Set_V_Out);

    if (Data->State_Button.Button_menu_pushed ) {
        ptr++;
        if (ptr == 3)
            ptr = 0;
    }

    if (ptr == 0)
        ssd1306_Draw_Bitmap_Mono(37, 6, &Image_Frame_22x20);

    if (ptr == 1)
        ssd1306_Draw_Bitmap_Mono(64, 6, &Image_Frame_22x20);

    if (ptr == 2)
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

    if (Data->State_Button.Button_select_pushed ) {
        if (ptr == 0)
            Data->Device_Settings.buzzer_enable = true;
        if (ptr == 1)
            Data->Device_Settings.buzzer_enable = false;
        if (ptr == 2) {
            Current_Menu = Current_Screen_Menu_Page_1;
            Settings_Set(&Data->Device_Settings);
            ptr = 0;
        }

    }
    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);
}


static void OLED_UI_Screen_Set_Time_Off (Device_Status_t *Data) {

    static uint8_t ptr = 0;
    static uint8_t time_minutes = 0;
    static bool read_settings = false;

    ssd1306_Draw_Bitmap_Mono(3, 3, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(4, 4, &Image_Set_Timer_Off);

    if (read_settings == false) {
        read_settings = true;
        time_minutes = (uint8_t) Data->Device_Settings.time_auto_off;
    }


    if (Data->State_Button.Button_menu_pushed) {
        ptr++;
        if (ptr == 2)
            ptr = 0;
    }

    if (Data->State_Button.Button_select_pushed) {
        if (ptr == 0) {
            time_minutes += 5;
            if (time_minutes > 60)
                time_minutes = 0;
        }

        if (ptr == 1) {
            Current_Menu = Current_Screen_Menu_Page_2;
            Data->Device_Settings.time_auto_off = (uint16_t)time_minutes;
            Settings_Set(&Data->Device_Settings);
            ptr = 0;
        }

    }

    if (ptr == 0)
        ssd1306_SetColor(Black);
    else
        ssd1306_SetColor(White);

    if (time_minutes == 0)
        sprintf(print_oled_string, "   off");
    else
        sprintf(print_oled_string, "%d min", time_minutes);

    ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X + 37, POSITION_TEXT_Y, &Font_8x10);
    ssd1306_SetColor(White);

    if (ptr == 1)
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);

}


static void OLED_UI_Screen_Set_Capacity (Device_Status_t *Data){


    static uint8_t ptr = 0;
    static uint8_t shift_frame;
    static uint16_t capacity = 0;
    static uint8_t set_capacity[4] = {0};
    static bool read_settings = false;

    if (!read_settings) {
        capacity = Data->Device_Settings.design_capacity;
        set_capacity[0] = capacity / 1000;
        set_capacity[1] = (capacity / 100) % 10;
        set_capacity[2] = (capacity / 10) % 10;
        set_capacity[3] = capacity % 10;
        read_settings = true;
    }

    if (Data->State_Button.Button_select_pushed ) {
        if ( ptr == 0 ) {
            set_capacity[0]++;
            if (set_capacity[0] == 7)
                set_capacity[0] = 0;
        }
        if ( ptr == 1 ){
            set_capacity[1]++;
            if (set_capacity[1] == 10)
                set_capacity[1] = 0;
        }
        if ( ptr == 2 ){
            set_capacity[2]++;
            if (set_capacity[2] == 10)
                set_capacity[2] = 0;
        }
        if ( ptr == 3 ){
            set_capacity[3]++;
            if (set_capacity[3] == 10)
                set_capacity[3] = 0;
        }
        if (ptr == 4 ){
            Current_Menu = Current_Screen_Menu_Page_2;
            Data->Device_Settings.design_capacity = (set_capacity[0] * 1000) + (set_capacity[1] * 100) + (set_capacity[2] * 10) + set_capacity[3];
            Settings_Set(&Data->Device_Settings);
            ptr = 0;
        }
    }

    if (Data->State_Button.Button_menu_pushed ) {
        ptr++;
        if (ptr == 5)
            ptr = 0;
    }

    ssd1306_Draw_Bitmap_Mono(0, 2, &Image_Frame_27x27);
    ssd1306_Draw_Bitmap_Mono(1, 3, &Image_Set_Capasity_Design);
    for (uint8_t i = 0; i < 4; i++) {
        sprintf(print_oled_string, "%d", set_capacity[i]);
        if (ptr == i )
            ssd1306_SetColor(Black);
        ssd1306_Draw_String(print_oled_string, POSITION_TEXT_X - 4  + POSITION_SHIFT - 4 + (i * 10), POSITION_TEXT_Y,
                            &Font_8x10);
        ssd1306_SetColor(White);
    }

    if (ptr == 4)
        ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X - 1, POSITION_ICO_APPLY_Y - 1, &Image_Frame_27x27);

    ssd1306_Draw_String("mAh", POSITION_TEXT_X + 67, POSITION_TEXT_Y, &Font_8x10);
    ssd1306_Draw_Bitmap_Mono(POSITION_ICO_APPLY_X, POSITION_ICO_APPLY_Y, &Image_Apply);

}


static void OLED_UI_Screen_Get_Info (Device_Status_t *Data){



}


static void Draw_Work_Time(Device_Status_t *Data){

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


static uint16_t MinMax(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    if (x < in_min)
        return out_min;

    if (x > in_max)
        return out_max;

    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}