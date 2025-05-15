// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "CallInvoker.h"

#include "CallInvoker.g.cpp"
#include "JsiApi.h"

namespace winrt::Microsoft::ReactNative::implementation {

static const ReactPropertyId<winrt::Microsoft::ReactNative::CallInvoker> &CallInvokerPropertyId() noexcept {
  static const ReactPropertyId<winrt::Microsoft::ReactNative::CallInvoker> prop{L"ReactNative", L"CallInvoker"};
  return prop;
}

CallInvoker::CallInvoker(
    Mso::React::IReactContext &reactContext,
    std::shared_ptr<facebook::react::CallInvoker> callInvoker) noexcept
    : m_callInvoker(callInvoker), m_context(&reactContext) {}

void CallInvoker::InvokeAsync(CallFunc func) noexcept {
  m_callInvoker->invokeAsync(
      [reactContext = m_context, func](facebook::jsi::Runtime & /*runtime*/) { func(reactContext->JsiRuntime()); });
}

void CallInvoker::InvokeSync(CallFunc func) noexcept {
  m_callInvoker->invokeSync(
      [reactContext = m_context, func](facebook::jsi::Runtime & /*runtime*/) { func(reactContext->JsiRuntime()); });
}

winrt::Microsoft::ReactNative::CallInvoker CallInvoker::FromProperties(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &properties) noexcept {
  return properties.Get(CallInvokerPropertyId());
}

void CallInvoker::SetProperties(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &properties,
    winrt::Microsoft::ReactNative::CallInvoker invoker) noexcept {
  properties.Set(CallInvokerPropertyId(), invoker);
}

} // namespace winrt::Microsoft::ReactNative::implementation
