#include "pch.h"

#include "ChakraJSIRuntimeHolder.h"

#include <Chakra/ChakraJsiRuntimeFactory.h>

using namespace facebook;
using namespace facebook::react;
using namespace facebook::jsi::chakraruntime;

namespace react {
namespace uwp {

std::shared_ptr<facebook::jsi::Runtime>
ChakraJSIRuntimeHolder::getRuntime() noexcept {
  std::call_once(once_flag_, [this]() { initRuntime(); });

  if (!runtime_)
    std::terminate();

  // ChakraJsiRuntime is not thread safe as of now.
  if (own_thread_id_ != std::this_thread::get_id())
    std::terminate();

  return runtime_;
}

void ChakraJSIRuntimeHolder::initRuntime() noexcept {
  runtime_ =
      facebook::jsi::chakraruntime::makeChakraJsiRuntime(std::move(args_));
  own_thread_id_ = std::this_thread::get_id();
}

Logger ChakraJSIRuntimeHolder::ChakraRuntimeLoggerFromReactLogger(
    facebook::react::NativeLoggingHook loggingCallback) noexcept {
  return [loggingCallback = std::move(loggingCallback)](
             const char *message, LogLevel logLevel) -> void {
    loggingCallback(
        static_cast<facebook::react::RCTLogLevel>(logLevel), message);
  };
}

ChakraJsiRuntimeArgs ChakraJSIRuntimeHolder::RuntimeArgsFromDevSettings(
    std::shared_ptr<facebook::react::DevSettings> devSettings) noexcept {
  ChakraJsiRuntimeArgs runtimeArgs;

  runtimeArgs.debuggerBreakOnNextLine = devSettings->debuggerBreakOnNextLine;
  runtimeArgs.debuggerPort = devSettings->debuggerPort;
  runtimeArgs.debuggerRuntimeName = devSettings->debuggerRuntimeName;

  runtimeArgs.enableDebugging = devSettings->useDirectDebugger;

  if (devSettings->loggingCallback) {
    runtimeArgs.loggingCallback =
        ChakraRuntimeLoggerFromReactLogger(devSettings->loggingCallback);
  }

  runtimeArgs.enableNativePerformanceNow =
      devSettings->enableNativePerformanceNow;

  runtimeArgs.enableJITCompilation = devSettings->useJITCompilation;

  runtimeArgs.memoryTracker = devSettings->memoryTracker;

  return runtimeArgs;
}

} // namespace uwp
} // namespace react
