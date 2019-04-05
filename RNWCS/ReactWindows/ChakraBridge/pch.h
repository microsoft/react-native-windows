// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <collection.h>
#include <ppltasks.h>
#include <jsrt.h>
#include <stdio.h>
#include <tchar.h>
#include "ChakraStringResult.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

extern "C" std::function<void(int, const wchar_t *)> g_loggingCallback;
extern "C" bool g_isLoggingEnabled;

#ifdef _DEBUG
inline wchar_t* LogLevelToString(int logLevel)
{
    switch (logLevel)
    {
    case 0:
        return L"Trace";
    case 1:
        return L"Info";
    case 2:
        return L"Warn";
    case 3:
        return L"Error";
    default:
        return L"Log";
    }
}
#endif

#define IfFailRetNullPtr(v) \
    { \
        JsErrorCode status = (v); \
        if (status != JsNoError) \
        { \
            ChakraStringResult stringResult; \
            stringResult.ErrorCode = status; \
            return stringResult; \
        } \
    }

#define IfFailRet(v) \
    { \
        JsErrorCode error = (v); \
        if (error != JsNoError) \
        { \
            return error; \
        } \
    }

#define IfFailThrow(v, e) \
    { \
        JsErrorCode error = (v); \
        if (error != JsNoError) \
        { \
            ThrowException((e)); \
            return JS_INVALID_REFERENCE; \
        } \
    }

#define IfFailCleanup(v) \
    { \
        status = (v); \
        if (status != JsNoError) \
        { \
            goto cleanup; \
        } \
    }

#define IfErrnoCleanup(v) \
    { \
        if (v) \
		{ \
            goto cleanup; \
		} \
    }
