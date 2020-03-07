// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlHelper.h"
#include "XamlHelper.g.cpp"

#include <Utils/ValueUtils.h>
#include "DynamicWriter.h"

namespace winrt::Microsoft::ReactNative::implementation {

Windows::UI::Xaml::Media::Brush XamlHelper::BrushFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return react::uwp::BrushFrom(value);
}

Windows::UI::Color XamlHelper::ColorFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return react::uwp::ColorFrom(value);
}

bool XamlHelper::IsValidColorValue(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return react::uwp::IsValidColorValue(value);
}

folly::dynamic XamlHelper::GetFollyDynamicFromValueProvider(JSValueArgWriter const &valueProvider) noexcept {
  auto writer = winrt::make_self<DynamicWriter>();
  valueProvider(*writer);
  return writer->TakeValue();
}

} // namespace winrt::Microsoft::ReactNative::implementation
