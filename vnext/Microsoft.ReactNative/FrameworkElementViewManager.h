// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Bridge.FrameworkElementViewManager.g.h"

namespace winrt::Microsoft::ReactNative::Bridge::implementation {
struct FrameworkElementViewManager
    : FrameworkElementViewManagerT<FrameworkElementViewManager> {
  FrameworkElementViewManager() = default;

  hstring GetNameCore();
  Windows::UI::Xaml::FrameworkElement CreateViewCore();
  void UpdatePropertiesCore(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      Windows::Foundation::Collections::IMapView<
          hstring,
          Windows::Foundation::IInspectable> const &propertyMap);

  // IViewManager
  hstring GetName();
  Windows::UI::Xaml::DependencyObject CreateView(int64_t tag);
  void UpdateProperties(
      Windows::UI::Xaml::DependencyObject const &view,
      Windows::Foundation::Collections::IMapView<
          hstring,
          Windows::Foundation::IInspectable> const &propertyMap);
};
} // namespace winrt::Microsoft::ReactNative::implementation
namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation {
struct FrameworkElementViewManager
    : FrameworkElementViewManagerT<
          FrameworkElementViewManager,
          implementation::FrameworkElementViewManager> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
