/********************************************************************************
 * @brief    Implementation syscall
 * Author: Vitalii Nimych
 ********************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include "usb_device.h"

#undef errno
int errno;
int  _end;

/*Low layer write(output) function*/
/*!< Redirecting standard stdio printf function to Lego UART */
//__attribute__ ((used))
//int _write(int file, const char * p_char, int len)
//{
//    for (int i = 0; i < len; i++)
//        while (CDC_Transmit_FS((uint8_t *) p_char, len) != USBD_OK) {}
//
//    return len;
//}

/*Low layer read(input) function*/
/*!< Redirecting standard stdio getchar(), scanf() function from Lego UART */
//__attribute__ ((used))
//int _read(int file, char * p_char, int len)
//{
////    if (usb_detect) {
////        while (CDC_Transmit_FS((uint8_t *) p_char, len) != USBD_OK) {}
////    }
//    while (CDC_Receive_FS((uint8_t *) p_char, (uint32_t *) len) != USBD_OK) {}
//
//    return 1;
//}

/*This function is used for handle heap option*/
__attribute__ ((used))
caddr_t _sbrk ( int incr )
{
    static unsigned char *heap = NULL;
    unsigned char *prev_heap;

    if (heap == NULL) {
        heap = (unsigned char *)&_end;
    }
    prev_heap = heap;

    heap += incr;

    return (caddr_t) prev_heap;
}

__attribute__ ((used))
int link(char *old, char *new)
{
    return -1;
}

__attribute__ ((used))
int _close(int file)
{
    return -1;
}

__attribute__ ((used))
int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

__attribute__ ((used))
int _isatty(int file)
{
    return 1;
}

__attribute__ ((used))
int _lseek(int file, int ptr, int dir)
{
    return 0;
}

__attribute__ ((used))
void abort(void)
{
    /* Abort called */
    while(1);
}
