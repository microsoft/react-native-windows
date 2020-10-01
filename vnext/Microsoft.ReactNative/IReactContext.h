// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ReactHost/React.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactContext : winrt::implements<ReactContext, IReactContext> {
  ReactContext(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;

 public: // IReactContext
  IReactPropertyBag Properties() noexcept;
  IReactNotificationService Notifications() noexcept;
  IReactDispatcher UIDispatcher() noexcept;
  IReactDispatcher JSDispatcher() noexcept;
#ifndef CORE_ABI
  void DispatchEvent(
      xaml::FrameworkElement const &view,
      hstring const &eventName,
      JSValueArgWriter const &eventDataArgWriter) noexcept;
#endif
  void CallJSFunction(
      hstring const &moduleName,
      hstring const &methodName,
      JSValueArgWriter const &paramsArgWriter) noexcept;
  void EmitJSEvent(
      hstring const &eventEmitterName,
      hstring const &eventName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

  bool UseWebDebugger() const noexcept;
  bool UseFastRefresh() const noexcept;
  bool UseDirectDebugger() const noexcept;
  bool DebuggerBreakOnNextLine() const noexcept;
  uint16_t DebuggerPort() const noexcept;
  hstring DebugBundlePath() const noexcept;
  hstring BundleRootPath() const noexcept;
  hstring SourceBundleHost() const noexcept;
  uint16_t SourceBundlePort() const noexcept;
  hstring JavaScriptBundleFile() const noexcept;

  // Not part of the public ABI interface
  // Internal accessor for within the Microsoft.ReactNative dll to allow calling into internal methods
  Mso::React::IReactContext &GetInner() const noexcept;

 private:
  Mso::CntPtr<Mso::React::IReactContext> m_context;
};

} // namespace winrt::Microsoft::ReactNative::implementation
