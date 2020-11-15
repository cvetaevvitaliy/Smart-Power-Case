#ifndef FONTS_H_
#define FONTS_H_
#include "stdint.h"


typedef struct {
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;


typedef struct {
    long int code;
    const tImage *image;
} tChar;

typedef struct {
    int length;
    const tChar *chars;
} tFont;


extern const tFont Font_8x10;
extern const tFont Font_13x16;
extern const tFont Font_Let_21x21;
extern const tFont Font_Let_15x21;
extern const tFont Font_25x27;
extern const tFont Font_24x17;
extern const tFont Font_20x24;
extern const tFont Font_LET_18x26;
extern const tFont Font_16x24;
extern const tFont Font_10x15_2;
extern const tFont Font_11x22;
extern const tFont Font_10x20;
extern const tFont Font_Let_9x13;
extern const tFont Font_Let_Inv_9x13;



extern const tImage Image_Frame;

extern const tImage Image_Set_Low_Volt;
extern const tImage Image_Frame_27x27;
extern const tImage Image_Set_I_Max;
extern const tImage Image_Set_V_Out;
extern const tImage Image_Buzzer;
extern const tImage Image_Set_Timer_Off;
extern const tImage Image_Set_Capasity_Design;
extern const tImage Image_Get_Status_Info;
extern const tImage Image_Exit_Menu;
extern const tImage Image_Apply;

extern const tImage Image_Used_mAh;
extern const tImage Image_Current_I;
extern const tImage Image_Remaining_Time;
extern const tImage Image_Power_On_Time;
extern const tImage Image_Vout;
extern const tImage Image_Set_12V;
extern const tImage Image_Set_8V;
extern const tImage Image_Battery_Status;

extern const tImage Image_Frame_22x20;

extern const tImage Image_Buzzer_On_Ico;
extern const tImage Image_Buzzer_Off_Ico;

extern const tImage Image_Battery_Life_Ico;
extern const tImage Image_Battery_Type_Ico;
extern const tImage Image_Charge_Count_Ico;

extern const tImage Image_USB_Ico;
extern const tImage Image_5V_Ico;
extern const tImage Image_QC_Ico;
extern const tImage Image_Err_Ico;
extern const tImage Image_Current_mAh_Ico;
extern const tImage Image_Full_mAh_Ico;
extern const tImage Image_Current_In;

#endif

