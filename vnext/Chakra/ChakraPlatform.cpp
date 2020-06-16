// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <cxxreact/ReactMarker.h>
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

JsValueRef __stdcall nowHookJNF(
    JsValueRef /*function*/,
    bool /*isConstructCall*/,
    JsValueRef * /*arguments*/,
    [[maybe_unused]] unsigned short argumentCount,
    void * /*callbackState*/) {
  assert(argumentCount == 1);
  double now = nowHook != nullptr ? nowHook() : 0;
  JsValueRef value;
  CHAKRA_ASSERTDO(JsDoubleToNumber(now, &value));
  return value;
}

} // namespace JSNativeHooks

#if __clang__
#pragma clang diagnostic pop
#endif

} // namespace react
} // namespace facebook
