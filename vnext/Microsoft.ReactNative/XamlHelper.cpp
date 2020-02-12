// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlHelper.h"
#include "XamlHelper.g.cpp"

#include <Utils/ValueUtils.h>
#include "DynamicWriter.h"

namespace winrt::Microsoft::ReactNative::implementation {

Windows::UI::Xaml::Media::Brush XamlHelper::BrushFrom(Microsoft::ReactNative::ValueProvider const &valueProvider) {
  IJSValueWriter writer = winrt::make<DynamicWriter>();
  valueProvider(writer);

  auto value = writer.as<DynamicWriter>()->TakeValue();
  return react::uwp::BrushFrom(value);
}

} // namespace winrt::Microsoft::ReactNative::implementation
