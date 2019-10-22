// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "CustomUserControlCPP.g.h"

namespace winrt::SampleLibraryCPP::implementation
{
struct CustomUserControlCPP : CustomUserControlCPPT<CustomUserControlCPP> {
  CustomUserControlCPP();

  static Windows::UI::Xaml::DependencyProperty LabelProperty();

  hstring Label();
  void Label(hstring const &value);

  static void OnLabelChanged(
      Windows::UI::Xaml::DependencyObject const &d,
      Windows::UI::Xaml::DependencyPropertyChangedEventArgs const &e);

 private:
  static Windows::UI::Xaml::DependencyProperty m_labelProperty;
};
} // namespace winrt::SampleLibraryCPP::implementation

namespace winrt::SampleLibraryCPP::factory_implementation {
struct CustomUserControlCPP : CustomUserControlCPPT<
                                  CustomUserControlCPP,
                                  implementation::CustomUserControlCPP> {};
} // namespace winrt::SampleLibraryCPP::factory_implementation

