// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "Logging.h"

#include <cxxreact/ReactMarker.h>
#include <chrono>

namespace facebook {
namespace react {

namespace {

NativeLoggingHook g_nativeLogHook;

void LogHook(RCTLogLevel logLevel, const char *message) {
  g_nativeLogHook(logLevel, message);
}

void logMarker(const ReactMarker::ReactMarkerId /*id*/, const char * /*tag*/) {}
} // end anonymous namespace

void InitializeLogging(NativeLoggingHook &&hook) {
  g_nativeLogHook = std::move(hook);

  ReactMarker::logTaggedMarkerImpl = logMarker;
}

} // namespace react
} // namespace facebook
