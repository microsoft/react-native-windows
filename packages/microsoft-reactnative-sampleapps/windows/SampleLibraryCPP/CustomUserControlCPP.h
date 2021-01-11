// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "CustomUserControlCpp.g.h"

namespace winrt::SampleLibraryCpp::implementation {
struct CustomUserControlCpp : CustomUserControlCppT<CustomUserControlCpp> {
  CustomUserControlCpp();

  static Windows::UI::Xaml::DependencyProperty LabelProperty();

  hstring Label();
  void Label(hstring const &value);

  static void OnLabelChanged(
      Windows::UI::Xaml::DependencyObject const &d,
      Windows::UI::Xaml::DependencyPropertyChangedEventArgs const &e);

 private:
  static Windows::UI::Xaml::DependencyProperty m_labelProperty;
};
} // namespace winrt::SampleLibraryCpp::implementation

namespace winrt::SampleLibraryCpp::factory_implementation {
struct CustomUserControlCpp : CustomUserControlCppT<CustomUserControlCpp, implementation::CustomUserControlCpp> {};
} // namespace winrt::SampleLibraryCpp::factory_implementation
