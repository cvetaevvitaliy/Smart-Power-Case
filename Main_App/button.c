/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "button.h"

#define DEBOUNCE_DELAY                 3

extern TIM_HandleTypeDef htim2;

typedef struct State_Button {
    bool Button1;
    bool Button2;
    uint16_t counter_button1;
    uint16_t counter_button2;
}State_Button_t;

static State_Button_t State_Button = {0};

void Button_Task(Button_t *Data, const Device_Settings_t *Settings) {


    State_Button.Button1 = HAL_GPIO_ReadPin (Button1_GPIO_Port, Button1_Pin);
    State_Button.Button2 = HAL_GPIO_ReadPin (Button2_GPIO_Port, Button2_Pin);


    if (State_Button.Button1 == true){
        State_Button.counter_button1++;
    }

    if (State_Button.Button1 == false && State_Button.counter_button1 > 0)
        if (State_Button.counter_button1 != 0) {
            State_Button.counter_button1--;
        }

    if (State_Button.counter_button1 >= DEBOUNCE_DELAY) {
        State_Button.counter_button1 = 0;
        Data->Button_menu_pushed = true;
        if (Settings->buzzer_enable == true) {
            //TIM2->ARR = 2000;
            //HAL_TIM_Base_Start_IT(&htim2);
        }
    } else
        Data->Button_menu_pushed = false;




    if (State_Button.Button2 == true)
        State_Button.counter_button2++;
    if (State_Button.Button2 == false && State_Button.counter_button2 > 0)
        if (State_Button.counter_button2 != 0)
            State_Button.counter_button2--;

    if (State_Button.counter_button2 >= DEBOUNCE_DELAY) {
        State_Button.counter_button2 = 0;
        Data->Button_select_pushed = true;
        if (Settings->buzzer_enable == true) {
            //TIM2->ARR = 2000;
            //HAL_TIM_Base_Start_IT(&htim2);
        }
    } else
        Data->Button_select_pushed = false;


}
