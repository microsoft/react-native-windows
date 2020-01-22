// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactContext.h"

namespace winrt::Microsoft::ReactNative {

void ReactContext::DispatchEvent(
    winrt::Windows::UI::Xaml::FrameworkElement const &view,
    hstring const &eventName,
    ReactArgWriter const &eventDataArgWriter) noexcept {
  if (auto instance = m_instance.lock()) {
    IJSValueWriter eventDataWriter = winrt::make<DynamicWriter>();
    eventDataArgWriter(eventDataWriter);

    auto eventData = eventDataWriter.as<DynamicWriter>()->TakeValue();
    instance->DispatchEvent(unbox_value<int64_t>(view.Tag()), to_string(eventName), std::move(eventData));
  }
}

void ReactContext::CallJsFunction(
    hstring const &moduleName,
    hstring const &method,
    ReactArgWriter const &paramsArgWriter) noexcept {
  if (auto instance = m_instance.lock()) {
    IJSValueWriter paramsWriter = winrt::make<DynamicWriter>();
    paramsArgWriter(paramsWriter);

    auto params = paramsWriter.as<DynamicWriter>()->TakeValue();
    instance->CallJsFunction(to_string(moduleName), to_string(method), std::move(params));
  }
}

} // namespace winrt::Microsoft::ReactNative
