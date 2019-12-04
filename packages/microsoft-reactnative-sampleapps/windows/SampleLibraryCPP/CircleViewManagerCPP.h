// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::SampleLibraryCPP::implementation {

struct CircleViewManagerCPP : winrt::implements<
                                  CircleViewManagerCPP,
                                  winrt::Microsoft::ReactNative::Bridge::IViewManager,
                                  winrt::Microsoft::ReactNative::Bridge::IViewManagerWithChildren> {
 public:
  CircleViewManagerCPP();

  // IViewManager
  winrt::hstring Name() noexcept;

  winrt::Windows::UI::Xaml::FrameworkElement CreateView() noexcept;

  // IViewManagerWithChildren

  void AddView(
      winrt::Windows::UI::Xaml::FrameworkElement const &parent,
      winrt::Windows::UI::Xaml::UIElement const &child,
      int64_t index) noexcept;
  void RemoveAllChildren(winrt::Windows::UI::Xaml::FrameworkElement const &parent) noexcept;
  void RemoveChildAt(winrt::Windows::UI::Xaml::FrameworkElement const &parent, int64_t index) noexcept;
  void ReplaceChild(
      winrt::Windows::UI::Xaml::FrameworkElement const &parent,
      winrt::Windows::UI::Xaml::UIElement const &oldChild,
      winrt::Windows::UI::Xaml::UIElement const &newChild) noexcept;
};

struct HeightToCornerRadiusConverter
    : winrt::implements<HeightToCornerRadiusConverter, winrt::Windows::UI::Xaml::Data::IValueConverter> {
 public:
  HeightToCornerRadiusConverter(){};

  winrt::Windows::Foundation::IInspectable Convert(
      winrt::Windows::Foundation::IInspectable const &value,
      winrt::Windows::UI::Xaml::Interop::TypeName const &targetType,
      winrt::Windows::Foundation::IInspectable const &parameter,
      winrt::hstring const &language) noexcept {
    double d = winrt::unbox_value<double>(value);

    if (isnan(d)) {
      d = 0.0;
    }

    return winrt::box_value(winrt::Windows::UI::Xaml::CornerRadiusHelper::FromUniformRadius(d));
  }

  winrt::Windows::Foundation::IInspectable ConvertBack(
      winrt::Windows::Foundation::IInspectable const &value,
      winrt::Windows::UI::Xaml::Interop::TypeName const &targetType,
      winrt::Windows::Foundation::IInspectable const &parameter,
      winrt::hstring const &language) noexcept {
    return value;
  }

  static winrt::Windows::UI::Xaml::Data::IValueConverter Instance() noexcept {
    static auto const &instance = winrt::make<HeightToCornerRadiusConverter>();
    return instance;
  };

  // IValueConverter
};

} // namespace winrt::SampleLibraryCPP::implementation
