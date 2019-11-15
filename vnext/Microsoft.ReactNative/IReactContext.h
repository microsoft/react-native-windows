// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactUWP/IReactInstance.h>

#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

struct ReactContext : winrt::implements<ReactContext, IReactContext> {
  ReactContext(std::shared_ptr<react::uwp::IReactInstance> instance) noexcept : m_instance(instance) {}

 public: // IReactContext
  void DispatchEvent(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      hstring const &eventName,
      IInspectable const &eventData) noexcept;
  void CallJsFunction(hstring const &moduleName, hstring const &method, IInspectable const &params) noexcept;

 private:
  std::shared_ptr<react::uwp::IReactInstance> m_instance{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Bridge
