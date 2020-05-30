/********************************
* Created by Vitaliy on 5/5/20.
********************************/

#ifndef FATSHARK_POWER_V1_HDQ_H
#define FATSHARK_POWER_V1_HDQ_H

#include "dwt_stm32_delay.h"
#include "stdbool.h"
/*************************
 * Read/write command mask
**************************/
#define HDQ_ADDR_MASK_READ      0x00
#define HDQ_ADDR_MASK_WRITE     0x80

/**************************************************
 * This is the number of times the slave wait loop
 * will run before we time out.
 * As far as I can tell the loop uses ~6 instructions
 * thus giving about 200uS delay which is a full bit write
***************************************************/
#define HDQ_DELAY_FAIL_TRIES    300

/**************************
 * HDQ Default timings
**************************/
#define HDQ_DELAY_THW1			45	/* 32 - 66uS  */
#define HDQ_DELAY_THW0			120	/* 70 - 145uS */

#define HDQ_DELAY_TDW1			48	/* 0.5 - 50us */
#define HDQ_DELAY_TDW0			110	/* 86 - 145uS */
#define HDQ_DELAY_TCYCD			205	/* 190 min    */

#define HDQ_DELAY_TB            250 /* Min: 190uS */
#define HDQ_DELAY_TBR           70  /* Min: 40uS  */
#define HDQ_DELAY_TCYCH         250 /* Min: 190uS Max: 250uS */

#define HDQ_DELAY_TRSPS_MAX     320 /* Max: 320uS */
#define HDQ_DELAY_BIT_TOTAL     200



void HDQ_doBreak(void);
void HDQ_writeByte(uint8_t payload);
bool HDQ_write(uint8_t reg, uint8_t payload);
bool HDQ_write_Verif(uint8_t reg, uint8_t payload, bool verif);
uint8_t HDQ_read(uint8_t reg);



#endif //FATSHARK_POWER_V1_HDQ_H
