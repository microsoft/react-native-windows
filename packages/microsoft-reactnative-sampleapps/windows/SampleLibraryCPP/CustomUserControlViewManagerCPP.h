// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.ReactNative.h"

namespace winrt::SampleLibraryCpp::implementation {

struct CustomUserControlViewManagerCpp
    : winrt::implements<
          CustomUserControlViewManagerCpp,
          winrt::Microsoft::ReactNative::IViewManager,
          winrt::Microsoft::ReactNative::IViewManagerWithReactContext,
          winrt::Microsoft::ReactNative::IViewManagerWithNativeProperties,
          winrt::Microsoft::ReactNative::IViewManagerWithCommands,
          winrt::Microsoft::ReactNative::IViewManagerWithExportedEventTypeConstants> {
 public:
  CustomUserControlViewManagerCpp();

  // IViewManager
  winrt::hstring Name() noexcept;

  winrt::Windows::UI::Xaml::FrameworkElement CreateView() noexcept;

  // IViewManagerWithReactContext
  winrt::Microsoft::ReactNative::IReactContext ReactContext() noexcept;

  void ReactContext(winrt::Microsoft::ReactNative::IReactContext reactContext) noexcept;

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
      winrt::Microsoft::ReactNative::IJSValueReader const &commandArgsReader) noexcept;

  // IViewManagerWithExportedEventTypeConstants
  winrt::Microsoft::ReactNative::ConstantProvider ExportedCustomBubblingEventTypeConstants() noexcept;

  winrt::Microsoft::ReactNative::ConstantProvider ExportedCustomDirectEventTypeConstants() noexcept;

 private:
  winrt::Microsoft::ReactNative::IReactContext m_reactContext{nullptr};
};

} // namespace winrt::SampleLibraryCpp::implementation
