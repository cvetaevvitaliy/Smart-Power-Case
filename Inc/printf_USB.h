//
// Created by Vitaliy on 4/28/20.
//
#ifndef FATSHARK_POWER_V1_PRINTF_USB_H
#define FATSHARK_POWER_V1_PRINTF_USB_H
#include "main.h"
#ifdef USE_USB_DEBUG_PRINTF
#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#endif

void clrscr(void);


#endif //FATSHARK_POWER_V1_PRINTF_USB_H
