// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::SampleLibraryCPP::implementation {

struct CustomUserControlViewManagerCPP
    : winrt::implements<
          CustomUserControlViewManagerCPP,
          winrt::Microsoft::ReactNative::Bridge::IViewManager,
          winrt::Microsoft::ReactNative::Bridge::IViewManagerWithNativeProperties,
          winrt::Microsoft::ReactNative::Bridge::IViewManagerWithCommands,
          winrt::Microsoft::ReactNative::Bridge::IViewManagerWithExportedEventTypeConstants> {
 public:
  CustomUserControlViewManagerCPP(winrt::Microsoft::ReactNative::Bridge::IReactContext const &reactContext);

  // IViewManager
  winrt::hstring Name() noexcept;

  winrt::Microsoft::ReactNative::Bridge::IReactContext ReactContext() noexcept;

  winrt::Windows::UI::Xaml::FrameworkElement CreateView() noexcept;

  // IViewManagerWithNativeProperties
  winrt::Windows::Foundation::Collections::
      IMapView<winrt::hstring, winrt::Microsoft::ReactNative::Bridge::ViewManagerPropertyType>
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
  winrt::Microsoft::ReactNative::Bridge::IReactContext m_reactContext{nullptr};
};

} // namespace winrt::SampleLibraryCPP::implementation
