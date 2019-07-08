// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <cxxreact/JSExecutor.h>
#include <cxxreact/MessageQueueThread.h>

#if !defined(OSS_RN)
#include <cxxreact/Platform.h>
#endif

#include "Logging.h"

#if defined(USE_EDGEMODE_JSRT)
#include <jsrt.h>
#else
#include <ChakraCore.h>
#endif

#define CHAKRA_FAILED(jsErrorCode) ((jsErrorCode) != ::JsErrorCode::JsNoError)

#ifdef NDEBUG
#define CHAKRA_ASSERTDO(jsErrorCodeExpr) (jsErrorCodeExpr)
#else
#define CHAKRA_ASSERTDO(jsErrorCodeExpr) \
  do {                                   \
    JsErrorCode ec = (jsErrorCodeExpr);  \
    assert(!CHAKRA_FAILED(ec));          \
  } while (false)
#endif

namespace facebook {
namespace react {

namespace JSNativeHooks {

using LoggingHook = void (*)(RCTLogLevel level, const char *);
extern LoggingHook loggingHook;

using NowHook = double (*)();
extern NowHook nowHook;

JsValueRef __stdcall nowHookJNF(
    JsValueRef function,
    bool isConstructCall,
    JsValueRef arguments[],
    unsigned short argumentCount,
    void *callbackState);

} // namespace JSNativeHooks

} // namespace react
} // namespace facebook
