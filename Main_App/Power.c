/********************************
* Created by Vitaliy on 4/28/20.
********************************/
#include "Power.h"


void Power_OLED_On(bool state){

    if (state)
        HAL_GPIO_WritePin(LCD_En_GPIO_Port, LCD_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(LCD_En_GPIO_Port, LCD_En_Pin, GPIO_PIN_RESET);

}

void Power_Boost_Enable(bool state){

    if (state)
        HAL_GPIO_WritePin(Bust_En_GPIO_Port, Bust_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(Bust_En_GPIO_Port, Bust_En_Pin, GPIO_PIN_RESET);

}

void Power_Boost_Enable_12V(bool state){

    if (state)
        HAL_GPIO_WritePin(Vout_En_12V_GPIO_Port, Vout_En_12V_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(Vout_En_12V_GPIO_Port, Vout_En_12V_Pin, GPIO_PIN_RESET);

}


void Power_USB_Enable(bool state){

    if (state)
        HAL_GPIO_WritePin(USB_En_GPIO_Port, USB_En_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(USB_En_GPIO_Port, USB_En_Pin, GPIO_PIN_RESET);

}


void Power_System_On(bool state){

    if (state)
        HAL_GPIO_WritePin(SYS_On_GPIO_Port, SYS_On_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(SYS_On_GPIO_Port, SYS_On_Pin, GPIO_PIN_RESET);

}