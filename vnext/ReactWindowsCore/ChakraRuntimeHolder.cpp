#include "pch.h"

#include "ChakraRuntimeHolder.h"

#include <JSI/Shared/ChakraRuntimeFactory.h>

namespace Microsoft::JSI {

std::shared_ptr<facebook::jsi::Runtime> ChakraRuntimeHolder::getRuntime() noexcept {
  std::call_once(once_flag_, [this]() { initRuntime(); });

  if (!runtime_)
    std::terminate();

  // ChakraRuntime is not thread safe as of now.
  if (own_thread_id_ != std::this_thread::get_id())
    std::terminate();

  return runtime_;
}

void ChakraRuntimeHolder::initRuntime() noexcept {
  runtime_ = Microsoft::JSI::makeChakraRuntime(std::move(args_));
  own_thread_id_ = std::this_thread::get_id();
}

Logger ChakraRuntimeHolder::ChakraRuntimeLoggerFromReactLogger(
    facebook::react::NativeLoggingHook loggingCallback) noexcept {
  return [loggingCallback = std::move(loggingCallback)](const char *message, LogLevel logLevel) -> void {
    loggingCallback(static_cast<facebook::react::RCTLogLevel>(logLevel), message);
  };
}

ChakraRuntimeArgs ChakraRuntimeHolder::RuntimeArgsFromDevSettings(
    std::shared_ptr<facebook::react::DevSettings> devSettings) noexcept {
  ChakraRuntimeArgs runtimeArgs;

  runtimeArgs.debuggerBreakOnNextLine = devSettings->debuggerBreakOnNextLine;
  runtimeArgs.debuggerPort = devSettings->debuggerPort;
  runtimeArgs.debuggerRuntimeName = devSettings->debuggerRuntimeName;

  runtimeArgs.enableDebugging = devSettings->useDirectDebugger;

  if (devSettings->loggingCallback) {
    runtimeArgs.loggingCallback = ChakraRuntimeLoggerFromReactLogger(devSettings->loggingCallback);
  }

  runtimeArgs.enableNativePerformanceNow = devSettings->enableNativePerformanceNow;

  runtimeArgs.enableJITCompilation = devSettings->useJITCompilation;

  runtimeArgs.memoryTracker = devSettings->memoryTracker;

  return runtimeArgs;
}

} // namespace Microsoft::JSI
