// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TurboModuleProvider.h"

namespace winrt::Microsoft::ReactNative {

// CallInvoker implementation based on JSDispatcher.
struct AbiCallInvoker final : facebook::react::CallInvoker {
  AbiCallInvoker(IReactDispatcher const &jsDispatcher) : m_jsDispatcher(jsDispatcher) {}

  void invokeAsync(std::function<void()> &&func) override {
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

// Creates CallInvoker based on JSDispatcher.
std::shared_ptr<facebook::react::CallInvoker> MakeAbiCallInvoker(IReactDispatcher const &jsDispatcher) noexcept {
  return std::make_shared<AbiCallInvoker>(jsDispatcher);
}

} // namespace winrt::Microsoft::ReactNative
