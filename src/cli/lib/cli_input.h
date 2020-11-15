#ifndef _CLI_INPUT_H_
#define _CLI_INPUT_H_

#include "cli_config.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	char keyCode;
	bool isValid;
	bool isAlphaBet;
}CLI_InputValue_t;

typedef enum
{
	MainBuffer		= 0x00,
	TransitBuffer	= 0x01
}CLI_InputBufferType_t;

void CLI_INPUT_Init();

char* CLI_INPUT_GetBuffer(CLI_InputBufferType_t type);
void CLI_INPUT_SetBuffer(CLI_InputBufferType_t type, char* buffer, uint32_t len);

bool CLI_INPUT_IsEmpty();
bool CLI_INPUT_IsFull();

void CLI_INPUT_RemChar();
void CLI_INPUT_AddChar(char c);

void CLI_INPUT_Cache();
void CLI_INPUT_Reset();

char CLI_INPUT_GetLastChar();

void CLI_INPUT_Refresh(const char* newCmd);

CLI_InputValue_t CLI_INPUT_PutChar(char c);

void CLI_INPUT_Delete();
void CLI_INPUT_Backspace();

void CLI_INPUT_CursorToHome();
void CLI_INPUT_CursorToEnd();
void CLI_INPUT_CursorTo(uint16_t pos);
void CLI_INPUT_CursorToLeft();
void CLI_INPUT_CursorToRight();

void CLI_INPUT_CursorShift(int16_t shift);

#endif // _CLI_INPUT_H_
