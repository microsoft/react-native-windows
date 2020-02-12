// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "XamlHelper.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct XamlHelper : XamlHelperT<XamlHelper> {
  XamlHelper() = default;

  static Windows::UI::Xaml::Media::Brush BrushFrom(Microsoft::ReactNative::ValueProvider const &valueProvider);
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct XamlHelper : XamlHelperT<XamlHelper, implementation::XamlHelper> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
