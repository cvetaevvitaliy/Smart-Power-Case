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


/** @brief CLI Error Result Execute */
typedef enum{ 
	CLI_OK = 0,
	CLI_Err,
	CLI_NotFound,
	CLI_ArgErr,
	CLI_ExecErr,
	CLI_WorkInt
}CLI_Result_t;

/** @brief CLI Add Command Result */
typedef enum{ 
	ADD_CMD_OK = 0,
	ADD_CMD_MaxCmd,
	ADD_CMD_FcnNull,
	ADD_CMD_EmptyName,
	ADD_CMD_RetryName
}CLI_ADD_CMD_Result_t;

/** @brief CLI Char Append Result */
typedef enum{ 
	CLI_APPEND_OK = 0,
    CLI_APPEND_Enter,
    CLI_APPEND_BufFull,
    CLI_APPEND_Reset,
    CLI_APPEND_Ignore
}CLI_Append_Result_t;

/** @brief CLI Default Init Command */
typedef enum{
	CLI_TDC_None = 0x0000,
    CLI_TDC_Time = 0x0001,
    CLI_TDC_CPU = 0x0002,
    CLI_TDC_All = 0xFFFF
}CLI_TypeDefaultCmd_t;

/** @brief CLI Mode Execute Command */
typedef enum{
	CLI_PrintNone = 0x0000,
	CLI_PrintStartTime = 0x0001,
	CLI_PrintStopTime = 0x0002,
	CLI_PrintDiffTime = 0x0004,
	CLI_Print_All = 0xFFFF,
}CLI_TypeModeCmd_t;


bool CLI_GetIntState(void);
#define CLI_CheckAbort()			{ if (CLI_GetIntState()){return TE_WorkInt;}}

/** @brief Terminal initialize */
void CLI_Init(CLI_TypeDefaultCmd_t defCmd);

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
CLI_ADD_CMD_Result_t CLI_AddCmd(const char* name, uint8_t (*fcn)(), uint8_t argc, uint16_t mode, const char* descr);


/** @brief Append new symbols */
CLI_Append_Result_t CLI_AppendChar(char c);

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
