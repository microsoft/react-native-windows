// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_REACTCONTEXT
#define MICROSOFT_REACTNATIVE_REACTCONTEXT

#include <CppWinRTIncludes.h>
#include <string_view>
#include "JSValueWriter.h"
#include "ReactPropertyBag.h"

namespace winrt::Microsoft::ReactNative {

// Represents a context of execution for the Native Module.
// It wraps up the IReactContext and adds convenience methods for
// working with C++ types.
struct ReactContext {
  ReactContext(IReactContext const &handle) noexcept : m_handle{handle} {}

  IReactContext const &Handle() const noexcept {
    return m_handle;
  }

  explicit operator bool() const noexcept {
    return static_cast<bool>(m_handle);
  }

  ReactPropertyBag Properties() const noexcept {
    return ReactPropertyBag{m_handle.Properties()};
  }

  template <class... TArgs>
  void CallJSFunction(std::wstring_view moduleName, std::wstring_view methodName, TArgs &&... args) const noexcept {
    m_handle.CallJSFunction(moduleName, methodName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
  }

  void CallJSFunction(
      std::wstring_view moduleName,
      std::wstring_view methodName,
      JSValueArgWriter const &paramsArgWriter) const noexcept {
    m_handle.CallJSFunction(moduleName, methodName, paramsArgWriter);
  }

  template <class... TArgs>
  void EmitJSEvent(std::wstring_view eventEmitterName, std::wstring_view eventName, TArgs &&... args) const noexcept {
    m_handle.EmitJSEvent(eventEmitterName, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
  }

  void EmitJSEvent(
      std::wstring_view eventEmitterName,
      std::wstring_view eventName,
      JSValueArgWriter const &paramsArgWriter) const noexcept {
    m_handle.EmitJSEvent(eventEmitterName, eventName, paramsArgWriter);
  }

  template <class... TArgs>
  void DispatchEvent(xaml::FrameworkElement const &view, std::wstring_view eventName, TArgs &&... args) const noexcept {
    m_handle.DispatchEvent(view, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
  }

  void DispatchEvent(
      xaml::FrameworkElement const &view,
      std::wstring_view eventName,
      JSValueArgWriter const &paramsArgWriter) const noexcept {
    m_handle.DispatchEvent(view, eventName, paramsArgWriter);
  }

 private:
  const IReactContext m_handle;
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_REACTCONTEXT
