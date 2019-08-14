#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ReactApplicationDelegate.g.h"
#include "ReactApplication.h"

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactApplicationDelegate
    : ReactApplicationDelegateT<ReactApplicationDelegate> {
  ReactApplicationDelegate() = default;
  ReactApplicationDelegate(Application const &application);

  void OnActivated(
      winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const
          &args);
  UIElement OnCreate(hstring const &args);

 private:
  Application m_application{nullptr};
  winrt::Microsoft::ReactNative::IReactApplication m_reactApplication{nullptr};

  void OnResuming(IInspectable sender, IInspectable args);
  void OnSuspending(IInspectable sender, IInspectable args);
  void OnLeavingBackground(
      IInspectable sender,
      winrt::Windows::ApplicationModel::LeavingBackgroundEventArgs args);
  void OnEnteredBackground(
      IInspectable sender,
      winrt::Windows::ApplicationModel::EnteredBackgroundEventArgs args);
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactApplicationDelegate
    : ReactApplicationDelegateT<
          ReactApplicationDelegate,
          implementation::ReactApplicationDelegate> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
