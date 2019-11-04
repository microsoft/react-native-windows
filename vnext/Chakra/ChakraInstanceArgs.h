// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#if defined(USE_EDGEMODE_JSRT)
#include <jsrt.h>
#else
#include <ChakraCore.h>
#endif

#include "DevSettings.h"

namespace facebook {
namespace react {

/**
 * @struct ChakraInstanceArgs
 *
 * @brief Arguments used during the instantiation of a Chakra Runtime.
 */
struct ChakraInstanceArgs {
  /**
   * @brief Attributes passed to JsCreateRuntime.
   */
  JsRuntimeAttributes RuntimeAttributes = JsRuntimeAttributeNone;

  /**
   * @brief Memory limit set on the newly created Chakra runtime.
   *
   * @remarks The value 0 means that no memory limit is being set on the
   * runtime.
   */
  size_t RuntimeMemoryLimit{0};

  NativeLoggingHook LoggingCallback{};

  /**
   * @brief Whether to enable debugging for the runtime instance.
   */
  bool EnableDebugging{false};

  /**
   * @brief Whether to break on the next line when debugging.
   */
  bool DebuggerBreakOnNextLine{false};

  /**
   * @brief Whether to enable function nativePerformanceNow.
   */
  bool EnableNativePerformanceNow{true};

  /**
   * @brief Whether to enable ChakraCore console redirection to debugger.
   */
  bool DebuggerConsoleRedirection{false};

  /**
   * @brief Port number to use when debugging.
   */
  uint16_t DebuggerPort{9229};

  /**
   * @brief Name to associate with runtime instance when debugging.
   */
  std::string DebuggerRuntimeName;

  /**
   * @brief Dispatcher for notifications about JS engine memory consumption.
   */
  std::shared_ptr<MemoryTracker> MemoryTracker;

  /**
   * @brief Maps script URL to metadata needed for bytecode caching. Should be
   * removed once we're on the JSI stack and using PreparedScriptStore.
   */
  std::shared_ptr<ScriptUrlMetadataMap> ScriptMetadata;
};

} // namespace react
} // namespace facebook
