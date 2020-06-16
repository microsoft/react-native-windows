// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "Logging.h"

#include <cxxreact/ReactMarker.h>
#include <chrono>
#include "../Chakra/ChakraPlatform.h"

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

void logMarker(const ReactMarker::ReactMarkerId /*id*/, const char * /*tag*/) {}
} // end anonymous namespace

void InitializeLogging(NativeLoggingHook &&hook) {
  g_nativeLogHook = std::move(hook);

  JSNativeHooks::loggingHook = LogHook;
  JSNativeHooks::nowHook = nativePerformanceNow;

  ReactMarker::logTaggedMarker = logMarker;
}

} // namespace react
} // namespace facebook
