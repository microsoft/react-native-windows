#pragma once

#if USE_EDGEMODE_JSRT
#include <jsrt.h>
#else
#include <ChakraCore.h>
#endif

#include <stdio.h>
#include <tchar.h>
#include "ChakraStringResult.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#if _WINRT_DLL
#define managedNew(type) ref new \
    type
#define declareManagedPointerReferenceFor(t, value) auto managedPointerReferenceFor_##value = &value
#else _WINRT_DLL
#include <vcclr.h>

#define managedNew(type) gcnew \
    type
#define declareManagedPointerReferenceFor(t, value)  pin_ptr<t> managedPointerReferenceFor_##value = &value
#endif _WINRT_DLL

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