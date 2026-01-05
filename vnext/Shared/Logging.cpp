// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "Logging.h"

// LEGACY REMOVED: ReactMarker.h - ReactMarker is no longer used in New Architecture
#include <chrono>

namespace facebook {
namespace react {

namespace {

NativeLoggingHook g_nativeLogHook;

void LogHook(RCTLogLevel logLevel, const char *message) {
  g_nativeLogHook(logLevel, message);
}

void logMarker(int /*id*/, const char * /*tag*/) {}
} // end anonymous namespace

void InitializeLogging(NativeLoggingHook &&hook) {
  g_nativeLogHook = std::move(hook);
}

} // namespace react
} // namespace facebook
