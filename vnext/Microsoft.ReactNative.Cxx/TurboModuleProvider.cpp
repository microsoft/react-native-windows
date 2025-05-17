// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TurboModuleProvider.h"

#include "JSI/JsiApiContext.h"

namespace winrt::Microsoft::ReactNative {

// CallInvoker implementation based on JSDispatcher.
struct AbiCallInvoker final : facebook::react::CallInvoker {
  AbiCallInvoker(IReactContext const &context) : m_context(context) {}

  void invokeAsync(facebook::react::CallFunc &&func) noexcept override {
    m_context.CallInvoker().InvokeAsync(
        [context = m_context, func = std::move(func)](const winrt::Windows::Foundation::IInspectable &runtimeHandle) {
          func(GetOrCreateContextRuntime(context, runtimeHandle));
        });
  }

  void invokeSync(facebook::react::CallFunc &&func) override {
    m_context.CallInvoker().InvokeSync(
        [context = m_context, func = std::move(func)](const winrt::Windows::Foundation::IInspectable &runtimeHandle) {
          func(GetOrCreateContextRuntime(context, runtimeHandle));
        });
  }

 private:
  IReactContext m_context{nullptr};
};

// Creates CallInvoker based on JSDispatcher.
std::shared_ptr<facebook::react::CallInvoker> MakeAbiCallInvoker(IReactContext const &context) noexcept {
  return std::make_shared<AbiCallInvoker>(context);
}

} // namespace winrt::Microsoft::ReactNative
