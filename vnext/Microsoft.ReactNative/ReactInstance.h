// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactInstance.g.h"

#ifndef CORE_ABI
// When building Desktop, the include below results in
// fatal error C1083: Cannot open include file: 'CppWinRTIncludes.h': No such file or directory
#include <ReactUWP/IReactInstance.h>
#endif

#include "HResult.h"

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
