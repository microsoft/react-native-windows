// Copyright (c) Microsoft Corporation.
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
   * @brief Port number to use when debugging.
   */
  uint16_t DebuggerPort{9229};

  /**
   * @brief Name to associate with runtime instance when debugging.
   */
  std::string DebuggerRuntimeName;
};

} // namespace react
} // namespace facebook
