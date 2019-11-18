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

/**
 * @remarks We do not make this class final to make sure that it can work with
 * Facebook's RuntimeDecorator class.
 */
class ChakraCoreRuntime : public ChakraRuntime {
 public:
  ChakraCoreRuntime(std::shared_ptr<ChakraCoreRuntimeArgs> &&args) noexcept;
  ~ChakraCoreRuntime() noexcept;

  // Functions inherited from jsi::Runtime =====================================

  facebook::jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &preparedJs) override;

  std::string description() override;

 protected:
  /**
   * @remarks We override the following because ChakraCore (unlike Chakra)
   * offers weak reference semantics.
   */
  /**@{*/
  facebook::jsi::WeakObject createWeakObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::Value lockWeakObject(const facebook::jsi::WeakObject &weakObj) override;
  /**@}*/

  //============================================================================

  // Functions inherited from ChakraRuntime ====================================

  facebook::jsi::JSRuntimeVersion_t Version() override;

 private:
  facebook::jsi::Value EvaluateJavaScriptSimple(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      const std::string &sourceUrl) override;

  std::shared_ptr<const facebook::jsi::Buffer> SerializeScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer) override;

  /**
   * @remarks ChakraCoreRuntime supports debugging through the Chrome debugging
   * protocol.
   */
  void StartDebugging() override;

  void InitializeAdditionalFeatures() override;

  //============================================================================

  using WeakChakraPointerValue = ChakraPointerValueTemplate<JsWeakRef>;

  /**
   * @param source A ChakraObjectRef managing a JS ArrayBuffer or JS
   * ExternalArrayBuffer.
   *
   * @param sourceUrl A ChakraObjectRef managing a JS string.
   */
  facebook::jsi::Value EvaluateJavaScriptImpl(const ChakraObjectRef &source, const ChakraObjectRef &sourceUrl);

  void ProcessDebuggerCommandQueue();
  static void CALLBACK ProcessDebuggerCommandQueueCallback(void *callbackState);

  void PromiseContinuation(ChakraObjectRef &&value) noexcept;
  void PromiseRejectionTracker(ChakraObjectRef &&promise, ChakraObjectRef &&reason, bool handled);
  static void CALLBACK PromiseContinuationCallback(JsValueRef funcRef, void *callbackState);
  // clang-format off
  static void CALLBACK PromiseRejectionTrackerCallback(
    JsValueRef promise, JsValueRef reason, bool handled, void *callbackState);
  // clang-format on

 private:
  std::shared_ptr<ChakraCoreRuntimeArgs> m_args;

  std::shared_ptr<DebugProtocolHandler> m_debugProtocolHandler;
  std::unique_ptr<DebugService> m_debugService;

  /**
   * @remarks See the implementation of PromiseContinuation for why this is
   * needed.
   */
  std::shared_ptr<bool> m_runtimeIsAlive;
};

} // namespace Microsoft::JSI

#endif
