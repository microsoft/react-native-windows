// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#if !defined(OSS_RN)
#include <cxxreact/Platform.h>
#endif
#include "ChakraPlatform.h"

namespace facebook {
namespace react {

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
#endif

namespace JSNativeHooks {

LoggingHook loggingHook = nullptr;
NowHook nowHook = nullptr;

JsValueRef __stdcall nowHookJNF(JsValueRef function, bool isConstructCall, JsValueRef arguments[], unsigned short argumentCount, void* callbackState)
{
  assert(argumentCount == 1);
  double now = nowHook != nullptr ? nowHook() : 0;
  JsValueRef value;
  CHAKRA_ASSERTDO(JsDoubleToNumber(now, &value));
  return value;
}

}

#if __clang__
#pragma clang diagnostic pop
#endif

} }
