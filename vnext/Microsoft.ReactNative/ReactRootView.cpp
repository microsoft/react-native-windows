// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "ReactRootView.h"
#include "ReactRootView.g.cpp"
#include "DynamicWriter.h"
#include "ReactNativeHost.h"

namespace winrt::Microsoft::ReactNative::implementation {

ReactRootView::ReactRootView() noexcept {
  m_rootControl = std::make_shared<react::uwp::ReactRootControl>(*this);
}

ReactNative::ReactNativeHost ReactRootView::ReactNativeHost() noexcept {
  return m_reactNativeHost;
}

void ReactRootView::ReactNativeHost(ReactNative::ReactNativeHost const &value) noexcept {
  if (m_reactNativeHost != value) {
    m_rootControl->ReactViewHost(nullptr);
    m_reactNativeHost = value;
    ReloadView();
  }
}

winrt::hstring ReactRootView::ComponentName() noexcept {
  return m_componentName;
}

void ReactRootView::ComponentName(winrt::hstring const &value) noexcept {
  if (m_componentName != value) {
    m_componentName = value;
    ReloadView();
  }
}

ReactNative::JSValueArgWriter ReactRootView::InitialProps() noexcept {
  return m_initialPropsWriter;
}

void ReactRootView::InitialProps(ReactNative::JSValueArgWriter const &value) noexcept {
  if (m_initialPropsWriter != value) {
    m_initialPropsWriter = value;
    m_initialProps = DynamicWriter::ToDynamic(m_initialPropsWriter);
    ReloadView();
  }
}

void ReactRootView::ReloadView() noexcept {
  if (m_reactNativeHost && !m_componentName.empty()) {
    Mso::React::ReactViewOptions viewOptions{};
    viewOptions.ComponentName = to_string(m_componentName);
    viewOptions.InitialProps = m_initialProps;
    if (auto reactViewHost = m_rootControl->ReactViewHost()) {
      reactViewHost->ReloadViewInstanceWithOptions(std::move(viewOptions));
    } else {
      auto reactNativeHost = winrt::get_self<implementation::ReactNativeHost>(m_reactNativeHost);
      auto newReactViewHost = reactNativeHost->ReactHost()->MakeViewHost(std::move(viewOptions));
      m_rootControl->ReactViewHost(newReactViewHost.Get());
    }
  } else {
    m_rootControl->ReactViewHost(nullptr);
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation
