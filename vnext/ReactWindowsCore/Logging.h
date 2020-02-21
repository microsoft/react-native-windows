// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>

namespace facebook {

namespace react {

enum class RCTLogLevel {
  Trace = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4,
};

using NativeLoggingHook = std::function<void(RCTLogLevel logLevel, const char *message)>;
void InitializeLogging(NativeLoggingHook &&hook);

} // namespace react
} // namespace facebook
