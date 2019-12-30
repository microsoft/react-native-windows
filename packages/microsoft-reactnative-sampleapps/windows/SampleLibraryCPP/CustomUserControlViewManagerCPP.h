// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::SampleLibraryCpp::implementation {

struct CustomUserControlViewManagerCpp
    : winrt::implements<
          CustomUserControlViewManagerCpp,
          winrt::Microsoft::ReactNative::IViewManager,
          winrt::Microsoft::ReactNative::IViewManagerWithNativeProperties,
          winrt::Microsoft::ReactNative::IViewManagerWithCommands,
          winrt::Microsoft::ReactNative::IViewManagerWithExportedEventTypeConstants> {
 public:
  CustomUserControlViewManagerCpp(winrt::Microsoft::ReactNative::IReactContext const &reactContext);

  // IViewManager
  winrt::hstring Name() noexcept;

  winrt::Microsoft::ReactNative::IReactContext ReactContext() noexcept;

  winrt::Windows::UI::Xaml::FrameworkElement CreateView() noexcept;

  // IViewManagerWithNativeProperties
  winrt::Windows::Foundation::Collections::
      IMapView<winrt::hstring, winrt::Microsoft::ReactNative::ViewManagerPropertyType>
      NativeProps() noexcept;

  void UpdateProperties(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, winrt::Windows::Foundation::IInspectable> const
          &propertyMap);

  // IViewManagerWithCommands
  winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, int64_t> Commands() noexcept;

  void DispatchCommand(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      int64_t commandId,
      winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Foundation::IInspectable>
          commandArgs) noexcept;

  // IViewManagerWithExportedEventTypeConstants
  winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, winrt::Windows::Foundation::IInspectable>
  ExportedCustomBubblingEventTypeConstants() noexcept;

  winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, winrt::Windows::Foundation::IInspectable>
  ExportedCustomDirectEventTypeConstants() noexcept;

 private:
  winrt::Microsoft::ReactNative::IReactContext m_reactContext{nullptr};
};

} // namespace winrt::SampleLibraryCpp::implementation
