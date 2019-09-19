// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "FrameworkElementViewManager.h"
#if __has_include("Bridge.FrameworkElementViewManager.g.cpp")
#include "Bridge.FrameworkElementViewManager.g.cpp"
#endif

namespace winrt {
using namespace Windows::UI::Xaml;
} // namespace winrt

namespace winrt::Microsoft::ReactNative::Bridge::implementation {
hstring FrameworkElementViewManager::GetNameCore() {
  throw hresult_not_implemented();
}

Windows::UI::Xaml::FrameworkElement
FrameworkElementViewManager::CreateViewCore() {
  throw hresult_not_implemented();
}

void FrameworkElementViewManager::UpdatePropertiesCore(
    Windows::UI::Xaml::FrameworkElement const &view,
    Windows::Foundation::Collections::IMapView<
        hstring,
        Windows::Foundation::IInspectable> const &propertyMap) {
  throw hresult_not_implemented();
}

// IViewManager
hstring FrameworkElementViewManager::GetName() {
  return GetNameCore();
}

Windows::UI::Xaml::DependencyObject FrameworkElementViewManager::CreateView(
    int64_t tag) {
  auto view = CreateViewCore();
  view.SetValue(winrt::FrameworkElement::TagProperty(), box_value(tag));
  return view;
}

void FrameworkElementViewManager::UpdateProperties(
    Windows::UI::Xaml::DependencyObject const &view,
    Windows::Foundation::Collections::IMapView<
        hstring,
        Windows::Foundation::IInspectable> const &propertyMap) {
  UpdatePropertiesCore(
      view.as<Windows::UI::Xaml::FrameworkElement>(), propertyMap);
  // TODO: Call "real" parent's FrameworkElementViewManager::updateProperties
}
} // namespace winrt::Microsoft::ReactNative::Bridge::implementation
