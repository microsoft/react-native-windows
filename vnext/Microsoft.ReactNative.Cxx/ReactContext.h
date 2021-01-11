// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
// IMPORTANT: Before updating this file
// please read react-native-windows repo:
// vnext/Microsoft.ReactNative.Cxx/README.md

#pragma once
#ifndef MICROSOFT_REACTNATIVE_REACTCONTEXT
#define MICROSOFT_REACTNATIVE_REACTCONTEXT

#if !defined(CORE_ABI) && !defined(__APPLE__)
#include <CppWinRTIncludes.h>
#endif
#include <string_view>
#include "JSValueWriter.h"
<<<<<<< HEAD
#include "ReactDispatcher.h"
#include "ReactPropertyBag.h"
||||||| 811c767bf
#include "winrt/Microsoft.ReactNative.h"
=======
#include "ReactNotificationService.h"
#include "ReactPropertyBag.h"
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa

namespace winrt::Microsoft::ReactNative {

// Represents a context of execution for the Native Module.
// It wraps up the IReactContext and adds convenience methods for
// working with C++ types.
struct ReactContext {
  ReactContext(std::nullptr_t = nullptr) noexcept {}

  ReactContext(IReactContext const &handle) noexcept : m_handle{handle} {}

<<<<<<< HEAD
  IReactContext const &Handle() const noexcept {
    return m_handle;
  }

  explicit operator bool() const noexcept {
    return m_handle ? true : false;
  }
||||||| 811c767bf
  template <class... TArgs>
  void CallJSFunction(std::wstring_view moduleName, std::wstring_view methodName, TArgs &&... args) noexcept;
=======
  IReactContext const &Handle() const noexcept {
    return m_handle;
  }

  explicit operator bool() const noexcept {
    return m_handle ? true : false;
  }

  ReactPropertyBag Properties() const noexcept {
    return ReactPropertyBag{m_handle.Properties()};
  }
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa

<<<<<<< HEAD
  ReactPropertyBag Properties() const noexcept {
    return ReactPropertyBag{m_handle.Properties()};
  }
||||||| 811c767bf
  void CallJSFunction(
      std::wstring_view moduleName,
      std::wstring_view methodName,
      JSValueArgWriter const &paramsArgWriter) noexcept;
=======
  ReactNotificationService Notifications() const noexcept {
    return ReactNotificationService{m_handle.Notifications()};
  }
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa

<<<<<<< HEAD
  ReactDispatcher UIDispatcher() const noexcept {
    return ReactDispatcher{m_handle.UIDispatcher()};
  }

  // Call methodName JS function of module with moduleName.
  // args are either function arguments or a single lambda with 'IJSValueWriter const&' argument.
||||||| 811c767bf
=======
  ReactDispatcher UIDispatcher() const noexcept {
    return ReactDispatcher{m_handle.UIDispatcher()};
  }

  ReactDispatcher JSDispatcher() const noexcept {
    return ReactDispatcher{m_handle.JSDispatcher()};
  }

  // Call methodName JS function of module with moduleName.
  // args are either function arguments or a single lambda with 'IJSValueWriter const&' argument.
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa
  template <class... TArgs>
  void CallJSFunction(std::wstring_view moduleName, std::wstring_view methodName, TArgs &&... args) const noexcept {
    m_handle.CallJSFunction(moduleName, methodName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
  }

<<<<<<< HEAD
  // Simplifies calls to CallJSFunction to emit events (method named 'emit').
  // Call eventName JS event of module with eventEmitterName.
  // args are either function arguments or a single lambda with 'IJSValueWriter const&' argument.
  template <class... TArgs>
  void EmitJSEvent(std::wstring_view eventEmitterName, std::wstring_view eventName, TArgs &&... args) const noexcept {
    m_handle.EmitJSEvent(eventEmitterName, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
  }
||||||| 811c767bf
  void EmitJSEvent(
      std::wstring_view eventEmitterName,
      std::wstring_view eventName,
      JSValueArgWriter const &paramsArgWriter) noexcept;
=======
  // Simplifies calls to CallJSFunction to emit events (method named 'emit').
  // Call eventName JS event of module with eventEmitterName.
  // args are either function arguments or a single lambda with 'IJSValueWriter const&' argument.
  template <class... TArgs>
  void EmitJSEvent(std::wstring_view eventEmitterName, std::wstring_view eventName, TArgs &&... args) const noexcept {
    m_handle.EmitJSEvent(eventEmitterName, eventName, MakeJSValueWriter(std::forward<TArgs>(args)...));
  }
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa

<<<<<<< HEAD
  // Dispatch eventName event to the view.
  // args are either function arguments or a single lambda with 'IJSValueWriter const&' argument.
||||||| 811c767bf
=======
#if !defined(CORE_ABI) && !defined(__APPLE__)
  // Dispatch eventName event to the view.
  // args are either function arguments or a single lambda with 'IJSValueWriter const&' argument.
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa
  template <class... TArgs>
<<<<<<< HEAD
  void DispatchEvent(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      std::wstring_view eventName,
      TArgs &&... args) const noexcept {
    m_handle.DispatchEvent(view, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
  }
||||||| 811c767bf
  void DispatchEvent(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      std::wstring_view eventName,
      TArgs &&... args) noexcept;
=======
  void DispatchEvent(xaml::FrameworkElement const &view, std::wstring_view eventName, TArgs &&... args) const noexcept {
    m_handle.DispatchEvent(view, eventName, MakeJSValueArgWriter(std::forward<TArgs>(args)...));
  }
#endif
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa

  friend bool operator==(ReactContext const &left, ReactContext const &right) noexcept {
    return left.m_handle == right.m_handle;
  }

  friend bool operator!=(ReactContext const &left, ReactContext const &right) noexcept {
    return left.m_handle != right.m_handle;
  }

  friend bool operator==(ReactContext const &left, std::nullptr_t) noexcept {
    return !static_cast<bool>(left.m_handle);
  }

  friend bool operator!=(ReactContext const &left, std::nullptr_t) noexcept {
    return static_cast<bool>(left.m_handle);
  }

  friend bool operator==(std::nullptr_t, ReactContext const &right) noexcept {
    return !static_cast<bool>(right.m_handle);
  }

  friend bool operator!=(std::nullptr_t, ReactContext const &right) noexcept {
    return static_cast<bool>(right.m_handle);
  }

 private:
  IReactContext m_handle;
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_REACTCONTEXT
