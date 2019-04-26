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
namespace react {
namespace chakra {

enum class LogLevel
{
  Trace = 0,
  Info = 1,
  Warning = 2,
  Error = 3,
  Fatal = 4,
};

using Logger = std::function<void(const char* message, LogLevel logLevel)>;

struct ChakraRuntimeArgs
{
  bool enableJITCompilation { true };

  Logger loggingCallback {};

  // Debugger configuration.
  bool enableDebugging { false };
  bool debuggerBreakOnNextLine { false };
  bool enableNativePerformanceNow { true };
  int debuggerPort { 9229 };
  std::string debuggerRuntimeName;

  // Memory governance.
  size_t runtimeMemoryLimit { 0 };
  std::shared_ptr<MemoryTracker> memoryTracker;

  std::shared_ptr<MessageQueueThread> jsQueue;

  // Script store which manages script and prepared script storage and versioning.
  std::unique_ptr<jsi::ScriptStore> scriptStore;
  std::unique_ptr<jsi::PreparedScriptStore> preparedScriptStore;
};

#ifndef JSI_CORE

std::unique_ptr<jsi::Runtime> makeChakraRuntime(ChakraRuntimeArgs&& args);

#else // JSI_CORE

struct IPromiseContinuation
{
  virtual void registerPromise(facebook::jsi::Function&& func) = 0;
};

std::unique_ptr<jsi::Runtime> makeChakraRuntime(IPromiseContinuation&);
std::unique_ptr<jsi::Runtime> makeChakraRuntime();

#endif // JSI_CORE

} // namespace chakra
} // namespace react
} // namespace facebook
