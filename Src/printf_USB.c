//
// Created by Vitaliy on 4/28/20.
//
#include "printf_USB.h"

#ifdef USE_USB_DEBUG

volatile bool usb_detect = false;

//int _write(int file,char *ptr, int len)
//{
//    if (usb_detect) {
//        CDC_Transmit_FS((uint8_t *) ptr, len);
//        HAL_Delay(5);
//    }
//   return len;
//
//}

//void clrscr(void){
//    printf("\033[2J"); /* Clear the entire screen. */
//    printf("\033[0;0f"); /* Move cursor to the top left hand corner */
//}

void Enable_USB_Debug(bool enable){
    usb_detect = enable;

}

bool Get_Enable_USB()
{
    return usb_detect;
}

#endif
