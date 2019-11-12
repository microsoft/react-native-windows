// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef CHAKRACORE

#include "ChakraCoreRuntimeArgs.h"
#include "ChakraRuntime.h"

#include "ChakraCoreDebugger.h"

#include <memory>
#include <string>

namespace Microsoft::JSI {

class ChakraCoreRuntime : public ChakraRuntime {
 public:
  ChakraCoreRuntime(const std::shared_ptr<ChakraCoreRuntimeArgs> &args) noexcept;
  ~ChakraCoreRuntime() noexcept;

#pragma region Functions_inherited_from_Runtime

  facebook::jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &preparedJs) override;

  std::string description() override;

 private:
  // We override the following because ChakraCore (unlike Chakra) offers
  // weak reference semantics.
  facebook::jsi::WeakObject createWeakObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::Value lockWeakObject(const facebook::jsi::WeakObject &weakObj) override;

#pragma endregion Functions_inherited_from_Runtime

#pragma region Functions_inherited_from_ChakraRuntime

  facebook::jsi::Value EvaluateJavaScriptSimple(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      const std::string &sourceUrl) override;

  std::shared_ptr<const facebook::jsi::Buffer> SerializeScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer) override;

  void StartDebugging() override;

  void InitializeAdditionalFeatures() override;

#pragma endregion Functions_inherited_from_ChakraRuntime

  using WeakChakraPointerValue = ChakraPointerValueTemplate<JsWeakRef>;

  // source must be a JS ArrayBuffer or JS ExternalArrayBuffer. sourceUrl must
  // be a JS string.
  facebook::jsi::Value EvaluateJavaScriptImpl(const ChakraObjectRef &source, const ChakraObjectRef &sourceUrl);

  // In-proc debugging helpers
  void ProcessDebuggerCommandQueue();
  static void CALLBACK ProcessDebuggerCommandQueueCallback(void *callbackState);

  // Promise Helpers
  void PromiseContinuation(ChakraObjectRef &&value) noexcept;
  void PromiseRejectionTracker(ChakraObjectRef &&promise, ChakraObjectRef &&reason, bool handled);
  static void CALLBACK PromiseContinuationCallback(JsValueRef funcRef, void *callbackState) noexcept;
  static void CALLBACK
  PromiseRejectionTrackerCallback(JsValueRef promise, JsValueRef reason, bool handled, void *callbackState);

  std::shared_ptr<ChakraCoreRuntimeArgs> m_args{nullptr};

  std::shared_ptr<DebugProtocolHandler> m_debugProtocolHandler{nullptr};
  std::unique_ptr<DebugService> m_debugService{nullptr};

  std::shared_ptr<bool> m_runtimeIsAlive{nullptr};
};

} // namespace Microsoft::JSI

#endif
