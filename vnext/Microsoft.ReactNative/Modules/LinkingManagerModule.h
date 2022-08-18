// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "codegen/NativeLinkingManagerSpec.g.h"
#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Microsoft::ReactNative {

REACT_MODULE(LinkingManager)
struct LinkingManager {
  LinkingManager() noexcept;
  ~LinkingManager() noexcept;

  using ModuleSpec = ReactNativeSpecs::LinkingManagerSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(canOpenURL)
  static winrt::fire_and_forget canOpenURL(std::string url, ::React::ReactPromise<::React::JSValue> result) noexcept;

  REACT_METHOD(openURL)
  void openURL(std::string &&url, ::React::ReactPromise<::React::JSValue> &&result) noexcept;

  REACT_METHOD(openSettings)
  static void openSettings(::React::ReactPromise<::React::JSValue> &&result) noexcept;

  REACT_METHOD(addListener)
  static void addListener(std::string eventName) noexcept;

  REACT_METHOD(removeListeners)
  static void removeListeners(double count) noexcept;

  REACT_METHOD(getInitialURL)
  static void getInitialURL(::React::ReactPromise<::React::JSValue> &&result) noexcept;

  static void OpenUri(winrt::Windows::Foundation::Uri const &uri) noexcept;

 private:
  void HandleOpenUri(winrt::hstring const &uri) noexcept;

  static std::mutex s_mutex;
  React::ReactContext m_context;
  static winrt::Windows::Foundation::Uri s_initialUri;
  static std::vector<LinkingManager *> s_linkingModules;
};

} // namespace Microsoft::ReactNative
