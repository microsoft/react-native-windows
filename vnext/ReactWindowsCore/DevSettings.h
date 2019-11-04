// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "Logging.h"
#include "MemoryTracker.h"

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#define STRING_(s) #s
#define STRING(s) STRING_(s)

namespace facebook {
namespace jsi {
struct RuntimeHolderLazyInit;
}
} // namespace facebook

namespace facebook {
namespace react {

enum class JSExceptionType : int32_t {
  Fatal = 0,
  Soft = 1,
};

struct JSExceptionInfo {
  JSExceptionType exceptionType;
  std::string exceptionMessage;
  uint32_t exceptionId;
  std::vector<std::string> callstack;
};

enum class JSIEngineOverride : int32_t {
  Default = 0, // No JSI, will use the legacy ExecutorFactory
  Chakra = 1, // Use the JSIExecutorFactory with ChakraRuntime
  ChakraCore = 2, // Use the JSIExecutorFactory with ChakraCoreRuntime
  Hermes = 3, // Use the JSIExecutorFactory with Hermes
  V8 = 4, // Use the JSIExecutorFactory with V8
  V8Lite = 5, // Use the JSIExecutorFactory with V8Lite

  Last = V8Lite
};

struct DevSettings {
  bool useSandbox{false};
  bool useJITCompilation{true};
  std::string sandboxPipeName;
  std::string debugHost;
  std::string debugBundlePath;
  std::string platformName{STRING(RN_PLATFORM)};
  std::function<void()> liveReloadCallback;
  std::function<void(std::string)> errorCallback;
  std::function<void()> waitingForDebuggerCallback;
  std::function<void()> debuggerAttachCallback;
  NativeLoggingHook loggingCallback;
  std::function<void(JSExceptionInfo &&)> jsExceptionCallback;

  /// Enables the user to set a custom root path for bundle resolution
  std::string bundleRootPath;

  /// A writeable directory which may be used for instance-internal data. The
  /// instance is not responsible for cleaning the path.
  std::filesystem::path scratchPath;

  /// Enables debugging directly in the JavaScript engine.
  bool useDirectDebugger{false};

  /// For direct debugging, break on the next line of JavaScript executed
  bool debuggerBreakOnNextLine{false};

  /// Enable function nativePerformanceNow.
  /// Method nativePerformanceNow() returns high resolution time info.
  /// It is not safe to expose to Custom Function. Add this flag so we can turn
  /// it off for Custom Function.
  bool enableNativePerformanceNow{true};

  /// For direct debugging, the port number to use, or zero for the default
  uint16_t debuggerPort{0};

  /// For direct debugging, name of runtime instance, or empty for default.
  std::string debuggerRuntimeName;

  /// Enables debugging by running the JavaScript in a web browser (Chrome)
  /// using http://localhost:8081/debugger-ui from the React Native packager
  /// (Metro / Haul). Debugging will start as soon as the react native instance
  /// is loaded.
  bool useWebDebugger{false};

  // Enables ChakraCore console redirection to debugger
  bool debuggerConsoleRedirection{false};

  /// Dispatcher for notifications about JS engine memory consumption.
  std::shared_ptr<MemoryTracker> memoryTracker;

  /// A factory and holder of jsi::Runtime instance to be used for this react
  /// instance. This object should in general be used only from the JS engine
  /// thread, unless the specific runtime implementation explicitly guarantees
  /// reentrancy.
  std::shared_ptr<jsi::RuntimeHolderLazyInit> jsiRuntimeHolder;

  // Until the ABI story is addressed we'll use this instead of the above for
  // the purposes of selecting a JSI Runtime to use.
  JSIEngineOverride jsiEngineOverride{JSIEngineOverride::Default};
};

} // namespace react
} // namespace facebook
