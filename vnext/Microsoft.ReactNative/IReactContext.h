// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactHost/React.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactContext : winrt::implements<ReactContext, IReactContext> {
  ReactContext(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;

 public: // IReactContext
  IReactPropertyBag Properties() noexcept;
  IReactDispatcher UIDispatcher() noexcept;
  void DispatchEvent(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      hstring const &eventName,
      JSValueArgWriter const &eventDataArgWriter) noexcept;
  void CallJSFunction(
      hstring const &moduleName,
      hstring const &methodName,
      JSValueArgWriter const &paramsArgWriter) noexcept;
  void EmitJSEvent(
      hstring const &eventEmitterName,
      hstring const &eventName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

 private:
  Mso::CntPtr<Mso::React::IReactContext> m_context;
};

} // namespace winrt::Microsoft::ReactNative::implementation
