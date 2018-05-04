// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <jsrt.h>

struct SerializedSourceContext
{
    HANDLE fileHandle;
    HANDLE mapHandle;
    BYTE* byteBuffer;
    wchar_t* scriptBuffer;

    void Dispose()
    {
        if (fileHandle != NULL)
        {
            UnmapViewOfFile(byteBuffer);
            CloseHandle(mapHandle);
            CloseHandle(fileHandle);
            fileHandle = NULL;
        }

        delete[] scriptBuffer;
        scriptBuffer = NULL;
    }

    ~SerializedSourceContext()
    {
        Dispose();
    }
};
