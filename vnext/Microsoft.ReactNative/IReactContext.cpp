// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactContext.h"

#include "ReactSupport.h"

namespace winrt::Microsoft::ReactNative::Bridge {

void ReactContext::DispatchEvent(
    winrt::Windows::UI::Xaml::FrameworkElement const &view,
    hstring const &eventName,
    IInspectable const &eventData) noexcept {
  if (auto instance = m_instance.lock()) {
    instance->DispatchEvent(unbox_value<int64_t>(view.Tag()), to_string(eventName), ConvertToDynamic(eventData));
  }
}

void ReactContext::CallJsFunction(
    hstring const &moduleName,
    hstring const &method,
    IInspectable const &params) noexcept {
  if (auto instance = m_instance.lock()) {
    instance->CallJsFunction(to_string(moduleName), to_string(method), ConvertToDynamic(params));
  }
}

} // namespace winrt::Microsoft::ReactNative::Bridge
