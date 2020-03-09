// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "XamlHelper.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct XamlHelper : XamlHelperT<XamlHelper> {
  XamlHelper() = default;

  static Windows::UI::Xaml::Media::Brush BrushFrom(JSValueArgWriter const &valueProvider) noexcept;
  static Windows::UI::Color ColorFrom(JSValueArgWriter const &valueProvider) noexcept;

  static folly::dynamic GetFollyDynamicFromValueProvider(JSValueArgWriter const &valueProvider) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct XamlHelper : XamlHelperT<XamlHelper, implementation::XamlHelper> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
