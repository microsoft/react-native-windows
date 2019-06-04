// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <jsi/jsi.h>
#include <jsi/ScriptStore.h>

namespace facebook {
namespace react {
class MemoryTracker;
class MessageQueueThread;
}}

namespace facebook {
namespace jsi {
namespace chakraruntime {

enum class LogLevel
{
  Trace = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4,
};

using Logger = std::function<void(const char* message, LogLevel logLevel)>;

struct ChakraJsiRuntimeArgs
{
  bool enableJITCompilation { true };

  Logger loggingCallback {};

  // Debugger configuration.
  bool enableDebugging { false };
  bool debuggerBreakOnNextLine { false };
  bool enableNativePerformanceNow { true };
  bool enableNativePromiseSupport { false };
  uint16_t debuggerPort { 9229 };
  std::string debuggerRuntimeName;

  // Memory governance.
  size_t runtimeMemoryLimit { 0 };
  std::shared_ptr<facebook::react::MemoryTracker> memoryTracker;

  std::shared_ptr<facebook::react::MessageQueueThread> jsQueue;

  // Script store which manages script and prepared script storage and versioning.
  std::unique_ptr<jsi::ScriptStore> scriptStore;
  std::unique_ptr<jsi::PreparedScriptStore> preparedScriptStore;
};

}}}
