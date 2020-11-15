/*************************************************************************
 *
 *			CLI configure file
 *
 *	---------------------------------------------------------------------
 *
 *
 ************************************************************************/

#ifndef _CLI_CONFIG_H_
#define _CLI_CONFIG_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "tinyprintf.h"


// ****************************** Code keys ********************************

#define _KEY_INIT(c)                       ((char)c)

#define TERM_KEY_ENTER                      (_KEY_INIT(0x0D))        // Enter command symbol
#define TERM_KEY_BACKSPACE                  (_KEY_INIT(0x08))        // Delete character before cursor position
#define TERM_KEY_LSHIFT                     (_KEY_INIT(0x08))        // Left shift symbol
#define TERM_KEY_ESCAPE                     (_KEY_INIT(0xF0))        // Exception execute command symbol
#define TERM_KEY_UP                         (_KEY_INIT(0xF1))        // KeyUp symbol
#define TERM_KEY_RIGHT                      (_KEY_INIT(0xF2))        // KeyRight symbol
#define TERM_KEY_DOWN                       (_KEY_INIT(0xF3))        // KeyDown symbol
#define TERM_KEY_LEFT                       (_KEY_INIT(0xF4))        // KeyLeft symbol
#define TERM_KEY_DEL                        (_KEY_INIT(0xF5))        // Delete character after cursor position
#define TERM_KEY_HOME                       (_KEY_INIT(0xA0))        // Home key
#define TERM_KEY_END                        (_KEY_INIT(0xA1))        // End key
#define TERM_KEY_TAB                        (_KEY_INIT(0x09))        // TAB key
#define TERM_KEY_CONF                       (_KEY_INIT(0x60))        // Configurator

// **************************************************************************

// **********************    CLI Settings   *********************************

#ifndef GIT_BRANCH
#define GIT_BRANCH               ("__________")   //If no version has been specified -DGIT_BRANCH will be empty
#endif
#ifndef GIT_HASH
#define GIT_HASH                 ("________")     //If no version has been specified -GIT_HASH will be empty
#endif
#ifndef MINOR
#define MINOR                    ("_")            //If no version has been specified -GMINOR will be empty
#endif
#ifndef MAJOR
#define MAJOR                    ("_")            //If no version has been specified -GMAJOR will be empty
#endif
#ifndef PATCH
#define PATCH                    ("_")            //If no version has been specified -GPATCH will be empty
#endif
#ifndef MCU
#define MCU                    ("_________")      //If no version has been specified -GMCU will be empty
#endif

#define _TERM_VER_                      ("v0.0.2")          // Terminal version
#define CLI_SIZE_TASK                   (20)                // Max number of commands
#define CLI_CMD_BUF_SIZE                (40)                // Max number of character buffer string command
#define CLI_CMD_LOG_SIZE                (10)                // Max number of loging command
#define CLI_ARGS_BUF_SIZE               (10)                // Max number of arguments in one command
#define CLI_ARG_SIZE                    (5)                 // Max number character of one arguments
#define CHAR_INTERRUPT                  (0x03)              // Abort execute command key-code symbol
#define STRING_TERM_ENTER               ("\n\r")            // String new line
#define STRING_TERM_ARROW               (">> ")             // String arrow enter

#define CLI_TIMELEFT_EN                     (1)            // Calculate time (If you need performance, don't turn it on )
#define CLI_CMD_LOG_EN                      (1)            // Command logging (history)
#define CLI_CMD_AUTOCMPLT_EN                (1)            // Command AutoComplete
#define CLI_LR_KEY_EN                       (1)            // Move cursor left-rigth
#define CLI_DEFAULT_ALLOC_EN                (0)            // Default Memory Allocate functions (use static or malloc memmory for add new command)
#define CLI_DEFAULT_STRING_EN               (1)            // Default String functions
#define CLI_PRINT_ERROR_EXEC_EN             (1)            // Print error after execute command
#define CLI_PRINT_ERROR_ADD_CMD_EN          (1)            // Print error after added command
#define ECHO_EN                             (1)            // Enter echo enable

// **************************************************************************

// *************************   IO CLI Settings    ***************************
void CLI_PrintStr(char* str);
void CLI_PrintChar(char c);

extern char dbgbuffer[128];
#define CLI_Printf(...)         {sprintf(dbgbuffer,__VA_ARGS__);CLI_PrintStr(dbgbuffer);}
#if (ECHO_EN == 1)
#define CLI_PutChar             CLI_PrintChar
#else	// ECHO_EN != 1 ECHO off
#define CLI_PutChar
#endif  // ECHO_EN == 1

// **************************************************************************

// ***********************   IO Debug CLI Settings    ***********************

/**< This macro for debug software */
#if (DEBUG == 1)
#define LOG_DEBUG(f_, ...)          CLI_Printf(("\n[DEBUG] "f_), ##__VA_ARGS__)
#define LOG_INFO(f_, ...)           CLI_Printf(("\n[INFO] "f_), ##__VA_ARGS__)
#define LOG_ERROR(f_, ...)          CLI_Printf(("\n[ERROR] "f_), ##__VA_ARGS__)
#else
#define LOG_DEBUG(f_, ...)
#define LOG_INFO(f_, ...)
#define LOG_ERROR(f_, ...)
#endif

// **************************************************************************

// ************************ Time calculate Settings *************************

#if (CLI_TIMELEFT_EN == 1)

// yout implementation
extern volatile uint64_t SysTic;
#define CLI_GetUs()                     ((float)SysTic * 1000)    // System time in us
#define CLI_GetFastUs()                 (SysTic << 3)            // System time in us (not exact)
#define CLI_GetFastMs()                 (SysTic >> 7)            // System time in ms (not exact)
#define CLI_CounterReset()              {SysTic = 0;}

#else	// CLI_TIMELEFT_EN != 1

#define CLI_GetUs()					(0)			// System time in us
#define CLI_GetFastUs()				(0)			// System time in us (not exact)
#define CLI_GetFastMs()				(0)			// System time in ms (not exact)
#define CLI_CounterReset()			{}

#endif    // CLI_TIMELEFT_EN == 1

// **************************************************************************

// ********************** memory allocate functions *************************

#if (CLI_DEFAULT_ALLOC_EN == 1)
#include <malloc.h>
#define cli_malloc      malloc                  // dynamic memory
#define cli_free        free                    // dynamic memory
#else
#include "static_memory.h"
#define cli_malloc	    init_static_memory      // static memory
#define cli_free		free_static             // static memory
#endif

// **************************************************************************

// *************************** string functions *****************************

#if (CLI_DEFAULT_STRING_EN == 1)

#include <string.h>

#define cli_memcpy        memcpy
#else
#define cli_memcpy		// your implementation
#endif

// **************************************************************************

#endif
