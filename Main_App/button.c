/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "button.h"

#define DELAY_KEY_PUSHED_POWER_OFF      (3500)

extern TIM_HandleTypeDef htim2;

static struct {
    bool button_menu;
    bool button_select;
    bool button_beep_event;
}State_Button_t;

static void BeepEvent(void){

    if (State_Button_t.button_beep_event) {
        TIM2->ARR = 2000;
        HAL_TIM_Base_Start_IT(&htim2);
    }
}

void Button_Task(const Device_Settings_t *Settings) {

    static uint32_t time_power_off = 0;
    static bool start_delay = false;


    /** todo need refactor  */
    if (State_Button_t.button_beep_event != Settings->buzzer_enable)
        State_Button_t.button_beep_event = Settings->buzzer_enable;

    if (!Settings->locked_power_off) {
        if (!HAL_GPIO_ReadPin(ButtonSelect_GPIO_Port, Button_Select_Pin) && !start_delay) {
            time_power_off = HAL_GetTick();
            start_delay = true;
        } else
            start_delay = false;

        if (HAL_GetTick() - time_power_off > DELAY_KEY_PUSHED_POWER_OFF) {
            Power_Off();
        }
    } else
        time_power_off = HAL_GetTick();

}

bool Button_GetState (enum Button_e button){

    bool state = false;

    switch (button) {
        case Button_menu:
            if (State_Button_t.button_menu) {
                state = State_Button_t.button_menu;
                State_Button_t.button_menu = false;
                HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
                HAL_NVIC_EnableIRQ(EXTI1_IRQn);
            }
            break;

        case Button_select:
            if (State_Button_t.button_select){
                state = State_Button_t.button_select;
                State_Button_t.button_select = false;
                HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
                HAL_NVIC_EnableIRQ(EXTI2_IRQn);
            }
            break;

        default:
            /** nothing do  */
            break;
    }

    return state;

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

    if (GPIO_Pin == Button_Menu_Pin && !State_Button_t.button_menu){
        HAL_NVIC_DisableIRQ(EXTI1_IRQn);
        State_Button_t.button_menu = true;
        BeepEvent();
    }

    if (GPIO_Pin == Button_Select_Pin && !State_Button_t.button_select){
        HAL_NVIC_DisableIRQ(EXTI2_IRQn);
        State_Button_t.button_select = true;
        BeepEvent();
    }
}


