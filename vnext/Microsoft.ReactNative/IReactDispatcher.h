// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ReactDispatcherHelper.g.h"
#include <ReactPropertyBag.h>
#include <dispatchQueue/dispatchQueue.h>
#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative {

struct ReactDispatcher : implements<ReactDispatcher, IReactDispatcher> {
  ReactDispatcher() = default;
  ReactDispatcher(Mso::DispatchQueue &&queue) noexcept;

  bool HasThreadAccess() noexcept;
  void Post(ReactDispatcherCallback const &callback) noexcept;

  static Mso::DispatchQueue GetUIDispatchQueue(IReactPropertyBag const &properties) noexcept;

  static IReactDispatcher UIThreadDispatcher() noexcept;
  static ReactPropertyId<IReactDispatcher> UIDispatcherProperty() noexcept;
  static IReactDispatcher GetUIDispatcher(IReactPropertyBag const &properties) noexcept;
  static void SetUIThreadDispatcher(IReactPropertyBag const &properties) noexcept;

 private:
  Mso::DispatchQueue m_queue;
};

} // namespace winrt::Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactDispatcherHelper {
  ReactDispatcherHelper() = default;

  static IReactDispatcher UIThreadDispatcher() noexcept {
    return ReactDispatcher::UIThreadDispatcher();
  }

  static IReactPropertyName UIDispatcherProperty() noexcept {
    return ReactDispatcher::UIDispatcherProperty().Handle();
  }
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactDispatcherHelper : ReactDispatcherHelperT<ReactDispatcherHelper, implementation::ReactDispatcherHelper> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
