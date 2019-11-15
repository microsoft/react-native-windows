// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef CHAKRACORE

#include "ChakraRuntimeArgs.h"

#include <functional>

namespace facebook::react {
class MessageQueueThread;
}

namespace Microsoft::JSI {

enum class LogLevel {
  Trace = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4,
};

using Logger = std::function<void(const char *message, LogLevel logLevel)>;

struct ChakraCoreRuntimeArgs : public ChakraRuntimeArgs {
  /**
   * @brief The queue on which debugging and native promise callbacks are
   * invoked. This argument must be supplied if one want to enable debugging and
   * native promise support.
   */
  std::shared_ptr<facebook::react::MessageQueueThread> jsQueue;

  /**
   * @brief Debugger configurations.
   */
  bool debuggerBreakOnNextLine{false};
  uint16_t debuggerPort{9229};
  std::string debuggerRuntimeName{"default runtime"};
  Logger debuggerLoggingCallback{[](const char *, LogLevel) {}};

  /**
   * @brief Enable native promist support.
   */
  bool enableNativePromiseSupport{false};
};

} // namespace Microsoft::JSI

#endif
