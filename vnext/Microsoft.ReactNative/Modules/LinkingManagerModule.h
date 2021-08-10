// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_MODULE(LinkingManager)
struct LinkingManager {
  LinkingManager() noexcept;
  ~LinkingManager() noexcept;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(openURL)
  void openURL(std::string const &url, React::ReactPromise<React::JSValue> result) noexcept;

  REACT_METHOD(canOpenURL)
  void canOpenURL(std::string const &url, React::ReactPromise<React::JSValue> result) noexcept;

  REACT_METHOD(getInitialURL)
  void getInitialURL(React::ReactPromise<React::JSValue> result) noexcept;

  REACT_METHOD(openSettings) void openSettings(React::ReactPromise<React::JSValue> result) noexcept;
  REACT_METHOD(addListener) void addListener(std::string &&eventName) noexcept;
  REACT_METHOD(removeListeners) void removeListeners(double count) noexcept;

  static void OpenUri(winrt::Windows::Foundation::Uri const &uri);

 private:
  React::ReactContext m_context;

  void HandleOpenUri(winrt::hstring const &uri) const noexcept;
};

} // namespace Microsoft::ReactNative
