// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CustomUserControlCPP.h"
#include "CustomUserControlCPP.g.cpp"

namespace winrt::SampleLibraryCPP::implementation {

Windows::UI::Xaml::DependencyProperty CustomUserControlCPP::m_labelProperty =
    Windows::UI::Xaml::DependencyProperty::Register(
        L"Label",
        winrt::xaml_typename<winrt::hstring>(),
        winrt::xaml_typename<winrt::SampleLibraryCPP::CustomUserControlCPP>(),
        Windows::UI::Xaml::PropertyMetadata{
            winrt::box_value(winrt::hstring())});

CustomUserControlCPP::CustomUserControlCPP() {
  DefaultStyleKey(winrt::box_value(L"SampleLibraryCPP.CustomUserControlCPP"));
}

Windows::UI::Xaml::DependencyProperty CustomUserControlCPP::LabelProperty() {
  return m_labelProperty;
}

hstring CustomUserControlCPP::Label() {
  return winrt::unbox_value<winrt::hstring>(GetValue(m_labelProperty));
}

void CustomUserControlCPP::Label(hstring const &value) {
  SetValue(m_labelProperty, winrt::box_value(value));
}

} // namespace winrt::SampleLibraryCPP::implementation
