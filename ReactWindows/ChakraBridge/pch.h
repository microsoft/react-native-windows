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