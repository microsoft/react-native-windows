#pragma once

#if USE_EDGEMODE_JSRT
#include <jsrt.h>
#else if USE_CHAKRACORE_JSRT 
#include <ChakraCore.h>
#endif

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
