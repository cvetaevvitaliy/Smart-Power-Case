#ifndef _CLI_STATIC_ARRAY_H_
#define _CLI_STATIC_ARRAY_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "cli_config.h"

/** Write to static memory what we defined before compilation */
char* init_static_memory(uint8_t n);

#endif