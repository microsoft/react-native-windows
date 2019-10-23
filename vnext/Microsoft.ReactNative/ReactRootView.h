#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ReactRootView.g.h"

#include "ReactInstanceManager.h"
#include "ReactNativeHost.h"
#include "ViewManagerProvider.h"

#include <ReactUWP/IReactInstance.h>
#include <ReactUWP/IXamlRootView.h>
#include <ReactUWP/ReactUwp.h>

using namespace winrt;
using namespace Windows::UI::Core;

namespace winrt::Microsoft::ReactNative::implementation {

static bool s_isShiftKeyDown;
static bool s_isControlKeyDown;

struct ReactRootView : ReactRootViewT<ReactRootView> {
  ReactRootView() = default;

  void OnCreate(ReactNative::ReactNativeHost const &host);
  fire_and_forget StartReactApplicationAsync(
      ReactNative::ReactInstanceManager const &instanceManager,
      hstring componentName,
      folly::dynamic initialProps);

 private:
  std::shared_ptr<react::uwp::IXamlRootView> m_xamlView;
  hstring m_moduleName{};
  folly::dynamic m_initialProps{};
  ReactNative::ReactInstanceManager m_reactInstanceManager{nullptr};

  static void OnBackRequested(
      ReactNative::ReactNativeHost const &host,
      IInspectable const &sender,
      BackRequestedEventArgs const &e);

  static void OnAcceleratorKeyActivated(
      ReactNative::ReactNativeHost const &host,
      CoreDispatcher const &sender,
      AcceleratorKeyEventArgs const &e);

  static bool IsKeyDown(CoreAcceleratorKeyEventType t);
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactRootView
    : ReactRootViewT<ReactRootView, implementation::ReactRootView> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
