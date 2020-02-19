// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlHelper.h"
#include "XamlHelper.g.cpp"

#include <Utils/ValueUtils.h>
#include "DynamicWriter.h"

namespace winrt::Microsoft::ReactNative::implementation {

Windows::UI::Xaml::Media::Brush XamlHelper::BrushFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto writer = winrt::make_self<DynamicWriter>();
  valueProvider(*writer);

  auto value = writer->TakeValue();
  return react::uwp::BrushFrom(value);
}

} // namespace winrt::Microsoft::ReactNative::implementation
