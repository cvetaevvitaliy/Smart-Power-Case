/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "button.h"

#define DELAY_PUSHED                    200
#define DELAY_KEY_PUSHED_POWER_OFF      4500

extern TIM_HandleTypeDef htim2;

static struct State_Button {
    bool button_menu;
    bool button_select;
    bool button_beep;
    uint32_t time_btn_menu;
    uint32_t time_btn_select;
}State_Button_t;

static void BeepEvent(void){

    if (State_Button_t.button_beep) {
        TIM2->ARR = 2000;
        HAL_TIM_Base_Start_IT(&htim2);
        State_Button_t.time_btn_menu = HAL_GetTick();
    }
}

static void BounceButton (Button_t *Data){

    Data->Button_menu_pushed = State_Button_t.button_menu;
    Data->Button_select_pushed = State_Button_t.button_select;

    if (HAL_GetTick() - State_Button_t.time_btn_menu < DELAY_PUSHED)
        State_Button_t.button_menu = false;
    if (HAL_GetTick() - State_Button_t.time_btn_select < DELAY_PUSHED)
        State_Button_t.button_select = false;

}

void Button_Task(Button_t *Data, const Device_Settings_t *Settings, uint8_t* time) {

    static uint32_t time_power_off = 0;
    static bool start_delay = false;

    State_Button_t.button_beep = Settings->buzzer_enable;
    BounceButton(Data);

    if (State_Button_t.button_select && State_Button_t.button_menu)
        time = 0;

    if (!Settings->locked_power_off) {
        if (!HAL_GPIO_ReadPin(ButtonSelect_GPIO_Port, Button_Select_Pin) && !start_delay) {
            time_power_off = HAL_GetTick();
            start_delay = true;
        } else
            start_delay = false;

        if (HAL_GetTick() - time_power_off > DELAY_KEY_PUSHED_POWER_OFF)
            Power_Off();
    } else
        time_power_off = HAL_GetTick();

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

    if (GPIO_Pin == Button_Menu_Pin){
        BeepEvent();
        State_Button_t.button_menu = true;
        State_Button_t.time_btn_menu = HAL_GetTick();
    }

    if (GPIO_Pin == Button_Select_Pin){
        BeepEvent();
        State_Button_t.button_select = true;
        State_Button_t.time_btn_select = HAL_GetTick();
    }
}


