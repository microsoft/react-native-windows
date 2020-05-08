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
  ReactContext(IReactContext const &context) noexcept;

  IReactContext const &ContextAbi() const noexcept;

  explicit operator bool() const noexcept;

  ReactPropertyBag Properties() const noexcept;

  template <class... TArgs>
  void CallJSFunction(std::wstring_view moduleName, std::wstring_view methodName, TArgs &&... args) const noexcept;

  void CallJSFunction(
      std::wstring_view moduleName,
      std::wstring_view methodName,
      JSValueArgWriter const &paramsArgWriter) const noexcept;

  template <class... TArgs>
  void EmitJSEvent(std::wstring_view eventEmitterName, std::wstring_view eventName, TArgs &&... args) const noexcept;

  void EmitJSEvent(
      std::wstring_view eventEmitterName,
      std::wstring_view eventName,
      JSValueArgWriter const &paramsArgWriter) const noexcept;

  template <class... TArgs>
  void DispatchEvent(xaml::FrameworkElement const &view, std::wstring_view eventName, TArgs &&... args) const noexcept;

  void DispatchEvent(
      xaml::FrameworkElement const &view,
      std::wstring_view eventName,
      JSValueArgWriter const &paramsArgWriter) const noexcept;

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

inline ReactContext::operator bool() const noexcept {
  return m_context != nullptr;
}

inline ReactPropertyBag ReactContext::Properties() const noexcept {
  return ReactPropertyBag{m_context.Properties()};
}

template <class... TArgs>
inline void ReactContext::CallJSFunction(std::wstring_view moduleName, std::wstring_view methodName, TArgs &&... args)
    const noexcept {
  m_context.CallJSFunction(moduleName, methodName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
}

inline void ReactContext::CallJSFunction(
    std::wstring_view moduleName,
    std::wstring_view methodName,
    JSValueArgWriter const &paramsArgWriter) const noexcept {
  m_context.CallJSFunction(moduleName, methodName, paramsArgWriter);
}

template <class... TArgs>
inline void ReactContext::EmitJSEvent(std::wstring_view eventEmitterName, std::wstring_view eventName, TArgs &&... args)
    const noexcept {
  m_context.EmitJSEvent(eventEmitterName, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
}

inline void ReactContext::EmitJSEvent(
    std::wstring_view eventEmitterName,
    std::wstring_view eventName,
    JSValueArgWriter const &paramsArgWriter) const noexcept {
  m_context.EmitJSEvent(eventEmitterName, eventName, paramsArgWriter);
}

template <class... TArgs>
inline void
ReactContext::DispatchEvent(xaml::FrameworkElement const &view, std::wstring_view eventName, TArgs &&... args) const
    noexcept {
  m_context.DispatchEvent(view, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
}

inline void ReactContext::DispatchEvent(
    xaml::FrameworkElement const &view,
    std::wstring_view eventName,
    JSValueArgWriter const &paramsArgWriter) const noexcept {
  m_context.DispatchEvent(view, eventName, paramsArgWriter);
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_REACTCONTEXT
