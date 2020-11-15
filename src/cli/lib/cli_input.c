#include "cli_input.h"

#include "cli_queue.h"
#include "cli_string.h"

#define INPUT_COUNT_BUFFER		2

typedef struct
{
	char Data[CLI_CMD_BUF_SIZE + 1];				// buffer
	int16_t CursorInBuffer;							// cursos position
	int16_t BufferCount;							// count entered symbols
}CLI_Buffer_t;

static struct
{
	CLI_Buffer_t Buffers[INPUT_COUNT_BUFFER];			// buffers commands
	CLI_Buffer_t* CurBuffer;
	CLI_Queue_t Symbols;								// queue symbols input
	CLI_InputBufferType_t CurrentBuffer;				// current proccesing buffer
}CLI_Input_s;

static void _AddChar(char c)
{
    CLI_PutChar(c);

    CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->CursorInBuffer] = c;
	CLI_Input_s.CurBuffer->BufferCount++;
	CLI_Input_s.CurBuffer->CursorInBuffer++;
    CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->BufferCount] = '\0';
}

static void _RemChar()
{
    CLI_PutChar(TERM_KEY_BACKSPACE);
    CLI_PutChar(' ');
    CLI_PutChar(TERM_KEY_BACKSPACE);
    
	CLI_Input_s.CurBuffer->CursorInBuffer--;
	CLI_Input_s.CurBuffer->BufferCount--;
    CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->BufferCount] = '\0';
}

void CLI_INPUT_Refresh(const char* newCmd)
{
    CLI_PutChar('\r');
    CLI_Printf(STRING_TERM_ARROW);

	if (CLI_Input_s.CurBuffer->Data != newCmd)
	{
		uint32_t lenNewCmd = _strlen(newCmd);
		uint32_t lenCurCmd = CLI_Input_s.CurBuffer->BufferCount;
		cli_memcpy(CLI_Input_s.CurBuffer->Data, newCmd, lenNewCmd);

        CLI_Input_s.CurBuffer->BufferCount = lenNewCmd;
        CLI_Input_s.CurBuffer->CursorInBuffer = lenNewCmd;

		for(uint8_t i = 0; i < lenNewCmd; i++)
		{
            CLI_PutChar(CLI_Input_s.CurBuffer->Data[i]);
		}

		uint8_t cntSpcChar = 0;
		for(uint8_t i = lenNewCmd; i < lenCurCmd; i++)
		{
            CLI_PutChar(' ');
			cntSpcChar++;
		}
		
		for(uint8_t i = 0; i < cntSpcChar; i++)
        	{ CLI_PutChar(TERM_KEY_BACKSPACE);}
        
#if 0
        LOG_DEBUG("\r\nNewCmd: %s", newCmd);
		LOG_DEBUG("\r\nlenNewCmd: %d", lenNewCmd);
#endif

	}
	else
	{
        //LOG_DEBUG("%s",Input.CurBuffer->Data);
		for(uint8_t i = 0; i < CLI_Input_s.CurBuffer->BufferCount; i++)
		{
            CLI_PutChar(CLI_Input_s.CurBuffer->Data[i]);
		}
	}
}

bool CLI_INPUT_IsEmpty()
{
	return CLI_Input_s.CurBuffer->BufferCount == 0;
}

bool CLI_INPUT_IsFull()
{
	return CLI_Input_s.CurBuffer->BufferCount >= CLI_CMD_BUF_SIZE;
}

void CLI_INPUT_RemChar()
{
	#if (CLI_LR_KEY_EN == 1)

	if (CLI_Input_s.CurBuffer->CursorInBuffer != CLI_Input_s.CurBuffer->BufferCount)
	{
		// save current position cursor
		uint8_t tmpPos = CLI_Input_s.CurBuffer->CursorInBuffer - 1;

		cli_memcpy(CLI_Input_s.Buffers[TransitBuffer].Data, CLI_Input_s.CurBuffer->Data, tmpPos);
		cli_memcpy(CLI_Input_s.Buffers[TransitBuffer].Data + tmpPos, CLI_Input_s.CurBuffer->Data + tmpPos + 1, CLI_Input_s.CurBuffer->BufferCount - tmpPos);

        CLI_Input_s.Buffers[TransitBuffer].Data[CLI_Input_s.CurBuffer->BufferCount - 1] = '\0';

        CLI_INPUT_Refresh(CLI_Input_s.Buffers[TransitBuffer].Data);

		for(uint8_t pos = 0; pos < CLI_Input_s.CurBuffer->BufferCount - tmpPos; pos++)
		{
            CLI_PutChar(TERM_KEY_LSHIFT);
			CLI_Input_s.CurBuffer->CursorInBuffer--;
		}
	}
	else
	{
        _RemChar();
	}
#else
    _RemChar();
#endif
}

void CLI_INPUT_AddChar(char c)
{
#if (CLI_LR_KEY_EN == 1)
	if (CLI_Input_s.CurBuffer->CursorInBuffer != CLI_Input_s.CurBuffer->BufferCount)
	{
		uint8_t tmpPos = CLI_Input_s.CurBuffer->CursorInBuffer;
		cli_memcpy(CLI_Input_s.Buffers[TransitBuffer].Data, CLI_Input_s.CurBuffer->Data, tmpPos);
		cli_memcpy(CLI_Input_s.Buffers[TransitBuffer].Data + tmpPos, &c, 1);
		cli_memcpy(CLI_Input_s.Buffers[TransitBuffer].Data + tmpPos + 1, CLI_Input_s.CurBuffer->Data + tmpPos, CLI_Input_s.CurBuffer->BufferCount - tmpPos);
        CLI_Input_s.Buffers[TransitBuffer].Data[CLI_Input_s.CurBuffer->BufferCount + 1] = '\0';
		
		CLI_Input_s.CurBuffer->BufferCount++;
		CLI_Input_s.CurBuffer->CursorInBuffer++;
        CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->BufferCount] = '\0';

		tmpPos++;
        CLI_INPUT_Refresh(CLI_Input_s.Buffers[TransitBuffer].Data);

		for(uint8_t pos = 0; pos < CLI_Input_s.CurBuffer->BufferCount - tmpPos; pos++)
		{
            CLI_PutChar(TERM_KEY_LSHIFT);
			CLI_Input_s.CurBuffer->CursorInBuffer--;
		}
	}
	else
	{
        _AddChar(c);
	}

#else
	_AddChar(c);
#endif	
}

void CLI_INPUT_Init()
{
	for(uint32_t i = 0; i < INPUT_COUNT_BUFFER; i++)
	{
        CLI_Input_s.Buffers[i].Data[0] = '\0';
        CLI_Input_s.Buffers[i].BufferCount = 0;
        CLI_Input_s.Buffers[i].CursorInBuffer = 0;
	}

    CLI_Input_s.CurBuffer = &CLI_Input_s.Buffers[MainBuffer];

    Q_Init(&CLI_Input_s.Symbols, 3, sizeof(char), QUEUE_FORCED_PUSH_POP_Msk);
    
    for(uint8_t i = 0; i < 3; i++)
    {
    	char c = 0;
        Q_Push(&CLI_Input_s.Symbols, &c);
	}
}

uint8_t arr_up[]	= {0x1B, 0x5B, 0x41};
uint8_t arr_down[]	= {0x1B, 0x5B, 0x42};
uint8_t arr_right[] = {0x1B, 0x5B, 0x43};
uint8_t arr_left[]	= {0x1B, 0x5B, 0x44};
uint8_t arr_esc[]	= {0x1B, 0x1B, 0x1B};
uint8_t del[]		= {0x1B, 0x5B, 0x33};
uint8_t home[]		= {0x1B, 0x5B, 0x31};
uint8_t end[]		= {0x1B, 0x5B, 0x34};

CLI_InputValue_t CLI_INPUT_PutChar(char c) {
    CLI_InputValue_t iv;

    Q_Push(&CLI_Input_s.Symbols, &c);

    if (Q_IsEqual(&CLI_Input_s.Symbols, arr_up, 3))           { c = TERM_KEY_UP; }
    else if (Q_IsEqual(&CLI_Input_s.Symbols, arr_down, 3))    { c = TERM_KEY_DOWN; }
    else if (Q_IsEqual(&CLI_Input_s.Symbols, arr_right, 3))   { c = TERM_KEY_RIGHT; }
    else if (Q_IsEqual(&CLI_Input_s.Symbols, arr_left, 3))    { c = TERM_KEY_LEFT; }
    else if (Q_IsEqual(&CLI_Input_s.Symbols, arr_esc, 3))     { c = TERM_KEY_ESCAPE; }
    else if (Q_IsEqual(&CLI_Input_s.Symbols, del, 3))         { c = TERM_KEY_DEL; }
    else if (Q_IsEqual(&CLI_Input_s.Symbols, home, 3))        { c = TERM_KEY_HOME; }
    else if (Q_IsEqual(&CLI_Input_s.Symbols, end, 3))         { c = TERM_KEY_END; }

    iv.isValid = ((CLI_Input_s.CurBuffer->BufferCount < CLI_CMD_BUF_SIZE) ||
                  (c == TERM_KEY_BACKSPACE) ||
                  (c == TERM_KEY_ENTER));

    iv.isAlphaBet = (((c >= '0') && (c <= '9')) ||
                     ((c >= 'a') && (c <= 'z')) ||
                     ((c >= 'A') && (c <= 'Z')) ||
                     (c == 0x20) || (c == '_') ||
                     (c == '-') || (c == 0x03) ||
                     (c == '$') || (c == '<'));

    iv.keyCode = c;
    return iv;
}

void CLI_INPUT_Cache()
{
    CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->BufferCount] = '\0';
	cli_memcpy(CLI_Input_s.Buffers[TransitBuffer].Data, CLI_Input_s.CurBuffer->Data, CLI_Input_s.CurBuffer->BufferCount + 1);
}

void CLI_INPUT_Reset()
{
    CLI_Input_s.CurBuffer->CursorInBuffer = CLI_Input_s.CurBuffer->BufferCount = 0;
    CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->BufferCount] = '\0';
}

char CLI_INPUT_GetLastChar()						{ return CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->BufferCount - 1];	}

void CLI_INPUT_CursorTo(uint16_t pos)				{ CLI_Input_s.CurBuffer->CursorInBuffer = pos; }

void CLI_INPUT_CursorShift(int16_t shift)			{ CLI_Input_s.CurBuffer->CursorInBuffer += shift; }

char* CLI_INPUT_GetBuffer(CLI_InputBufferType_t type)	{ return CLI_Input_s.Buffers[type].Data; }

void CLI_INPUT_SetBuffer(CLI_InputBufferType_t type, char* buffer, uint32_t len)
{
	cli_memcpy(CLI_Input_s.Buffers[type].Data, buffer, len);
    CLI_Input_s.CurBuffer->BufferCount = CLI_Input_s.CurBuffer->CursorInBuffer = len;
}

void CLI_INPUT_CursorToHome()
{
	while(CLI_Input_s.CurBuffer->CursorInBuffer > 0)
	{
        CLI_PutChar(TERM_KEY_LSHIFT);
        CLI_INPUT_CursorShift(-1);
	}
}

void CLI_INPUT_CursorToEnd()
{
	while(CLI_Input_s.CurBuffer->CursorInBuffer < CLI_Input_s.CurBuffer->BufferCount)
	{
        CLI_PutChar(CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->CursorInBuffer]);
        CLI_INPUT_CursorShift(1);
	}
}

void CLI_INPUT_CursorToLeft()
{
	if (CLI_Input_s.CurBuffer->CursorInBuffer > 0)
	{
        CLI_INPUT_CursorShift(-1);
        CLI_PutChar(TERM_KEY_LSHIFT);
	}
}

void CLI_INPUT_CursorToRight()
{
	if (CLI_Input_s.CurBuffer->CursorInBuffer < CLI_Input_s.CurBuffer->BufferCount)
	{
        CLI_PutChar(CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->CursorInBuffer]);
        CLI_INPUT_CursorShift(1);
	}	
}

void CLI_INPUT_Delete()
{
	if ((CLI_Input_s.CurBuffer->CursorInBuffer != CLI_Input_s.CurBuffer->BufferCount) && (!CLI_INPUT_IsEmpty()))
	{
        CLI_INPUT_CursorShift(1);
		if(CLI_Input_s.CurBuffer->CursorInBuffer != CLI_Input_s.CurBuffer->BufferCount)
		{
            CLI_PutChar(CLI_Input_s.CurBuffer->Data[CLI_Input_s.CurBuffer->CursorInBuffer - 1]);
		}
        CLI_INPUT_RemChar();
	}	
}

void CLI_INPUT_Backspace()
{
	if (!CLI_INPUT_IsEmpty() && (CLI_Input_s.CurBuffer->CursorInBuffer > 0))
        CLI_INPUT_RemChar();
}
