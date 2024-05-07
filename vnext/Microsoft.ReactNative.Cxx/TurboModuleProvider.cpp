// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TurboModuleProvider.h"

namespace winrt::Microsoft::ReactNative {

// CallInvoker implementation based on JSDispatcher.
struct AbiCallInvoker final : facebook::react::CallInvoker {
  AbiCallInvoker(IReactContext const &context) : m_context(context) {}

  void invokeAsync(std::function<void()> &&func) noexcept override {
    m_context.JSDispatcher().Post([func = std::move(func)]() { func(); });
  }

  void invokeAsync(facebook::react::CallFunc &&func) noexcept override {
    // If async is allowed to run sync if already on the JS thread, this could be replaced with just the ExecuteJsi call
    m_context.JSDispatcher().Post([context = m_context, func = std::move(func)]() {
      winrt::Microsoft::ReactNative::ExecuteJsi(context, std::move(func));
    });
  }

  virtual void invokeSync(facebook::react::CallFunc &&func) override {
    // Throwing an exception in this method matches the behavior of
    // Instance::JSCallInvoker::invokeSync in react-native\ReactCommon\cxxreact\Instance.cpp
    UNREFERENCED_PARAMETER(func);
    throw std::runtime_error("Synchronous native -> JS calls are currently not supported.");
  }

 private:
  IReactContext m_context{nullptr};
};

// Creates CallInvoker based on JSDispatcher.
std::shared_ptr<facebook::react::CallInvoker> MakeAbiCallInvoker(IReactContext const &context) noexcept {
  return std::make_shared<AbiCallInvoker>(context);
}

} // namespace winrt::Microsoft::ReactNative
