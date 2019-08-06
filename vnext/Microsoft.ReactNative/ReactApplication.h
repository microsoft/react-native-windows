#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ReactApplication.g.h"
#include "ReactNativeHost.h"

#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.UI.Core.h>

using namespace winrt;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml::Navigation;

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactApplication : ReactApplicationT<ReactApplication> {
  ReactApplication();

  Microsoft::ReactNative::ReactNativeHost Host();

  virtual Microsoft::ReactNative::ReactNativeHost HostCore();

  virtual void OnLaunched(Activation::LaunchActivatedEventArgs const &);

  virtual void OnSuspending(IInspectable const &, SuspendingEventArgs const &);

  virtual void OnNavigationFailed(
      IInspectable const &,
      NavigationFailedEventArgs const &);

 protected:
  virtual winrt::Microsoft::ReactNative::
      ReactApplicationDelegate __stdcall CreateReactApplicationDelegate() {
    return winrt::Microsoft::ReactNative::ReactApplicationDelegate(*this);
  }

 private:
  Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  Microsoft::ReactNative::ReactApplicationDelegate m_delegate{nullptr};

  void OnCreate(Activation::LaunchActivatedEventArgs const &e);
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactApplication
    : ReactApplicationT<ReactApplication, implementation::ReactApplication> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
