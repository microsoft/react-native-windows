// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactModuleBuilderMock.h"
#include "JSValue.h"

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// ReactModuleBuilderMock implementation
//===========================================================================

void ReactModuleBuilderMock::SetEventEmitterName(hstring const &name) noexcept {
  m_eventEmitterName = name;
}

void ReactModuleBuilderMock::AddMethod(
    hstring const &name,
    MethodReturnType returnType,
    MethodDelegate const &method) noexcept {
  m_methods.emplace(name, std::tuple<MethodReturnType, MethodDelegate>{returnType, method});
}

void ReactModuleBuilderMock::AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept {
  m_syncMethods.emplace(name, method);
}

void ReactModuleBuilderMock::AddConstantProvider(ConstantProvider const &constantProvider) noexcept {
  m_constProviders.push_back(constantProvider);
}

void ReactModuleBuilderMock::AddEventHandlerSetter(
    hstring const &name,
    ReactEventHandlerSetter const &eventHandlerSetter) noexcept {
  eventHandlerSetter([ this, name = std::wstring{name} ](ReactArgWriter const &argWriter) noexcept {
    auto it = m_eventHandlers.find(name);
    if (it != m_eventHandlers.end()) {
      JSValue jsValue;
      auto writer = MakeJSValueTreeWriter(jsValue);
      writer.WriteArrayBegin();
      argWriter(writer);
      writer.WriteArrayEnd();
      auto reader = MakeJSValueTreeReader(jsValue);
      it->second(reader);
    }
  });
}

JSValueObject ReactModuleBuilderMock::GetConstants() noexcept {
  JSValue jsValue;
  auto constantWriter = MakeJSValueTreeWriter(jsValue);
  constantWriter.WriteObjectBegin();
  for (const auto &constantProvider : m_constProviders) {
    constantProvider(constantWriter);
  }

  constantWriter.WriteObjectEnd();
  return jsValue.TakeObject();
}

MethodDelegate ReactModuleBuilderMock::GetMethod0(std::wstring const &methodName) const noexcept {
  auto it = m_methods.find(methodName);
  return (it != m_methods.end() && std::get<0>(it->second) == MethodReturnType::Void) ? std::get<1>(it->second)
                                                                                      : nullptr;
}

MethodDelegate ReactModuleBuilderMock::GetMethod1(std::wstring const &methodName) const noexcept {
  auto it = m_methods.find(methodName);
  return (it != m_methods.end() && std::get<0>(it->second) == MethodReturnType::Callback) ? std::get<1>(it->second)
                                                                                          : nullptr;
}

MethodDelegate ReactModuleBuilderMock::GetMethod2(std::wstring const &methodName) const noexcept {
  auto it = m_methods.find(methodName);
  return (it != m_methods.end() &&
          (std::get<0>(it->second) == MethodReturnType::TwoCallbacks ||
           std::get<0>(it->second) == MethodReturnType::Promise))
      ? std::get<1>(it->second)
      : nullptr;
}

SyncMethodDelegate ReactModuleBuilderMock::GetSyncMethod(std::wstring const &methodName) const noexcept {
  auto it = m_syncMethods.find(methodName);
  return (it != m_syncMethods.end()) ? it->second : nullptr;
}

/*static*/ IJSValueWriter ReactModuleBuilderMock::ArgWriter(JSValue &jsValue) noexcept {
  return MakeJSValueTreeWriter(jsValue);
}

/*static*/ IJSValueReader ReactModuleBuilderMock::CreateArgReader(
    std::function<void(IJSValueWriter const &)> const &argWriter) noexcept {
  JSValue jsValue;
  auto writer = MakeJSValueTreeWriter(jsValue);
  argWriter(writer);
  return MakeJSValueTreeReader(std::move(jsValue));
}

} // namespace winrt::Microsoft::ReactNative
