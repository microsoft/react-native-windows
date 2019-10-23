// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "Logging.h"

#include <chrono>
#include "../Chakra/ChakraPlatform.h"
#if !defined(OSS_RN)
#include <cxxreact/Platform.h>
#endif

namespace facebook {
namespace react {

namespace {

NativeLoggingHook g_nativeLogHook;

void LogHook(RCTLogLevel logLevel, const char *message) {
  g_nativeLogHook(logLevel, message);
}

static double nativePerformanceNow() {
  return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

#if !defined(OSS_RN)
void logMarker(const ReactMarker::ReactMarkerId /*id*/, const char * /*tag*/) {}
#endif
} // end anonymous namespace

void InitializeLogging(NativeLoggingHook &&hook) {
  g_nativeLogHook = std::move(hook);

  JSNativeHooks::loggingHook = LogHook;
  JSNativeHooks::nowHook = nativePerformanceNow;

#if !defined(OSS_RN)
  ReactMarker::logTaggedMarker = logMarker;
#endif
}

} // namespace react
} // namespace facebook
