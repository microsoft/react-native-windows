// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TurboModuleProvider.h"

namespace winrt::Microsoft::ReactNative {

ReactPropertyId<ReactNonAbiValue<std::weak_ptr<facebook::react::CallInvoker>>> CallInvokerProperty() noexcept {
  ReactPropertyId<ReactNonAbiValue<std::weak_ptr<facebook::react::CallInvoker>>> propId{
      L"ReactNative.JSI", L"CallInvoker"};
  return propId;
}

// CallInvoker implementation based on JSDispatcher.
struct AbiCallInvoker final : facebook::react::CallInvoker {
  AbiCallInvoker(IReactDispatcher const &jsDispatcher) : m_jsDispatcher(jsDispatcher) {}

  void invokeAsync(std::function<void()> &&func) noexcept override {
    m_jsDispatcher.Post([func = std::move(func)]() { func(); });
  }

  void invokeSync(std::function<void()> && /*func*/) override {
    // Throwing an exception in this method matches the behavior of
    // Instance::JSCallInvoker::invokeSync in react-native\ReactCommon\cxxreact\Instance.cpp
    throw std::runtime_error("Synchronous native -> JS calls are currently not supported.");
  }

 private:
  IReactDispatcher m_jsDispatcher{nullptr};
};

std::shared_ptr<facebook::react::CallInvoker> GetCallInvokerFromContext(IReactContext const &context) noexcept {
  // Try to get the CallInvoker instance from Context
  const auto callInvokerPropertyValue = ReactPropertyBag{context.Properties()}.Get(CallInvokerProperty());
  const auto weakCallInvoker =
      callInvokerPropertyValue ? callInvokerPropertyValue.Value() : std::weak_ptr<facebook::react::CallInvoker>{};
  if (const auto callInvoker = weakCallInvoker.lock()) {
    return callInvoker;
  }

  // Fallback to creating CallInvoker based on JSDispatcher.
  return std::make_shared<AbiCallInvoker>(context.JSDispatcher());
}

void SetCallInvoker(
    IReactPropertyBag const &properties,
    std::weak_ptr<facebook::react::CallInvoker> const &value) noexcept {
  ReactPropertyBag{properties}.Set(CallInvokerProperty(), value);
}

} // namespace winrt::Microsoft::ReactNative
