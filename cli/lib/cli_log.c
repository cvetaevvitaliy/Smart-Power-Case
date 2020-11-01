#include "cli_log.h"

#include "cli_string.h"

extern uint8_t _strcmp(const char* str1, const char* str2);

struct{
	char cmds[CLI_CMD_LOG_SIZE][CLI_CMD_BUF_SIZE];
	int8_t _curCmd;
	int8_t _cntCmd;
}TermLog;

void CLI_LogInit()
{
	TermLog._cntCmd = 0;
    CLI_CurReset();
	
	for(uint8_t i = 0; i < CLI_CMD_LOG_SIZE; i++)
	{
		TermLog.cmds[i][0] = '\0';
	}
}

void CLI_LogCmdPush(const char* cmd)
{
	if (TermLog._cntCmd < CLI_CMD_LOG_SIZE)
	{
		if (TermLog._cntCmd > 0)
		{
			if (_strcmp(cmd, (const char*) TermLog.cmds[TermLog._cntCmd-1]) == 0)
			{
				cli_memcpy(TermLog.cmds[TermLog._cntCmd], cmd, CLI_CMD_BUF_SIZE);
				TermLog._cntCmd++;
			}
		}
		else
		{
			cli_memcpy(TermLog.cmds[TermLog._cntCmd], cmd, CLI_CMD_BUF_SIZE);
			TermLog._cntCmd++;
		}
	}
	else
	{
		if (_strcmp(cmd, (const char*) TermLog.cmds[TermLog._cntCmd-1]) == 0)
		{
			cli_memcpy(&TermLog.cmds[0][0], &TermLog.cmds[1][0], CLI_CMD_BUF_SIZE*(CLI_CMD_LOG_SIZE - 1));
			cli_memcpy(&TermLog.cmds[TermLog._cntCmd-1][0], cmd, CLI_CMD_BUF_SIZE);
			TermLog._cntCmd = CLI_CMD_LOG_SIZE;
		}
	}
}

const char* CLI_LogCmdGet(uint8_t index)
{
	if (index < CLI_CMD_LOG_SIZE)
	{
		return &TermLog.cmds[index][0];
	}

	return NULL;
}

const char* CLI_GetNextCmd()
{
	if (TermLog._curCmd < TermLog._cntCmd - 1)
	{
		TermLog._curCmd++;
		return &TermLog.cmds[TermLog._curCmd][0];
	}

	return NULL;
}

const char* CLI_GetLastCmd()
{
	if (TermLog._curCmd > 0)
	{
		TermLog._curCmd--;
		return &TermLog.cmds[TermLog._curCmd][0];
	}

	return NULL;
}

void CLI_CurReset()
{
	TermLog._curCmd = TermLog._cntCmd;
}
