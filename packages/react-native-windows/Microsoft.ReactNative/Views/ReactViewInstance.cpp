// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactViewInstance.h"

namespace Microsoft::ReactNative {

//===========================================================================
// ReactViewInstance implementation
//===========================================================================

ReactViewInstance::ReactViewInstance(
    winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::ReactRootView> &&weakRootControl,
    Mso::DispatchQueue const &uiQueue) noexcept
    : m_weakRootControl{std::move(weakRootControl)}, m_uiQueue{uiQueue} {}

Mso::Future<void> ReactViewInstance::InitRootView(
    Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
    Mso::React::ReactViewOptions &&viewOptions) noexcept {
  return PostInUIQueue(
      [reactInstance{std::move(reactInstance)}, viewOptions{std::move(viewOptions)}](
          winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::ReactRootView> &rootControl) mutable noexcept {
        rootControl->InitRootView(std::move(reactInstance), std::move(viewOptions));
      });
}

Mso::Future<void> ReactViewInstance::UpdateRootView() noexcept {
  return PostInUIQueue(
      [](winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::ReactRootView> &rootControl) mutable noexcept {
        rootControl->UpdateRootView();
      });
}

Mso::Future<void> ReactViewInstance::UninitRootView() noexcept {
  return PostInUIQueue(
      [](winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::ReactRootView> &rootControl) mutable noexcept {
        rootControl->UninitRootView();
      });
}

} // namespace Microsoft::ReactNative
