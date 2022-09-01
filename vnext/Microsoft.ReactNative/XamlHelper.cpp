// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlHelper.h"
#include "XamlHelper.g.cpp"

#include <Utils/ValueUtils.h>
#include "DynamicWriter.h"
#include "XamlView.h"

namespace winrt::Microsoft::ReactNative::implementation {

xaml::Media::Brush XamlHelper::BrushFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return ::Microsoft::ReactNative::IsValidColorValue(value) ? ::Microsoft::ReactNative::BrushFrom(value) : nullptr;
}

Windows::UI::Color XamlHelper::ColorFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return ::Microsoft::ReactNative::ColorFrom(value);
}

xaml::DependencyProperty XamlHelper::ReactTagProperty() noexcept {
  return ::Microsoft::ReactNative::ReactTagProperty();
}

int64_t XamlHelper::GetReactTag(xaml::DependencyObject const &dependencyObject) noexcept {
  return ::Microsoft::ReactNative::GetTag(dependencyObject);
}

void XamlHelper::SetReactTag(xaml::DependencyObject const &dependencyObject, int64_t tag) noexcept {
  return ::Microsoft::ReactNative::SetTag(dependencyObject, tag);
}

folly::dynamic XamlHelper::GetFollyDynamicFromValueProvider(JSValueArgWriter const &valueProvider) noexcept {
  auto writer = winrt::make_self<DynamicWriter>();
  valueProvider(*writer);
  return writer->TakeValue();
}

} // namespace winrt::Microsoft::ReactNative::implementation
