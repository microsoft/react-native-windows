// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ExceptionsManager.h"

#include <DynamicWriter.h>
#include <IRedBoxHandler.h>
#include <JSValueWriter.h>

namespace Microsoft::ReactNative {

static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<Mso::React::IRedBoxHandler>>>
    &RedBoxHandlerPropertyId() noexcept {
  static const React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<Mso::React::IRedBoxHandler>>> prop{
      L"ReactNative.ExceptionsManager", L"RedBoxHandler"};
  return prop;
}

Mso::React::ErrorInfo CreateErrorInfo(
    std::string message,
    std::vector<ReactNativeSpecs::ExceptionsManagerSpec_StackFrame> const &stack,
    double exceptionId) {
  Mso::React::ErrorInfo errorInfo;
  errorInfo.Message = message;
  errorInfo.Id = static_cast<uint32_t>(exceptionId);
  for (auto frame : stack) {
    errorInfo.Callstack.push_back(Mso::React::ErrorFrameInfo{
        frame.file.value_or(""),
        frame.methodName,
        static_cast<int>(frame.lineNumber.value_or(0)),
        static_cast<int>(frame.column.value_or(0)),
        frame.collapse.value_or(false)});
  }
  return errorInfo;
};

void ExceptionsManager::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_reactContext = reactContext;
  m_redboxHandler = *m_reactContext.Properties().Get(RedBoxHandlerPropertyId());
}

void ExceptionsManager::reportFatalException(
    std::string message,
    std::vector<ReactNativeSpecs::ExceptionsManagerSpec_StackFrame> const &stack,
    double exceptionId) noexcept {
  if (m_redboxHandler && m_redboxHandler->isDevSupportEnabled()) {
    m_redboxHandler->showNewError(
        std::move(CreateErrorInfo(message, stack, exceptionId)), Mso::React::ErrorType::JSFatal);
  }
}

void ExceptionsManager::reportSoftException(
    std::string message,
    std::vector<ReactNativeSpecs::ExceptionsManagerSpec_StackFrame> const &stack,
    double exceptionId) noexcept {
  if (m_redboxHandler && m_redboxHandler->isDevSupportEnabled()) {
    m_redboxHandler->showNewError(
        std::move(CreateErrorInfo(message, stack, exceptionId)), Mso::React::ErrorType::JSSoft);
  }
}

void ExceptionsManager::reportException(ReactNativeSpecs::ExceptionsManagerSpec_ExceptionData &&data) noexcept {
  if (m_redboxHandler && m_redboxHandler->isDevSupportEnabled()) {
    Mso::React::ErrorInfo errorInfo;
    errorInfo.Message = data.message;
    errorInfo.OriginalMessage = data.originalMessage.value_or("");
    errorInfo.Name = data.name.value_or("");
    errorInfo.ComponentStack = data.componentStack.value_or("");
    errorInfo.Id = static_cast<uint32_t>(data.id);

    auto writer = winrt::make_self<winrt::Microsoft::ReactNative::DynamicWriter>();
    winrt::Microsoft::ReactNative::WriteValue(
        writer.as<winrt::Microsoft::ReactNative::IJSValueWriter>(), data.extraData);
    errorInfo.ExtraData = writer->TakeValue();

    for (auto frame : data.stack) {
      errorInfo.Callstack.push_back(Mso::React::ErrorFrameInfo{
          frame.file.value_or(""),
          frame.methodName,
          static_cast<int>(frame.lineNumber.value_or(0)),
          static_cast<int>(frame.column.value_or(0)),
          frame.collapse.value_or(false)});
    }
    m_redboxHandler->showNewError(
        std::move(errorInfo), data.isFatal ? Mso::React::ErrorType::JSFatal : Mso::React::ErrorType::JSSoft);
  }
}

void ExceptionsManager::updateExceptionMessage(
    std::string message,
    std::vector<ReactNativeSpecs::ExceptionsManagerSpec_StackFrame> const &stack,
    double exceptionId) noexcept {
  if (m_redboxHandler && m_redboxHandler->isDevSupportEnabled()) {
    m_redboxHandler->updateError(std::move(CreateErrorInfo(message, stack, exceptionId)));
  }
}

void ExceptionsManager::dismissRedbox() noexcept {
  if (m_redboxHandler)
    m_redboxHandler->dismissRedbox();
}

void ExceptionsManager::SetRedBoxHander(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &properties,
    std::shared_ptr<Mso::React::IRedBoxHandler> handler) noexcept {
  properties.Set(RedBoxHandlerPropertyId(), handler);
}

} // namespace Microsoft::ReactNative
