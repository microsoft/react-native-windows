// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Logging.h"

#include <IRedBoxHandler.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#define STRING_(s) #s
#define STRING(s) STRING_(s)

namespace Microsoft::JSI {
struct RuntimeHolderLazyInit;
} // namespace Microsoft::JSI

namespace facebook {
namespace react {

enum class JSIEngineOverride : int32_t {
  Default = 0, // No JSI, will use the legacy ExecutorFactory
  Chakra = 1, // Use the JSIExecutorFactory with ChakraRuntime

#if 0 // Deprecated
  ChakraCore = 2, // Use the JSIExecutorFactory with ChakraCoreRuntime
#endif

  Hermes = 3, // Use the JSIExecutorFactory with Hermes
  V8 = 4, // Use the JSIExecutorFactory with V8
  V8Lite = 5, // Use the JSIExecutorFactory with V8Lite
  V8NodeApi = 6, // Use the JSIExecutorFactory with V8 via ABI-safe NAPI

  Last = V8NodeApi
};

struct DevSettings {
  bool useJITCompilation{true};
  uint16_t sourceBundlePort{0};
  std::string sourceBundleHost;
  std::string debugBundlePath;
  std::string platformName{STRING(RN_PLATFORM)};
  std::string bundleAppId;
  std::function<void()> liveReloadCallback;
  std::function<void(std::string)> errorCallback;
  std::function<void()> waitingForDebuggerCallback;
  std::function<void()> debuggerAttachCallback;
  NativeLoggingHook loggingCallback;
  std::shared_ptr<Mso::React::IRedBoxHandler> redboxHandler;

  /// Enables the user to set a custom root path for bundle resolution
  std::string bundleRootPath;

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
  /// (Metro / Haul). Debugging will start as soon as the React Native instance
  /// is loaded.
  bool useWebDebugger{false};

  bool useFastRefresh{false};

  /// A factory and holder of jsi::Runtime instance to be used for this react
  /// instance. This object should in general be used only from the JS engine
  /// thread, unless the specific runtime implementation explicitly guarantees
  /// reentrancy.
  std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> jsiRuntimeHolder;

  // Until the ABI story is addressed we'll use this instead of the above for
  // the purposes of selecting a JSI Runtime to use.
  JSIEngineOverride jsiEngineOverride{JSIEngineOverride::Default};

  /// Callback to show the devmenu
  std::function<void()> showDevMenuCallback;

  bool inlineSourceMap{true};

  // When querying the bundle server for a bundle, should it request the dev bundle or release bundle
  bool devBundle{true};

  bool enableDefaultCrashHandler{false};

  // Transitory. Used to indicate whether or not to load networking types in the default Cxx module registry.
  bool omitNetworkingCxxModules{false};

  // OC:8368383 - Memory leak under investigation.
  bool useWebSocketTurboModule{false};

  // If true, then use only Turbo Modules instead of CxxModules.
  bool useTurboModulesOnly{false};
};

} // namespace react
} // namespace facebook
