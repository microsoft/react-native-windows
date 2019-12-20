// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactInstance.g.h"

#include <ReactUWP/IReactInstance.h>
#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactInstance : ReactInstanceT<ReactInstance> {
  ReactInstance() = default;
  ReactInstance(std::shared_ptr<react::uwp::IReactInstance> instance) : m_instance(instance) {
    if (instance == nullptr) {
      throw hresult_null_argument(L"instance");
    }
  }

  void InvokeFunction(
      hstring const &moduleName,
      hstring const &method,
      Windows::Foundation::Collections::IVectorView<IInspectable> const &arguments);

 private:
  std::shared_ptr<react::uwp::IReactInstance> m_instance{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactInstance : ReactInstanceT<ReactInstance, implementation::ReactInstance> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
