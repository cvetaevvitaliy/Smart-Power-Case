#include "stdint.h"


#ifndef Fonts
#define Fonts

//
//	Structure om font te definieren
//
typedef struct {
	const uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} FontDef;


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


extern FontDef Font_7x10;
extern FontDef Font_10x15;
extern FontDef Font_11x18;
extern FontDef Font_13x19;
extern FontDef Font_14x22;
extern FontDef Font_16x25;
extern FontDef Font_16x26;



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


extern const tImage Image_B;
extern const tImage Image_A;
extern const tImage Image_Active_A_Ch;
extern const tImage Image_inActive_A_Ch;
extern const tImage Image_Active_B_Ch;
extern const tImage Image_inActive_B_Ch;
extern const tImage image_Big_A;
extern const tImage image_Big_B;
extern const tImage image_Big_D;
extern const tImage image_Big_E;
extern const tImage image_Big_F;
extern const tImage image_Big_R;

extern const tImage Image_Trash;
extern const tImage Image_Settings;
extern const tImage Image_Scan;
extern const tImage Image_Exit;
extern const tImage Image_Calibrate;
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
extern const tImage Image_Temperature_Ico;
extern const tImage Image_Vout;
extern const tImage Image_Set_12V;
extern const tImage Image_Set_8V;

extern const tImage Image_Frame_22x20;

extern const tImage Image_Buzzer_On_Ico;
extern const tImage Image_Buzzer_Off_Ico;

#endif

