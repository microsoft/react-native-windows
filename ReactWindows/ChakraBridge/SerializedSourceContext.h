#pragma once

#include <jsrt.h>

struct SerializedSourceContext
{
    BYTE* byteCode;
    const wchar_t* sourcePath;
    wchar_t* scriptBuffer;

    ~SerializedSourceContext()
    {
        if (byteCode)
        {
            delete[] byteCode;
        }

        if (scriptBuffer)
        {
            delete[] scriptBuffer;
        }
    }
};
