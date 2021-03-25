// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ChakraRuntime.h"

//#ifndef USE_EDGEMODE_JSRT
//#define USE_EDGEMODE_JSRT
//#endif
//#include <jsrt.h>


namespace Microsoft::JSI {

class SystemChakraRuntime : public ChakraRuntime {

  #pragma region ChakraRuntime

  void setupNativePromiseContinuation() noexcept override;

  void startDebuggingIfNeeded() override;

  void stopDebuggingIfNeeded() override;

  std::unique_ptr<const facebook::jsi::Buffer> generatePreparedScript(
      const std::string &sourceURL,
      const facebook::jsi::Buffer &sourceBuffer) noexcept override;

  facebook::jsi::Value evaluateJavaScriptSimple(const facebook::jsi::Buffer &buffer, const std::string &sourceURL)
      override;

  bool evaluateSerializedScript(
      const facebook::jsi::Buffer &scriptBuffer,
      const facebook::jsi::Buffer &serializedScriptBuffer,
      const std::string &sourceURL) override;

#pragma endregion
};

} // namespace Microsoft::JSI
