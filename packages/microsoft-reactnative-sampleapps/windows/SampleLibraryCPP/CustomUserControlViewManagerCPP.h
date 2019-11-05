// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::SampleLibraryCPP::implementation {

struct CustomUserControlViewManagerCPP : winrt::implements<
                                             CustomUserControlViewManagerCPP,
                                             winrt::Microsoft::ReactNative::Bridge::IViewManager,
                                             winrt::Microsoft::ReactNative::Bridge::IViewManagerWithNativeProperties> {
 public:
  CustomUserControlViewManagerCPP() = default;

  // IViewManager
  winrt::hstring Name() noexcept;

  winrt::Windows::UI::Xaml::FrameworkElement CreateView() noexcept;

  // IViewManagerWithNativeProperties
  winrt::Windows::Foundation::Collections::
      IMapView<winrt::hstring, winrt::Microsoft::ReactNative::Bridge::ViewManagerPropertyType>
      NativeProps() noexcept;

  void UpdateProperties(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, winrt::Windows::Foundation::IInspectable> const
          &propertyMap);
};

} // namespace winrt::SampleLibraryCPP::implementation
