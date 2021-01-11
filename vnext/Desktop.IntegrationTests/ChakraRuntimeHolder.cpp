#include "ChakraRuntimeHolder.h"

#include <JSI/ChakraRuntimeFactory.h>

using namespace facebook;
using namespace facebook::react;

namespace Microsoft::React::Test {

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

Microsoft::JSI::Logger ChakraRuntimeHolder::ChakraRuntimeLoggerFromReactLogger(
    facebook::react::NativeLoggingHook loggingCallback) noexcept {
  return
      [loggingCallback = std::move(loggingCallback)](const char *message, Microsoft::JSI::LogLevel logLevel) -> void {
        loggingCallback(static_cast<facebook::react::RCTLogLevel>(logLevel), message);
      };
}

Microsoft::JSI::ChakraRuntimeArgs ChakraRuntimeHolder::RuntimeArgsFromDevSettings(
    std::shared_ptr<facebook::react::DevSettings> devSettings) noexcept {
  Microsoft::JSI::ChakraRuntimeArgs runtimeArgs;

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

} // namespace Microsoft::React::Test
