#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ReactApplicationDelegate.g.h"
#include "ReactRootView.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactApplicationDelegate : ReactApplicationDelegateT<ReactApplicationDelegate> {
  ReactApplicationDelegate() = default;
  ReactApplicationDelegate(Windows::UI::Xaml::Application const &application) noexcept;

  void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const &args) noexcept;
  Windows::UI::Xaml::UIElement OnCreate(hstring const &args) noexcept;

 private:
  void OnResuming(IInspectable const &sender, IInspectable const &args) noexcept;
  void OnSuspending(IInspectable const &sender, IInspectable const &args) noexcept;
  void OnLeavingBackground(
      IInspectable const &sender,
      Windows::ApplicationModel::LeavingBackgroundEventArgs const &args) noexcept;
  void OnEnteredBackground(
      IInspectable const &sender,
      Windows::ApplicationModel::EnteredBackgroundEventArgs const &args) noexcept;

 private:
  Windows::UI::Xaml::Application m_application{nullptr};
  IReactApplication m_reactApplication{nullptr};
  winrt::com_ptr<ReactRootView> m_reactRootView{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactApplicationDelegate
    : ReactApplicationDelegateT<ReactApplicationDelegate, implementation::ReactApplicationDelegate> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
