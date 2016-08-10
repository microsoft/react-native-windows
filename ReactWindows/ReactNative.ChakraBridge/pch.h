#pragma once

#define USE_EDGEMODE_JSRT
#include <jsrt.h>
#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#define IfFailRet(v) \
    { \
        JsErrorCode error = (v); \
        if (error != JsNoError) \
        { \
            return error; \
        } \
    }
