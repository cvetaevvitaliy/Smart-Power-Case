#ifndef __EEPROMCONFIG_H
#define __EEPROMCONFIG_H


//#define   _EEPROM_F1_LOW_DESTINY  // 32 kb flash
#define   _EEPROM_F1_MEDIUM_DESTINY // 64 to 128 kb flash
//#define   _EEPROM_F1_HIGH_DESTINY // 256 to 512 kb flash

#if defined(STM32F103TBU) // STM32F103TBU - 128kb flash, write in last 127 page
#define		_EEPROM_USE_FLASH_PAGE				127
#else // STM32F103T8U 64kb flash, write in last 63 page
#define		_EEPROM_USE_FLASH_PAGE				63
#endif
#endif
