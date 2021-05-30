// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ChakraRuntime.h"

namespace Microsoft::JSI {

class SystemChakraRuntime : public ChakraRuntime {
 public:
  SystemChakraRuntime(ChakraRuntimeArgs &&args) noexcept;

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
      const std::string &sourceURL,
      JsValueRef *result) override;

#pragma endregion
};

} // namespace Microsoft::JSI
