// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ChakraRuntime.h"

#include "ChakraCore.h"
#include "ChakraCoreDebugger.h"

namespace Microsoft::JSI {

class ChakraCoreRuntime : public ChakraRuntime {
 public:
  ChakraCoreRuntime(ChakraRuntimeArgs &&args) noexcept;

  ~ChakraCoreRuntime() noexcept;

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

 private:
  JsErrorCode enableDebugging(
      JsRuntimeHandle runtime,
      std::string const &runtimeName,
      bool breakOnNextLine,
      uint16_t port,
      std::unique_ptr<DebugProtocolHandler> &debugProtocolHandler,
      std::unique_ptr<DebugService> &debugService);

  void ProcessDebuggerCommandQueue();

  static void CALLBACK ProcessDebuggerCommandQueueCallback(void *callbackState);

  std::unique_ptr<DebugProtocolHandler> m_debugProtocolHandler;
  std::unique_ptr<DebugService> m_debugService;
};

} // namespace Microsoft::JSI
