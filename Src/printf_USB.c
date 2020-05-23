//
// Created by Vitaliy on 4/28/20.
//
#include "printf_USB.h"

#ifdef USE_USB_DEBUG_PRINTF


int _write(int file,char *ptr, int len)
{
   (CDC_Transmit_FS((uint8_t*)ptr, len));
   return len;

}

void clrscr(void){
    printf("\033[2J"); /* Clear the entire screen. */
    printf("\033[0;0f"); /* Move cursor to the top left hand corner */
}

#endif
