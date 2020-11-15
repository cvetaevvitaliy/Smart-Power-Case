#include "cli_string.h"
#include "cli_config.h"

uint16_t _getCountSeparator(const char* str, const char* separator);

/// \brief Splitting a string by a given separator
void split(char* strSrc, const char* separator, Params_s* dst)
{
    uint8_t count_sep = _getCountSeparator(strSrc, separator);
    uint16_t lenSep = _strlen((char*)separator);

    dst->argc = count_sep;

    int start_index = 0;
    int size_word = 0;

    int co = 0;
    int co_arg = 0;
    while(1)
    {

        uint16_t s = 0;

        for(; s < lenSep; s++){
            if((strSrc[co] == separator[s]) || (strSrc[co] == '\0'))
            {
                size_word = co - start_index;

                dst->argv[co_arg][size_word] = '\0';
                _strcpy(strSrc, start_index, dst->argv[co_arg], 0, size_word);

                start_index = co + 1;
                co_arg++;
                break;
            }
        }

        if (strSrc[co] == '\0')
            break;

        co++;
    }
}


void ArgDestroy(Params_s* src)
{
    src->argc = 0;
}

// ****************** private methods **********************

void _strcpy(const char* src, uint16_t offsetSrc, char* dst, uint16_t offsetDst, uint16_t length)
{
    uint16_t i = 0;
    for(; i < length; i++)
        dst[i + offsetDst] = src[i + offsetSrc];
}

uint32_t _strlen(const char* strSrc)
{
    int32_t co = 0;
    while((strSrc != NULL) && (*strSrc != '\0')){
        strSrc++;
        co++;
    }

    return co;
}

uint16_t _getCountSeparator(const char* strSrc, const char* separator)
{
    uint8_t i = 0;
    uint16_t result = 0;
    uint16_t count_separator = _strlen((char*) separator);

    for(; strSrc[i] != '\0'; i++)
    {
        uint16_t j = 0;
        for(; j < count_separator; j++)
            if (strSrc[i] == separator[j]){
                result++;
                break;
            }
    }

    return result + 1;
}

uint8_t _strPartCmp(const char* str1, const char* str2)
{
	uint8_t co = 0;
	while(((str1 + co) != NULL) && (*(str1 + co) != '\0') &&
          ((str2 + co) != NULL) && (*(str2 + co) != '\0')){

        if (str1[co] != str2[co])
            return 0;

        co++;
    }
    
    return 1;
}

uint8_t _strcmp(const char* str1, const char* str2)
{
    uint16_t co = 0;

	if (_strlen(str1) != _strlen(str2))
		return 0;

    while(((str1 + co) != NULL) && (*(str1 + co) != '\0') &&
          ((str2 + co) != NULL) && (*(str2 + co) != '\0')){

        if (str1[co] != str2[co])
            return 0;

        co++;
    }

    if  (
            (((str1 + co) == NULL) && ((str2 + co) != NULL)) ||
            (((str2 + co) == NULL) && ((str1 + co) != NULL)) ||
            (((str1 + co) != NULL) && ((str2 + co) != NULL) && (*(str2 + co) != *(str1 + co)))
         )
        return 0;


    return 1;
}
