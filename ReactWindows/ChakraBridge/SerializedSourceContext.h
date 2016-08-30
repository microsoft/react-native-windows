#pragma once

#include <jsrt.h>

struct SerializedSourceContext
{
    BYTE* byteBuffer;
    wchar_t* scriptBuffer;

    ~SerializedSourceContext()
    {
        if (byteBuffer != nullptr)
        {
            delete[] byteBuffer;
        }
        if (scriptBuffer != nullptr)
        {
            delete[] scriptBuffer;
        }
    }
};
