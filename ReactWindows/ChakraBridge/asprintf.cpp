// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "asprintf.h"

int _aswprintf(wchar_t **ret, const wchar_t *format, ...)
{
    va_list ap;

    *ret = NULL;  /* Ensure value can be passed to free() */

    va_start(ap, format);
    int count = _vscwprintf(format, ap);
    va_end(ap);

    if (count >= 0)
    {
        wchar_t* buffer = (wchar_t*)malloc((count + 1) * sizeof(wchar_t));
        if (buffer == NULL)
            return -1;

        va_start(ap, format);
        count = _vsnwprintf_s(buffer, count + 1, count + 1, format, ap);
        va_end(ap);

        if (count < 0)
        {
            free(buffer);
            return count;
        }
        *ret = buffer;
    }

    return count;
}

int _asprintf(char **ret, const char *format, ...)
{
    va_list ap;

    *ret = NULL;  /* Ensure value can be passed to free() */

    va_start(ap, format);
    int count = _vscprintf(format, ap);
    va_end(ap);

    if (count >= 0)
    {
        char* buffer = (char*)malloc(count + 1);
        if (buffer == NULL)
            return -1;

        va_start(ap, format);
        count = vsnprintf_s(buffer, count + 1, count + 1, format, ap);
        va_end(ap);

        if (count < 0)
        {
            free(buffer);
            return count;
        }
        *ret = buffer;
    }

    return count;
}
