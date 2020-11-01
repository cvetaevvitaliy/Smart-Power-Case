/**
 * @brief Command Line Interface for microcontrollers
 *  
 * Author: Vitalii Nimych
 *
 * */

#ifndef _CLI_H_
#define _CLI_H_

#include "cli_config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "tinyprintf.h"


/** @brief Terminal Error Result Execute */
typedef enum{ 
	TE_OK = 0, 
	TE_Err, 
	TE_NotFound, 
	TE_ArgErr, 
	TE_ExecErr, 
	TE_WorkInt
}TE_Result_e;

/** @brief Terminal Add Command Result */
typedef enum{ 
	TA_OK = 0, 
	TA_MaxCmd, 
	TA_FcnNull, 
	TA_EmptyName, 
	TA_RetryName
}TA_Result_e;

/** @brief Terminal Char Append Result */
typedef enum{ 
	TC_OK = 0, 
	TC_Enter, 
	TC_BufFull, 
	TC_Reset, 
	TC_Ignore
}TC_Result_e; 

/** @brief Terminal Default Init Command */
typedef enum{
	TDC_None = 0x0000,
	TDC_Time = 0x0001,
	TDC_CPU = 0x0002,
	TDC_All = 0xFFFF
}TypeDefaultCmd_e;

/** @brief Terminal Mode Execute Command */
typedef enum{
	TMC_None = 0x0000,
	TMC_PrintStartTime = 0x0001,
	TMC_PrintStopTime = 0x0002,
	TMC_PrintDiffTime = 0x0004,
	TMC_All = 0xFFFF,
}TypeModeCmd_e;


bool CLI_GetIntState(void);
#define CLI_CheckAbort()			{ if (CLI_GetIntState()){return TE_WorkInt;}}

/** @brief Terminal initialize */
void CLI_Init(TypeDefaultCmd_e defCmd);

/** @brief Execute command */
bool CLI_Service();


/**
 * @brief Function for Add command
 *  @param name - input name
 * @param fcn - callback function
 * @param argc - min count arguments
 * @param mode - execute mode
 * @param descr - description
 * @return result append command
 * */
TA_Result_e CLI_AddCmd(const char* name, uint8_t (*fcn)(), uint8_t argc, uint16_t mode, const char* descr);


/** @brief Append new symbols */
TC_Result_e CLI_AppendChar(char c);

/** @brief For check arguments flag */
bool CLI_IsArgFlag(const char* flag);

/** @brief Check convert string arguments to Dec */
bool CLI_GetArgDecByFlag(const char* flag, uint32_t* outValue);

/** @brief Check convert string arguments to Hex */
bool CLI_GetArgHexByFlag(const char* flag, uint32_t* outValue);

/** @brief Get argument in str */
int CLI_GetArgStr(uint8_t index, char* str);


/** @brief Convert and Get argument in Dec 
 * @param index argument
 * @return results conversion
*/
uint32_t CLI_GetArgDec(uint8_t index);

/** @brief Convert and Get argument in Hex 
 * @param index argument
 * @return results conversion
*/
uint32_t CLI_GetArgHex(uint8_t index);


void CLI_Set_First_IN(bool set);

void SysTick_CLI(void);

#endif // _CLI_H_
