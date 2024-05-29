// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "codegen/NativeExceptionsManagerSpec.g.h"
#include <NativeModules.h>

namespace Mso::React {
struct IRedBoxHandler;
}

namespace Microsoft::ReactNative {

REACT_MODULE(ExceptionsManager)
struct ExceptionsManager {
  using ModuleSpec = ReactNativeSpecs::ExceptionsManagerSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(reportFatalException)
  void reportFatalException(
      std::string message,
      std::vector<ReactNativeSpecs::ExceptionsManagerSpec_StackFrame> const &stack,
      double exceptionId) noexcept;

  REACT_METHOD(reportSoftException)
  void reportSoftException(
      std::string message,
      std::vector<ReactNativeSpecs::ExceptionsManagerSpec_StackFrame> const &stack,
      double exceptionId) noexcept;

  REACT_METHOD(reportException)
  void reportException(ReactNativeSpecs::ExceptionsManagerSpec_ExceptionData &&data) noexcept;

  REACT_METHOD(updateExceptionMessage)
  void updateExceptionMessage(
      std::string message,
      std::vector<ReactNativeSpecs::ExceptionsManagerSpec_StackFrame> const &stack,
      double exceptionId) noexcept;

  REACT_METHOD(dismissRedbox)
  void dismissRedbox() noexcept;

  static void SetRedBoxHander(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &properties,
      std::shared_ptr<Mso::React::IRedBoxHandler> handler) noexcept;

 private:
  std::shared_ptr<Mso::React::IRedBoxHandler> m_redboxHandler;
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
};

} // namespace Microsoft::ReactNative
