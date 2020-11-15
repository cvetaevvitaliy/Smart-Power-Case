#include "static_memory.h"

#define MEMORY_LEN      CLI_CMD_BUF_SIZE // Since how much memory you need to determine before compilation (determine how much you need here)
#define MEMORY_CNT      CLI_SIZE_TASK    // How many memory array (blocks) will you need

static uint8_t cnt = 0;
static char arr[MEMORY_CNT][MEMORY_LEN];

/** Write to static memory what we defined before compilation */
char* init_static_memory(uint8_t n) {

    if (n > MEMORY_LEN) {
        LOG_ERROR("Error init static memory");
        return 0;
    }

    cnt ++;
    if (cnt > MEMORY_CNT){
        LOG_ERROR("Error init static memory");
        return 0;
    }

    return arr[cnt - 1];
}
