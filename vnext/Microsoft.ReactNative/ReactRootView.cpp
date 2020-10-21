// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "ReactRootView.h"
#include "ReactRootView.g.cpp"
#include <UI.Xaml.Media.Media3D.h>
#include "DynamicWriter.h"
#include "ReactNativeHost.h"

namespace winrt::Microsoft::ReactNative::implementation {

ReactRootView::ReactRootView() noexcept {
  m_rootControl = std::make_shared<react::uwp::ReactRootControl>(*this);
  UpdatePerspective();
  Loaded([this](auto &&, auto &&) {
    ::Microsoft::ReactNative::SetCompositor(::Microsoft::ReactNative::GetCompositor(*this));
  });
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

void ReactRootView::UpdatePerspective() {
  // Xaml's default projection in 3D is orthographic (all lines are parallel)
  // However React Native's default projection is a one-point perspective.
  // Set a default perspective projection on the main control to mimic this.
  auto grid = m_rootControl->GetXamlView().as<xaml::Controls::Grid>();

  if (m_isPerspectiveEnabled) {
    auto perspectiveTransform3D = xaml::Media::Media3D::PerspectiveTransform3D();
    perspectiveTransform3D.Depth(850);
    xaml::Media::Media3D::Transform3D t3d(perspectiveTransform3D);
    grid.Transform3D(t3d);
  } else {
    grid.ClearValue(xaml::UIElement::Transform3DProperty());
  }
}
} // namespace winrt::Microsoft::ReactNative::implementation
