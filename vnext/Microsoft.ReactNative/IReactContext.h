// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactUWP/IReactInstance.h>
#include "DynamicWriter.h"

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

struct ReactContext : winrt::implements<ReactContext, IReactContext> {
  ReactContext(std::weak_ptr<react::uwp::IReactInstance> instance) noexcept : m_instance(instance) {}

 public: // IReactContext
  void DispatchEvent(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      hstring const &eventName,
      ReactArgWriter const &eventDataArgWriter) noexcept;
  void CallJsFunction(hstring const &moduleName, hstring const &method, ReactArgWriter const &paramsArgWriter) noexcept;

 private:
  std::weak_ptr<react::uwp::IReactInstance> m_instance;
};

} // namespace winrt::Microsoft::ReactNative
