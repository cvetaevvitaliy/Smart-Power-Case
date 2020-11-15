#ifndef __MAIN_H__
#define __MAIN_H__
#include "tinyprintf.h"
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

#define Button_Menu_Pin GPIO_PIN_1
#define ButtonMenu_GPIO_Port GPIOA
#define ButtonMenu_EXTI_IRQn EXTI1_IRQn
#define Button_Select_Pin GPIO_PIN_2
#define ButtonSelect_GPIO_Port GPIOA
#define ButtonSelect_EXTI_IRQn EXTI2_IRQn
#define USB_En_Pin GPIO_PIN_4
#define USB_En_GPIO_Port GPIOA
#define Bust_En_Pin GPIO_PIN_5
#define Bust_En_GPIO_Port GPIOA
#define CHG_Pin GPIO_PIN_7
#define CHG_GPIO_Port GPIOA
#define CHG_EXTI_IRQn EXTI9_5_IRQn
#define PWM_Buzz_Pin GPIO_PIN_8
#define PWM_Buzz_GPIO_Port GPIOA
#define Vbus_detect_Pin GPIO_PIN_15
#define Vbus_detect_GPIO_Port GPIOA
#define Vbus_detect_EXTI_IRQn EXTI15_10_IRQn
#define LCD_En_Pin GPIO_PIN_3
#define LCD_En_GPIO_Port GPIOB
#define SYS_On_Pin GPIO_PIN_4
#define SYS_On_GPIO_Port GPIOB
#define Vout_En_12V_Pin GPIO_PIN_5
#define Vout_En_12V_GPIO_Port GPIOB


void _Error_Handler(char *, int);

#endif
