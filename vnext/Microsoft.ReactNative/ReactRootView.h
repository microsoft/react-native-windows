// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "ReactRootView.g.h"

#include "ReactHost/React.h"
#include "Views/ReactRootControl.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactRootView : ReactRootViewT<ReactRootView> {
  ReactRootView() noexcept;

  // property ReactNativeHost
  ReactNative::ReactNativeHost ReactNativeHost() noexcept;
  void ReactNativeHost(ReactNative::ReactNativeHost const &value) noexcept;

  // property ComponentName
  hstring ComponentName() noexcept;
  void ComponentName(hstring const &value) noexcept;

  // property InitialProps
  ReactNative::JSValueArgWriter InitialProps() noexcept;
  void InitialProps(ReactNative::JSValueArgWriter const &value) noexcept;

  // property IsPerspectiveEnabled
  bool IsPerspectiveEnabled() const noexcept {
    return m_isPerspectiveEnabled;
  }
  void IsPerspectiveEnabled(bool value) noexcept {
    m_isPerspectiveEnabled = value;
    UpdatePerspective();
  }

  void ReloadView() noexcept;

 private:
  void Attach() noexcept;
  void Detach() noexcept;

 private:
  ReactNative::ReactNativeHost m_reactNativeHost{nullptr};
  hstring m_componentName;
  ReactNative::JSValueArgWriter m_initialPropsWriter;
  folly::dynamic m_initialProps;
  bool m_isPerspectiveEnabled{true};
  std::shared_ptr<react::uwp::ReactRootControl> m_rootControl;

  void UpdatePerspective();
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactRootView : ReactRootViewT<ReactRootView, implementation::ReactRootView> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
