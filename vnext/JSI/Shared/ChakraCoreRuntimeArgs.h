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
 public:
  /**
   * TODO (yicyao)
   */
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue{nullptr};

  /**
   * TODO (yicyao)
   */
  bool m_debuggerBreakOnNextLine{false};
  uint16_t m_debuggerPort{9229};
  std::string m_debuggerRuntimeName{"default runtime"};
  Logger m_debuggerLoggingCallback{[](const char *, LogLevel) {}};

  /**
   * TODO (yicyao)
   */
  bool m_enableNativePromiseSupport{false};
};

} // namespace Microsoft::JSI

#endif
