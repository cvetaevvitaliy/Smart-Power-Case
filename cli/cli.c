/**
 * @brief Command Line Interface for microcontrollers
 * 
 * Author: Vitalii Nimych
 *
 * */

#include "cli.h"

#include "cli_queue.h"
#include "cli_string.h"
#include "cli_time.h"
#include "cli_log.h"
#include "cli_input.h"
#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#define printArrow()			{CLI_Printf("%s%s",STRING_TERM_ENTER, STRING_TERM_ARROW);}	// Output of input line
#define printArrowWithoutN()	{CLI_Printf(STRING_TERM_ARROW);}

static void PrintResultExec(uint8_t code);
static void PrintResultAddCmd(uint8_t code);
static void PrintTime(CLI_Time_s *time);
static int8_t IndexOfFlag(const char *flag);

static void CLI_PrintTime();
void CLI_PrintTimeWithoutRN();
char dbgbuffer[128];
volatile uint64_t SysTic;                                        // Variable tackts cntr


static uint8_t _help_cmd();
static uint8_t reboot_mcu();
static uint8_t print_cli_w(void);


// ************************* interrupt function ***************************

static bool _interrupt_operation = false;

/** @brief Checking the status of the start of the operation (return, stator-on) */
inline bool CLI_GetIntState()
{

	bool res = _interrupt_operation;
	_interrupt_operation = false;
	return res;
}

// ************************************************************************

// *********************** Terminal fcns **********************************

/** @brief Command settings */
typedef struct
{
	uint8_t (*fcn)();		 // callback function command
	const char *name;		 // name command
	uint8_t argc;			 // min count argument
	uint16_t mode;			 // mode execute command
	const char *description; // description command
} TermCmd_s;

/** @brief Terminal State */
struct
{
	TermCmd_s cmds[CLI_SIZE_TASK]; // list commands
	uint8_t countCommand;		   // count commands
	uint8_t executeState;		   // state terminal
	volatile Params_s inputArgs;   // args current execute command
	bool isEntered;				   //
	bool first_in;
} Terminal;

TermCmd_s *_findTermCmd(const char *cmdName);

static void cli_welcome(void)
{
	CLI_Printf("\r\n****************************************************");
	CLI_Printf("\r\n| CLI Smart Power Case                             |");
	CLI_Printf("\r\n| SW  ver: v%s.%s.%s   MCU: %s \t\t   |", MINOR, MAJOR, PATCH, MCU);
	CLI_Printf("\r\n| CLI ver: %s \t\t\t\t   |", _TERM_VER_);
	CLI_Printf("\r\n| Build Date: %s %s \t\t   |", __DATE__, __TIME__);
	CLI_Printf("\r\n| Branch: %s GIT-HASH: %s\t\t   |", GIT_BRANCH, GIT_HASH);
	CLI_Printf("\r\n****************************************************");
	CLI_Printf("\r\n");
}

uint8_t sys_uptime(void)
{
    return TE_OK;
}

/** @brief Terminal initialize */
void CLI_Init(TypeDefaultCmd_e defCmd)
{
	//cli_welcome();
	INPUT_Init();

	Terminal.countCommand = 0;
	Terminal.executeState = 0;
	Terminal.isEntered = false;

	CLI_AddCmd("help", _help_cmd, 0, TMC_None, "help by terminal command");
	CLI_AddCmd("version", print_cli_w, 0, TMC_None, "Print CLI Welcome");
    CLI_AddCmd("sys_uptime", sys_uptime, 0, TMC_PrintStartTime, "System UpTime");
	CLI_AddCmd("reboot", reboot_mcu, 0, TMC_None, "reboot MCU");

	CLI_Printf("\r\n");

	Terminal.inputArgs.argv = (char **)cli_malloc(sizeof(char *) * CLI_ARGS_BUF_SIZE);
	for (uint8_t i = 0; i < CLI_ARGS_BUF_SIZE; i++)
		Terminal.inputArgs.argv[i] = cli_malloc(sizeof(char) * (CLI_ARG_SIZE + 1));

#if (CLI_CMD_LOG_EN == 1)
	CLI_LogInit();
#endif

	printArrow();
}

/**
 * @brief Get index coinciding args string or -1
 * @param flag - searched string
* */
int8_t IndexOfFlag(const char *flag)
{
	for (uint8_t i = 0; i < Terminal.inputArgs.argc; i++)
	{
		if (_strcmp(Terminal.inputArgs.argv[i], flag))
		{
			return i;
		}
	}

	return -1;
}

#define CLI_GetDecString(str) ((uint32_t)strtol((const char *)str, NULL, 10))
#define CLI_GetHexString(str) ((uint32_t)strtol((const char *)str, NULL, 16))
#define CLI_GetArgString(str) ((uint32_t)_strcmp((const char *)str, NULL, 16))

/** @brief Get argument in Dec */
inline uint32_t CLI_GetArgDec(uint8_t index)
{
	return CLI_GetDecString(Terminal.inputArgs.argv[index + 1]);
}

/** @brief Get argument in Hex */
inline uint32_t CLI_GetArgHex(uint8_t index)
{
	return CLI_GetHexString(Terminal.inputArgs.argv[index + 1]);
}

/** @brief Get argument in str */
int CLI_GetArgStr(uint8_t index, char* str)
{
    return _strcmp(Terminal.inputArgs.argv[index + 1], str);
}

bool CLI_GetArgDecByFlag(const char *flag, uint32_t *outValue)
{
	int8_t w = IndexOfFlag(flag);
	if ((w > 0) && (w + 1 < Terminal.inputArgs.argc))
	{
		*outValue = CLI_GetArgDec(w);
		return true;
	}

	return false;
}

bool CLI_GetArgHexByFlag(const char *flag, uint32_t *outValue)
{
	int8_t w = IndexOfFlag(flag);
	if ((w > 0) && (w + 1 < Terminal.inputArgs.argc))
	{
		*outValue = CLI_GetArgHex(w);
		return true;
	}

	return false;
}

bool CLI_IsArgFlag(const char *flag)
{
	return IndexOfFlag(flag) >= 0;
}

/**
 * @brief Execute command
 * @param argv argument strings
 * @param argc count argument strings
 * @return result execute command
* */
TE_Result_e Execute(char **argv, uint8_t argc)
{
	if (argc < 1)
		return TE_ArgErr;

	TermCmd_s *cmd = _findTermCmd(argv[0]);

	if (cmd != NULL)
	{

		if ((argc - 1) < cmd->argc)
			return TE_ArgErr;

		Terminal.executeState = 1;

		if (cmd->mode & TMC_PrintStartTime)
			CLI_PrintTime();

		uint32_t startMs = CLI_GetMs();
		TE_Result_e result = cmd->fcn(argv, argc);
		uint32_t stopMs = CLI_GetMs();

		if (cmd->mode & TMC_PrintStopTime)
			CLI_PrintTime();

		if (cmd->mode & TMC_PrintDiffTime)
		{
			CLI_Time_s t = CLI_GetFormatTimeByMs(stopMs - startMs);
			PrintTime(&t);
		}

		Terminal.executeState = 0;

		return result;
	}

	return TE_NotFound;
}

/**
 * @brief Execute command
 * @param str command const string include arguments
 * @return result execute command
* */
TE_Result_e ExecuteString(const char *str)
{
	split((char *)str, " ", (Params_s *)&Terminal.inputArgs);

#if 0
	CLI_DPrintf("\r\nCMD: ");
	CLI_DPrintf(str);
#endif

#if 0
	for(uint8_t i = 0; i < Terminal.input_args.argc;i++)
        CLI_DPrintf("\r\n: %s", Terminal.input_args.argv[i]);
#endif

	TE_Result_e result = Execute(Terminal.inputArgs.argv, Terminal.inputArgs.argc);

	ArgDestroy((Params_s *)&Terminal.inputArgs);

#if (CLI_PRINT_ERROR_EXEC_EN == 1)
	PrintResultExec(result);
#endif

	printArrow();

	return result;
}

/** @brief Execute CLI */
bool CLI_Service()
{
	if (Terminal.isEntered == true)
	{
		ExecuteString((const char *)INPUT_GetBuffer(TransitBuffer));
		Terminal.isEntered = false;

		return true;
	}

	return false;
}

/**
 * @brief Add command
 * @param name - input name
 * @param fcn - callback function
 * @param argc - min count arguments
 * @param mode - execute mode
 * @param descr - description
 * @return result append command
* */
TA_Result_e CLI_AddCmd(const char *name, uint8_t (*fcn)(), uint8_t argc, uint16_t mode, const char *descr)
{
	if (Terminal.countCommand >= CLI_SIZE_TASK)
	{
		PrintResultAddCmd(TA_MaxCmd);
		return TA_MaxCmd;
	}

	if (fcn == NULL)
	{
		PrintResultAddCmd(TA_FcnNull);
		return TA_FcnNull;
	}

	if (_strlen((char *)name) == 0)
	{
		PrintResultAddCmd(TA_EmptyName);
		return TA_EmptyName;
	}

	uint8_t i = 0;
	for (; i < Terminal.countCommand; i++)
		if (_strcmp((char *)Terminal.cmds[i].name, (char *)name))
		{
			PrintResultAddCmd(TA_RetryName);
			return TA_RetryName;
		}

	uint8_t countCmd = Terminal.countCommand;
	Terminal.cmds[countCmd].fcn = fcn;
	Terminal.cmds[countCmd].name = name;
	Terminal.cmds[countCmd].argc = argc;
	Terminal.cmds[countCmd].mode = mode;
	Terminal.cmds[countCmd].description = descr;
	Terminal.countCommand++;

	return TA_OK;
}

/**
 * @brief Print result execute command
 * @param code - result code
 * @return none
 * */
static void PrintResultExec(uint8_t code)
{
	switch (code)
	{
	case TE_NotFound:
		CLI_Printf("\n\rerr: Command not found");
		break;
	case TE_ArgErr:
		CLI_Printf("\n\rerr: Fault argument");
		break;
	case TE_ExecErr:
		CLI_Printf("\n\rerr: Execute functions");
		break;
	case TE_WorkInt:
		CLI_Printf("\n\rmsg: Command abort");
		break;
	default:
		break;
	}
}

/**
 * @brief Print result add command action
 * @param code - result code
 * @return none
 * */
static void PrintResultAddCmd(uint8_t code)
{
#if (CLI_PRINT_ERROR_ADD_CMD_EN == 1)
	switch (code)
	{
	case TA_MaxCmd:
		CLI_Printf("\n\radd cmd err: Memory is full");
		break;
	case TA_FcnNull:
		CLI_Printf("\n\radd cmd err: Function callback is NULL");
		break;
	case TA_EmptyName:
		CLI_Printf("\n\radd cmd err: Empty name command");
		break;
	case TA_RetryName:
		CLI_Printf("\n\radd cmd err: Retry name command");
		break;
	default:
		break;
	}
#endif
}

/**
 * @brief Search command by name
 * @return Command pointer or NULL
 * */
TermCmd_s *_findTermCmd(const char *cmdName)
{
	uint8_t i = 0;
	for (; i < Terminal.countCommand; i++)
	{
		char *name1 = (char *)Terminal.cmds[i].name;
		char *name2 = (char *)cmdName;

		int res = _strcmp(name1, name2);

		if (res)
			return &Terminal.cmds[i];
	}

	return NULL;
}

/**
 * @brief Search command by name
 * @return Command pointer or NULL
* */
TermCmd_s *_findPartTermCmd(const char *cmdName)
{
	TermCmd_s *result = NULL;

	uint8_t i = 0;
	for (; i < Terminal.countCommand; i++)
	{
		char *name1 = (char *)Terminal.cmds[i].name;
		char *name2 = (char *)cmdName;

		int res = _strPartCmp(name1, name2);

		if (res)
		{
			if (result != NULL)
				return NULL;

			result = &Terminal.cmds[i];
		}
	}

	return result;
}

// ************************************************************************

// *************************   sys cmd CLI    *****************************

uint8_t _help_cmd()
{
	CLI_Printf("\r\nCount command: %d", (int)Terminal.countCommand - 1); // -1 HELP
	CLI_Printf("\r\n----------------------------------------------------");

	for (uint16_t i = 1; i < Terminal.countCommand; i++)
	{
		CLI_Printf("\r\n%-10s - %s", Terminal.cmds[i].name, Terminal.cmds[i].description);
		CLI_Printf("\r\n----------------------------------------------------");
	}

	return TE_OK;
}

uint8_t reboot_mcu(void)
{
    CLI_Printf("\r\nreset MCU\r\n")
    HAL_Delay(1000);
    HAL_NVIC_SystemReset();
	return TE_OK;
}

uint8_t print_cli_w(void)
{
	cli_welcome();
	return TE_OK;
}

// ************************************************************************

void PrintTime(CLI_Time_s *t)
{
	CLI_Printf("\r\n%02dh:%02dm:%02ds.%03dms", (int)t->hour, (int)t->minute, (int)t->second, (int)t->msec);
}

/** 
 * @brief CLI PrintTime
 * @return none
* */
void CLI_PrintTime()
{
#if (CLI_TIMELEFT_EN == 1)
	uint32_t ms = CLI_GetMs();
	CLI_Time_s t = CLI_GetFormatLastTimeByMs(ms);
	PrintTime(&t);
#endif
}

/**
 * @brief 
 * @return none
* */
void CLI_PrintTimeWithoutRN()
{
#if (CLI_TIMELEFT_EN == 1)
	uint32_t ms = CLI_GetMs();
	CLI_Time_s t = CLI_GetFormatLastTimeByMs(ms);
	PrintTime(&t);
#endif
}

void CLI_Set_First_IN(bool set){

    Terminal.first_in = set;
}

/** @brief Append new symbols */
TC_Result_e CLI_AppendChar(char ch)
{
	static bool rstUnlock = false;
	if (rstUnlock)
		rstUnlock = false;

    InputValue_s iv = INPUT_PutChar(ch);
	char c = iv.keyCode;

#if 0
	CLI_DPrintf("\r\n");
	for(uint8_t i = 0; i < 4; i++)
	{
		CLI_DPrintf("0x%02X ", (char) *((char*)(Terminal.symbols.ptrObj + i)));
	}
    CLI_DPrintf("\r\nKey Code: 0x%02X", (uint8_t) ch);
#endif
	if (iv.isValid)
	{
		switch (c) {
            case TERM_KEY_ENTER: {
                if (Terminal.first_in == false) {
                    cli_welcome();
                    Terminal.first_in = true;
                }
                if (INPUT_IsEmpty()) {
                    printArrow();
                    return TC_Ignore;
                }
                Terminal.isEntered = true;

                INPUT_Cache();

#if (CLI_CMD_LOG_EN == 1)
                CLI_LogCmdPush(INPUT_GetBuffer(MainBuffer));
                CLI_CurReset();
#endif

                INPUT_Reset();

                return TC_Enter;
            }
                break;

            case TERM_KEY_ESCAPE:
                _interrupt_operation = true;
                CLI_Printf("\r\nESC\n");
                break;

            case CHAR_INTERRUPT :
                _interrupt_operation = true;
                CLI_Printf("\r\nINTERRUPT\n");
                break;

            case TERM_KEY_BACKSPACE:
                INPUT_Backspace();
                break;

            case TERM_KEY_TAB: {
#if (CLI_CMD_AUTOCMPLT_EN == 1)
                if ((!INPUT_IsEmpty()) && (INPUT_GetLastChar() != ' ')) {
                    char *buf = INPUT_GetBuffer(MainBuffer);
                    TermCmd_s *cmd = _findPartTermCmd(buf);

                    if (cmd != NULL) {
                        uint8_t len = _strlen(cmd->name);

                        INPUT_SetBuffer(MainBuffer, (char *) cmd->name, len + 1);
                        INPUT_Refresh(strcat(buf, " "));
                    }
                }
#endif
            }
                break;

            case TERM_KEY_DOWN: {
#if (CLI_CMD_LOG_EN == 1)
                const char *ptrCmd = CLI_GetNextCmd();
                if (ptrCmd != NULL)
                    INPUT_Refresh(ptrCmd);
#endif
            }
                break;

            case TERM_KEY_UP: {
#if (CLI_CMD_LOG_EN == 1)
                const char *ptrCmd = CLI_GetLastCmd();
                if (ptrCmd != NULL)
                    INPUT_Refresh(ptrCmd);
#endif
            }
                break;

#if (CLI_LR_KEY_EN == 1)
            case TERM_KEY_LEFT:
                INPUT_CursorToLeft();
                break;
#endif

#if (CLI_LR_KEY_EN == 1)
            case TERM_KEY_RIGHT:
                INPUT_CursorToRight();
                break;
#endif

            case TERM_KEY_DEL:
                INPUT_Delete();
                break;

            case TERM_KEY_HOME:
                INPUT_CursorToHome();
                break;

            case TERM_KEY_END:
                INPUT_CursorToEnd();
                break;

            default: {
                if (iv.isAlphaBet && !INPUT_IsFull())
                    INPUT_AddChar(c);
            }
                break;
        }
	}
	else
	{
		return TC_BufFull;
	}

	return TC_OK;
}

void TUSART_PutChar(char c)
{
    CDC_Transmit_SET_BUFF(c);
}

void TUSART_Print(char *str)
{
	uint16_t i = 0;
	while (str[i] != '\0')
	{
		TUSART_PutChar(str[i]);
		i++;
	}
}

void SysTick_CLI(void)
{
    SysTic++;
}