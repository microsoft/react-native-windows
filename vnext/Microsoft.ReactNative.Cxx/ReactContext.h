// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_REACTCONTEXT
#define MICROSOFT_REACTNATIVE_REACTCONTEXT

#include <string_view>
#include "JSValueWriter.h"
#include "winrt/Microsoft.ReactNative.h"
#include <../include/CppWinRTIncludes.h>

namespace winrt::Microsoft::ReactNative {

// Represents a context of execution for the Native Module.
// It wraps up the IReactContext and adds convenience methods for
// working with C++ types.
struct ReactContext {
  ReactContext(IReactContext const &context) noexcept;

  IReactContext const &ContextAbi() const noexcept;

  explicit operator bool() noexcept;

  template <class... TArgs>
  void CallJSFunction(std::wstring_view moduleName, std::wstring_view methodName, TArgs &&... args) noexcept;

  void CallJSFunction(
      std::wstring_view moduleName,
      std::wstring_view methodName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

  template <class... TArgs>
  void EmitJSEvent(std::wstring_view eventEmitterName, std::wstring_view eventName, TArgs &&... args) noexcept;

  void EmitJSEvent(
      std::wstring_view eventEmitterName,
      std::wstring_view eventName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

  template <class... TArgs>
  void DispatchEvent(
      xaml::FrameworkElement const &view,
      std::wstring_view eventName,
      TArgs &&... args) noexcept;

  void DispatchEvent(
      xaml::FrameworkElement const &view,
      std::wstring_view eventName,
      JSValueArgWriter const &paramsArgWriter) noexcept;

 private:
  const IReactContext m_context;
};

//==============================================================================
// ReactContext inline implementation
//==============================================================================

inline ReactContext::ReactContext(IReactContext const &context) noexcept : m_context{context} {}

inline IReactContext const &ReactContext::ContextAbi() const noexcept {
  return m_context;
}

inline ReactContext::operator bool() noexcept {
  return m_context != nullptr;
}

template <class... TArgs>
inline void
ReactContext::CallJSFunction(std::wstring_view moduleName, std::wstring_view methodName, TArgs &&... args) noexcept {
  m_context.CallJSFunction(moduleName, methodName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
}

inline void ReactContext::CallJSFunction(
    std::wstring_view moduleName,
    std::wstring_view methodName,
    JSValueArgWriter const &paramsArgWriter) noexcept {
  m_context.CallJSFunction(moduleName, methodName, paramsArgWriter);
}

template <class... TArgs>
inline void
ReactContext::EmitJSEvent(std::wstring_view eventEmitterName, std::wstring_view eventName, TArgs &&... args) noexcept {
  m_context.EmitJSEvent(eventEmitterName, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
}

inline void ReactContext::EmitJSEvent(
    std::wstring_view eventEmitterName,
    std::wstring_view eventName,
    JSValueArgWriter const &paramsArgWriter) noexcept {
  m_context.EmitJSEvent(eventEmitterName, eventName, paramsArgWriter);
}

template <class... TArgs>
inline void ReactContext::DispatchEvent(
    xaml::FrameworkElement const &view,
    std::wstring_view eventName,
    TArgs &&... args) noexcept {
  m_context.DispatchEvent(view, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
}

inline void ReactContext::DispatchEvent(
    xaml::FrameworkElement const &view,
    std::wstring_view eventName,
    JSValueArgWriter const &paramsArgWriter) noexcept {
  m_context.DispatchEvent(view, eventName, paramsArgWriter);
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_REACTCONTEXT
