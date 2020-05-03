//
// Created by Vitaliy on 4/28/20.
//
#include "printf_USB.h"

#ifdef USE_USB_DEBUG_PRINTF


int _write(int file,char *ptr, int len)
{
   CDC_Transmit_FS((uint8_t*)ptr, len);
   return len;
}

#endif
