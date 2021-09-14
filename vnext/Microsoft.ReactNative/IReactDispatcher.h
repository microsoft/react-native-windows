// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "ReactDispatcherHelper.g.h"
#include <dispatchQueue/dispatchQueue.h>
#include <winrt/Microsoft.ReactNative.h>

namespace Mso::React {

MSO_GUID(IDispatchQueue2, "a8d9db25-3d16-4476-ae65-682fcee1260c")
struct IDispatchQueue2 : ::IUnknown {
  //! Post the task to the end of the queue for asynchronous invocation.
  virtual void Post(Mso::DispatchTask &&task) const noexcept = 0;

  //! Invoke the task immediately if the queue uses the current thread. Otherwise, post it.
  //! The immediate execution ignores the suspend or shutdown states.
  virtual void InvokeElsePost(Mso::DispatchTask &&task) const noexcept = 0;
};

} // namespace Mso::React

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactDispatcher : implements<ReactDispatcher, IReactDispatcher, Mso::React::IDispatchQueue2> {
  ReactDispatcher() = default;
  ReactDispatcher(Mso::DispatchQueue &&queue) noexcept;

  bool HasThreadAccess() noexcept;
  void Post(ReactDispatcherCallback const &callback) noexcept;

  static IReactDispatcher CreateSerialDispatcher() noexcept;

  static Mso::CntPtr<IDispatchQueue2> GetUIDispatchQueue2(IReactPropertyBag const &properties) noexcept;
  static IReactDispatcher UIThreadDispatcher() noexcept;
  static IReactPropertyName UIDispatcherProperty() noexcept;
  static IReactDispatcher GetUIDispatcher(IReactPropertyBag const &properties) noexcept;
  static void SetUIThreadDispatcher(IReactPropertyBag const &properties) noexcept;

  static IReactPropertyName JSDispatcherProperty() noexcept;

  void Post(Mso::DispatchTask &&task) const noexcept override;
  void InvokeElsePost(Mso::DispatchTask &&task) const noexcept override;

 private:
  Mso::DispatchQueue m_queue;
};

struct ReactDispatcherHelper {
  ReactDispatcherHelper() = default;

  static IReactDispatcher CreateSerialDispatcher() noexcept {
    return ReactDispatcher::CreateSerialDispatcher();
  }

  static IReactDispatcher UIThreadDispatcher() noexcept {
    return ReactDispatcher::UIThreadDispatcher();
  }

  static IReactPropertyName UIDispatcherProperty() noexcept {
    return ReactDispatcher::UIDispatcherProperty();
  }

  static IReactPropertyName JSDispatcherProperty() noexcept {
    return ReactDispatcher::JSDispatcherProperty();
  }
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactDispatcherHelper : ReactDispatcherHelperT<ReactDispatcherHelper, implementation::ReactDispatcherHelper> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
