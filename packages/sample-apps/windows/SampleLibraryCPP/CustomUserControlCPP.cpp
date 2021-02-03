// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CustomUserControlCpp.h"
#include "CustomUserControlCpp.g.cpp"

namespace winrt::SampleLibraryCpp::implementation {

Windows::UI::Xaml::DependencyProperty CustomUserControlCpp::m_labelProperty =
    Windows::UI::Xaml::DependencyProperty::Register(
        L"Label",
        winrt::xaml_typename<winrt::hstring>(),
        winrt::xaml_typename<winrt::SampleLibraryCpp::CustomUserControlCpp>(),
        Windows::UI::Xaml::PropertyMetadata{winrt::box_value(winrt::hstring())});

CustomUserControlCpp::CustomUserControlCpp() {
  DefaultStyleKey(winrt::box_value(L"SampleLibraryCpp.CustomUserControlCpp"));
}

Windows::UI::Xaml::DependencyProperty CustomUserControlCpp::LabelProperty() {
  return m_labelProperty;
}

hstring CustomUserControlCpp::Label() {
  return winrt::unbox_value<winrt::hstring>(GetValue(m_labelProperty));
}

void CustomUserControlCpp::Label(hstring const &value) {
  SetValue(m_labelProperty, winrt::box_value(value));
}

} // namespace winrt::SampleLibraryCpp::implementation
