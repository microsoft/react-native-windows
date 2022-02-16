// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlHelper.h"
#include "XamlHelper.g.cpp"

#include <Utils/ValueUtils.h>
#include "DynamicWriter.h"

namespace winrt::Microsoft::ReactNative::implementation {

xaml::Media::Brush XamlHelper::BrushFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return ::Microsoft::ReactNative::IsValidColorValue(value) ? ::Microsoft::ReactNative::BrushFrom(value) : nullptr;
}

Windows::UI::Color XamlHelper::ColorFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return ::Microsoft::ReactNative::ColorFrom(value);
}

folly::dynamic XamlHelper::GetFollyDynamicFromValueProvider(JSValueArgWriter const &valueProvider) noexcept {
  auto writer = winrt::make_self<DynamicWriter>();
  valueProvider(*writer);
  return writer->TakeValue();
}

} // namespace winrt::Microsoft::ReactNative::implementation
