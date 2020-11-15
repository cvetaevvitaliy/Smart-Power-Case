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
#include "usbd_cdc_if.h"

#define printArrow()            {CLI_Printf("%s%s",STRING_TERM_ENTER, STRING_TERM_ARROW);}    // Output of input line
#define printArrowWithoutN()    {CLI_Printf(STRING_TERM_ARROW);}

static void PrintResultExec(uint8_t code);

static void PrintResultAddCmd(uint8_t code);

static void PrintTime(CLI_Time_t *time);

static int8_t IndexOfFlag(const char *flag);

static void CLI_PrintTime();

void CLI_PrintTimeWithoutRN();

char dbgbuffer[128];
volatile uint64_t SysTic;                                        // Variable tackts cntr
volatile uint32_t *UniqueID = (uint32_t *) 0x1FFFF7E8;


static uint8_t _help_cmd();

static uint8_t reboot_mcu();

static uint8_t print_cli_w(void);


// ************************* interrupt function ***************************

static bool _interrupt_operation = false;

/** @brief Checking the status of the start of the operation (return, stator-on) */
inline bool CLI_GetIntState() {

    bool res = _interrupt_operation;
    _interrupt_operation = false;
    return res;
}

// ************************************************************************

// *********************** Terminal fcns **********************************

/** @brief Command settings */
typedef struct {
    uint8_t (*fcn)();           // callback function command
    const char *name;           // name command
    uint8_t argc;               // min count argument
    uint16_t mode;              // mode execute command
    const char *description;    // description command
} TermCmd_s;

/** @brief Terminal State */
struct {
    TermCmd_s cmds[CLI_SIZE_TASK];  // list commands
    uint8_t countCommand;           // count commands
    uint8_t executeState;           // state terminal
    volatile Params_s inputArgs;    // args current execute command
    bool isEntered;
    bool first_in;
} Terminal;

TermCmd_s *_findTermCmd(const char *cmdName);

static void cli_welcome(void) {

    CLI_Printf("\r\n****************************************************");
    CLI_Printf("\r\n| Smart Power Case                                 |");
    CLI_Printf("\r\n| SW  ver: v%s.%s.%s   MCU: %s \t\t   |", MINOR, MAJOR, PATCH, MCU);
    CLI_Printf("\r\n| CLI ver: %s   UID: %X-%X-%X |", _TERM_VER_, UniqueID[0], UniqueID[1], UniqueID[2]);
    CLI_Printf("\r\n| Build Date: %s %s \t\t   |", __DATE__, __TIME__);
    CLI_Printf("\r\n| Branch: %s GIT-HASH: %s\t\t   |", GIT_BRANCH, GIT_HASH);
    CLI_Printf("\r\n****************************************************");
    CLI_Printf("\r\n");
}

uint8_t sys_uptime(void) {
    return CLI_OK;
}

/** @brief Terminal initialize */
void CLI_Init(CLI_TypeDefaultCmd_t defCmd) {

    CLI_INPUT_Init();

    Terminal.countCommand = 0;
    Terminal.executeState = 0;
    Terminal.isEntered = false;

    CLI_AddCmd("help", _help_cmd, 0, CLI_PrintNone, "help by terminal command");
    CLI_AddCmd("version", print_cli_w, 0, CLI_PrintNone, "Print CLI Welcome");
    CLI_AddCmd("sys_uptime", sys_uptime, 0, CLI_PrintStartTime, "System UpTime");
    CLI_AddCmd("reboot", reboot_mcu, 0, CLI_PrintNone, "reboot MCU");

    CLI_Printf("\r\n");

    Terminal.inputArgs.argv = (char **) cli_malloc(sizeof(char *) * CLI_ARGS_BUF_SIZE);
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
int8_t IndexOfFlag(const char *flag) {

    for (uint8_t i = 0; i < Terminal.inputArgs.argc; i++) {
        if ( _strcmp(Terminal.inputArgs.argv[i], flag)) {
            return i;
        }
    }

    return -1;
}

#define CLI_GetDecString(str) ((uint32_t)strtol((const char *)str, NULL, 10))
#define CLI_GetHexString(str) ((uint32_t)strtol((const char *)str, NULL, 16))
#define CLI_GetArgString(str) ((uint32_t)_strcmp((const char *)str, NULL, 16))

/** @brief Get argument in Dec */
inline uint32_t CLI_GetArgDec(uint8_t index) {
    return CLI_GetDecString(Terminal.inputArgs.argv[index + 1]);
}

/** @brief Get argument in Hex */
inline uint32_t CLI_GetArgHex(uint8_t index) {
    return CLI_GetHexString(Terminal.inputArgs.argv[index + 1]);
}

/** @brief Get argument in str */
int CLI_GetArgStr(uint8_t index, char *str) {
    return _strcmp(Terminal.inputArgs.argv[index + 1], str);
}

bool CLI_GetArgDecByFlag(const char *flag, uint32_t *outValue) {

    int8_t w = IndexOfFlag(flag);

    if ((w > 0) && (w + 1 < Terminal.inputArgs.argc)) {
        *outValue = CLI_GetArgDec(w);
        return true;
    }

    return false;
}

bool CLI_GetArgHexByFlag(const char *flag, uint32_t *outValue) {

    int8_t w = IndexOfFlag(flag);

    if ((w > 0) && (w + 1 < Terminal.inputArgs.argc)) {
        *outValue = CLI_GetArgHex(w);
        return true;
    }

    return false;
}

bool CLI_IsArgFlag(const char *flag) {
    return IndexOfFlag(flag) >= 0;
}

/**
 * @brief Execute command
 * @param argv argument strings
 * @param argc count argument strings
 * @return result execute command
* */
CLI_Result_t Execute(char **argv, uint8_t argc) {

    if ( argc < 1 )
        return CLI_ArgErr;

    TermCmd_s *cmd = _findTermCmd(argv[0]);

    if ( cmd != NULL) {

        if ((argc - 1) < cmd->argc )
            return CLI_ArgErr;

        Terminal.executeState = 1;

        if ( cmd->mode & CLI_PrintStartTime )
            CLI_PrintTime();

        uint32_t startMs = CLI_GetMs();
        CLI_Result_t result = cmd->fcn(argv, argc);
        uint32_t stopMs = CLI_GetMs();

        if ( cmd->mode & CLI_PrintStopTime )
            CLI_PrintTime();

        if ( cmd->mode & CLI_PrintDiffTime ) {
            CLI_Time_t t = CLI_GetFormatTimeByMs(stopMs - startMs);
            PrintTime(&t);
        }

        Terminal.executeState = 0;

        return result;
    }

    return CLI_NotFound;
}

/**
 * @brief Execute command
 * @param str command const string include arguments
 * @return result execute command
* */
CLI_Result_t ExecuteString(const char *str) {

    split((char *) str, " ", (Params_s *) &Terminal.inputArgs);

#if 0
    LOG_DEBUG("\r\nCMD: ");
    LOG_DEBUG("%s",str);
#endif

#if 0
    for(uint8_t i = 0; i < Terminal.inputArgs.argc; i++)
        LOG_DEBUG("\r\n: %s", Terminal.inputArgs.argv[i]);
#endif

    CLI_Result_t result = Execute(Terminal.inputArgs.argv, Terminal.inputArgs.argc);

    ArgDestroy((Params_s *) &Terminal.inputArgs);

#if (CLI_PRINT_ERROR_EXEC_EN == 1)
    PrintResultExec(result);
#endif

    printArrow();

    return result;
}

/** @brief Execute CLI */
bool CLI_Service() {

    if ( Terminal.isEntered == true ) {
        ExecuteString((const char *) CLI_INPUT_GetBuffer(TransitBuffer));
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
CLI_ADD_CMD_Result_t CLI_AddCmd(const char *name, uint8_t (*fcn)(), uint8_t argc, uint16_t mode, const char *descr) {

    if ( Terminal.countCommand >= CLI_SIZE_TASK) {
        PrintResultAddCmd(ADD_CMD_MaxCmd);
        return ADD_CMD_MaxCmd;
    }

    if ( fcn == NULL) {
        PrintResultAddCmd(ADD_CMD_FcnNull);
        return ADD_CMD_FcnNull;
    }

    if ( _strlen((char *) name) == 0 ) {
        PrintResultAddCmd(ADD_CMD_EmptyName);
        return ADD_CMD_EmptyName;
    }

    uint8_t i = 0;
    for (; i < Terminal.countCommand; i++)
        if ( _strcmp((char *) Terminal.cmds[i].name, (char *) name)) {
            PrintResultAddCmd(ADD_CMD_RetryName);
            return ADD_CMD_RetryName;
        }

    uint8_t countCmd = Terminal.countCommand;
    Terminal.cmds[countCmd].fcn = fcn;
    Terminal.cmds[countCmd].name = name;
    Terminal.cmds[countCmd].argc = argc;
    Terminal.cmds[countCmd].mode = mode;
    Terminal.cmds[countCmd].description = descr;
    Terminal.countCommand++;

    return ADD_CMD_OK;
}

/**
 * @brief Print result execute command
 * @param code - result code
 * @return none
 * */
static void PrintResultExec(uint8_t code) {

    switch (code) {
        case CLI_NotFound:
            CLI_Printf ("\n\rerr: Command not found");
            break;
        case CLI_ArgErr:
            CLI_Printf ("\n\rerr: Fault argument");
            break;
        case CLI_ExecErr:
            CLI_Printf ("\n\rerr: Execute functions");
            break;
        case CLI_WorkInt:
            CLI_Printf ("\n\rmsg: Command abort");
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
static void PrintResultAddCmd(uint8_t code) {
#if (CLI_PRINT_ERROR_ADD_CMD_EN == 1)
    switch (code) {
        case ADD_CMD_MaxCmd:
            CLI_Printf ("\n\radd cmd err: Memory is full");
            break;
        case ADD_CMD_FcnNull:
            CLI_Printf ("\n\radd cmd err: Function callback is NULL");
            break;
        case ADD_CMD_EmptyName:
            CLI_Printf ("\n\radd cmd err: Empty name command");
            break;
        case ADD_CMD_RetryName:
            CLI_Printf ("\n\radd cmd err: Retry name command");
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
TermCmd_s *_findTermCmd(const char *cmdName) {
    uint8_t i = 0;
    for (; i < Terminal.countCommand; i++) {
        char *name1 = (char *) Terminal.cmds[i].name;
        char *name2 = (char *) cmdName;

        int res = _strcmp(name1, name2);

        if ( res )
            return &Terminal.cmds[i];
    }

    return NULL;
}

/**
 * @brief Search command by name
 * @return Command pointer or NULL
* */
TermCmd_s *_findPartTermCmd(const char *cmdName) {
    TermCmd_s *result = NULL;

    uint8_t i = 0;
    for (; i < Terminal.countCommand; i++) {
        char *name1 = (char *) Terminal.cmds[i].name;
        char *name2 = (char *) cmdName;

        int res = _strPartCmp(name1, name2);

        if ( res ) {
            if ( result != NULL)
                return NULL;

            result = &Terminal.cmds[i];
        }
    }

    return result;
}

// ************************************************************************

// *************************   sys cmd CLI    *****************************

uint8_t _help_cmd() {
    CLI_Printf("\r\nCount command: %d", (int) Terminal.countCommand - 1); // -1 HELP
    CLI_Printf("\r\n----------------------------------------------------");

    for (uint16_t i = 1; i < Terminal.countCommand; i++) {
        CLI_Printf("\r\n%-10s - %s", Terminal.cmds[i].name, Terminal.cmds[i].description);
        CLI_Printf("\r\n----------------------------------------------------");
    }

    return CLI_OK;
}

uint8_t reboot_mcu(void) {
    CLI_Printf("\r\nreset MCU\r\n")
    HAL_Delay(1000);
    HAL_NVIC_SystemReset();
    return CLI_OK;
}

uint8_t print_cli_w(void) {
    cli_welcome();
    return CLI_OK;
}

// ************************************************************************

void PrintTime(CLI_Time_t *t) {
    CLI_Printf("\r\n%02dh:%02dm:%02ds.%03dms", (int) t->hour, (int) t->minute, (int) t->second, (int) t->msec);
}

/**
 * @brief CLI PrintTime
 * @return none
* */
void CLI_PrintTime() {
#if (CLI_TIMELEFT_EN == 1)
    uint32_t ms = CLI_GetMs();
    CLI_Time_t t = CLI_GetFormatLastTimeByMs(ms);
    PrintTime(&t);
#endif
}

/**
 * @brief
 * @return none
* */
void CLI_PrintTimeWithoutRN() {
#if (CLI_TIMELEFT_EN == 1)
    uint32_t ms = CLI_GetMs();
    CLI_Time_t t = CLI_GetFormatLastTimeByMs(ms);
    PrintTime(&t);
#endif
}

void CLI_Set_First_IN(bool set) {

    Terminal.first_in = set;
}

/** @brief Append new symbols */
CLI_Append_Result_t CLI_AppendChar(char ch) {
    static bool rstUnlock = false;
    if ( rstUnlock )
        rstUnlock = false;

    CLI_InputValue_t iv = CLI_INPUT_PutChar(ch);
    char c = iv.keyCode;

#if 0
    CLI_DPrintf("\r\n");
    for(uint8_t i = 0; i < 4; i++)
    {
        CLI_DPrintf("0x%02X ", (char) *((char*)(Terminal.symbols.ptrObj + i)));
    }
    CLI_DPrintf("\r\nKey Code: 0x%02X", (uint8_t) ch);
#endif
    if ( iv.isValid ) {
        switch (c) {
            case TERM_KEY_ENTER: {
                if ( Terminal.first_in == false ) {
                    cli_welcome();
                    Terminal.first_in = true;
                }
                if ( CLI_INPUT_IsEmpty()) {
                    printArrow();
                    return CLI_APPEND_Ignore;
                }
                Terminal.isEntered = true;

                CLI_INPUT_Cache();

#if (CLI_CMD_LOG_EN == 1)
                CLI_LogCmdPush(CLI_INPUT_GetBuffer(MainBuffer));
                CLI_CurReset();
#endif

                CLI_INPUT_Reset();

                return CLI_APPEND_Enter;
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
                CLI_INPUT_Backspace();
                break;

            case TERM_KEY_TAB: {
#if (CLI_CMD_AUTOCMPLT_EN == 1)
                if ((!CLI_INPUT_IsEmpty()) && (CLI_INPUT_GetLastChar() != ' ')) {
                    char *buf = CLI_INPUT_GetBuffer(MainBuffer);
                    TermCmd_s *cmd = _findPartTermCmd(buf);

                    if ( cmd != NULL) {
                        uint8_t len = _strlen(cmd->name);

                        CLI_INPUT_SetBuffer(MainBuffer, (char *) cmd->name, len + 1);
                        CLI_INPUT_Refresh(strcat(buf, " "));
                    }
                }
#endif
            }
                break;

            case TERM_KEY_DOWN: {
#if (CLI_CMD_LOG_EN == 1)
                const char *ptrCmd = CLI_GetNextCmd();
                if ( ptrCmd != NULL)
                    CLI_INPUT_Refresh(ptrCmd);
#endif
            }
                break;

            case TERM_KEY_UP: {
#if (CLI_CMD_LOG_EN == 1)
                const char *ptrCmd = CLI_GetLastCmd();
                if ( ptrCmd != NULL)
                    CLI_INPUT_Refresh(ptrCmd);
#endif
            }
                break;

#if (CLI_LR_KEY_EN == 1)
            case TERM_KEY_LEFT:
                CLI_INPUT_CursorToLeft();
                break;
#endif

#if (CLI_LR_KEY_EN == 1)
            case TERM_KEY_RIGHT:
                CLI_INPUT_CursorToRight();
                break;
#endif

            case TERM_KEY_DEL:
                CLI_INPUT_Delete();
                break;

            case TERM_KEY_HOME:
                CLI_INPUT_CursorToHome();
                break;

            case TERM_KEY_END:
                CLI_INPUT_CursorToEnd();
                break;

            default: {
                if ( iv.isAlphaBet && !CLI_INPUT_IsFull())
                    CLI_INPUT_AddChar(c);
            }
                break;
        }
    } else {
        return CLI_APPEND_BufFull;
    }

    return CLI_APPEND_OK;
}

void CLI_PrintChar(char c) {
    CDC_Transmit_SET_BUFF(c);
}

void CLI_PrintStr(char *str) {
    uint16_t i = 0;
    while (str[i] != '\0') {
        CLI_PrintChar(str[i]);
        i++;
    }
}

void SysTick_CLI(void) {
    SysTic++;
}