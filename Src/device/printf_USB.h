//
// Created by Vitaliy on 4/28/20.
//
#ifndef FATSHARK_POWER_V1_PRINTF_USB_H
#define FATSHARK_POWER_V1_PRINTF_USB_H
#include "main.h"
#ifdef USE_USB_DEBUG
#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "stdint.h"
#include "stdbool.h"


void Enable_USB_Debug(bool enable);
bool Get_Enable_USB();
#endif
void clrscr(void);


#endif //FATSHARK_POWER_V1_PRINTF_USB_H
