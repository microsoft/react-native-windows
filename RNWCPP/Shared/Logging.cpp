// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "Logging.h"

#include "../Chakra/ChakraPlatform.h"
#include <chrono>
#include <cxxreact/Platform.h>

#if !defined(NOJSC)
#include <jschelpers/Value.h>
#endif

namespace facebook { namespace react {

namespace {

NativeLoggingHook g_nativeLogHook;

void LogHook(RCTLogLevel logLevel, const char* message) {
  g_nativeLogHook(logLevel, message);
}

static double nativePerformanceNow() {
  return std::chrono::duration<double, std::milli>(
    std::chrono::steady_clock::now().time_since_epoch()).count();
}

void logMarker(const ReactMarker::ReactMarkerId /*id*/, const char* /*tag*/) {
}

#if !defined(NOJSC)

JSValueRef nativePerformanceNowJSC(
    JSContextRef ctx,
    JSObjectRef function,
    JSObjectRef thisObject,
    size_t argumentCount,
    const JSValueRef arguments[], JSValueRef *exception) {
  // TODO
  return Value::makeNumber(ctx, 0);
}

JSValueRef nativeLoggingHookJSC(
    JSContextRef ctx,
    JSObjectRef function,
    JSObjectRef thisObject,
    size_t argumentCount,
    const JSValueRef arguments[], JSValueRef *exception) {
  // TODO
  return Value::makeUndefined(ctx);
}

#endif // !defined(NOJSC)

} // end anonymous namespace

void InitializeLogging(NativeLoggingHook&& hook) {
  g_nativeLogHook = std::move(hook);

  JSNativeHooks::loggingHook = LogHook;
  JSNativeHooks::nowHook = nativePerformanceNow;

#if !defined(NOJSC)
  JSCNativeHooks::loggingHook = nativeLoggingHookJSC;
  JSCNativeHooks::nowHook = nativePerformanceNowJSC;
  // JSCNativeHooks::installPerfHooks = addNativePerfLoggingHooksJSC;
#endif

  ReactMarker::logTaggedMarker = logMarker;
}

}}
