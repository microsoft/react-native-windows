#pragma once

#include <jsrt.h>

struct SerializedSourceContext
{
    HANDLE fileHandle;
    HANDLE mapHandle;
    BYTE* byteBuffer;
    wchar_t* scriptBuffer;

    ~SerializedSourceContext()
    {
        if (fileHandle != NULL)
        {
            UnmapViewOfFile(byteBuffer);
            CloseHandle(mapHandle);
            CloseHandle(fileHandle);
        }
        else
        {
            delete[] byteBuffer;
        }
        delete[] scriptBuffer;
    }
};
