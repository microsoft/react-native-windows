// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstance.h"
#if __has_include("Bridge.ReactInstance.g.cpp")
#include "Bridge.ReactInstance.g.cpp"
#endif

#include "ReactSupport.h"

namespace winrt::Microsoft::ReactNative::Bridge::implementation {
void ReactInstance::InvokeFunction(
    hstring const &moduleName,
    hstring const &method,
    IVectorView<IInspectable> const &arguments) {
  folly::dynamic args =
      Microsoft::ReactNative::Bridge::ConvertToDynamic(arguments);

  m_instance->CallJsFunction(
      to_string(moduleName), to_string(method), std::move(args));
}

IAsyncAction ReactInstance::InitializeAsync() {
  if (m_instance == nullptr)
    throw hresult_invalid_operation(L"Internal instance is null.");

  // Notify all registered native modules by calling their Initialize
  // method.  Modules shouldn't be on the UI thread anyway so do it
  // in the background.

  apartment_context current_thread;
  co_await resume_background();

  auto modules = NativeModules();
  for (auto module : modules) {
    module.Initialize();
  }

  co_await current_thread;
}

} // namespace winrt::Microsoft::ReactNative::Bridge::implementation
