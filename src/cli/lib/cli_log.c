#include "cli_log.h"
#include "cli_string.h"


static struct{
	char cmds[CLI_CMD_LOG_SIZE][CLI_CMD_BUF_SIZE];
	int8_t _curCmd;
	int8_t _cntCmd;
}TermLog_s;

void CLI_LogInit()
{
    TermLog_s._cntCmd = 0;
    CLI_CurReset();
	
	for(uint8_t i = 0; i < CLI_CMD_LOG_SIZE; i++)
	{
        TermLog_s.cmds[i][0] = '\0';
	}
}

void CLI_LogCmdPush(const char* cmd)
{
	if (TermLog_s._cntCmd < CLI_CMD_LOG_SIZE)
	{
		if (TermLog_s._cntCmd > 0)
		{
			if (_strcmp(cmd, (const char*) TermLog_s.cmds[TermLog_s._cntCmd - 1]) == 0)
			{
				cli_memcpy(TermLog_s.cmds[TermLog_s._cntCmd], cmd, CLI_CMD_BUF_SIZE);
				TermLog_s._cntCmd++;
			}
		}
		else
		{
			cli_memcpy(TermLog_s.cmds[TermLog_s._cntCmd], cmd, CLI_CMD_BUF_SIZE);
			TermLog_s._cntCmd++;
		}
	}
	else
	{
		if (_strcmp(cmd, (const char*) TermLog_s.cmds[TermLog_s._cntCmd - 1]) == 0)
		{
			cli_memcpy(&TermLog_s.cmds[0][0], &TermLog_s.cmds[1][0], CLI_CMD_BUF_SIZE * (CLI_CMD_LOG_SIZE - 1));
			cli_memcpy(&TermLog_s.cmds[TermLog_s._cntCmd - 1][0], cmd, CLI_CMD_BUF_SIZE);
            TermLog_s._cntCmd = CLI_CMD_LOG_SIZE;
		}
	}
}

const char* CLI_LogCmdGet(uint8_t index)
{
	if (index < CLI_CMD_LOG_SIZE)
	{
		return &TermLog_s.cmds[index][0];
	}

	return NULL;
}

const char* CLI_GetNextCmd()
{
	if (TermLog_s._curCmd < TermLog_s._cntCmd - 1)
	{
		TermLog_s._curCmd++;
		return &TermLog_s.cmds[TermLog_s._curCmd][0];
	}

	return NULL;
}

const char* CLI_GetLastCmd()
{
	if (TermLog_s._curCmd > 0)
	{
		TermLog_s._curCmd--;
		return &TermLog_s.cmds[TermLog_s._curCmd][0];
	}

	return NULL;
}

void CLI_CurReset()
{
    TermLog_s._curCmd = TermLog_s._cntCmd;
}
