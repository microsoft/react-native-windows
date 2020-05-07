// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactContext.h"
#include "DynamicWriter.h"

namespace winrt::Microsoft::ReactNative {

ReactContext::ReactContext(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept : m_context{std::move(context)} {}

ReactPropertyBag ReactContext::GlobalProperties() noexcept {
  return m_context->GlobalProperties();
}

ReactPropertyBag ReactContext::InstanceProperties() noexcept {
  return m_context->InstanceProperties();
}

void ReactContext::DispatchEvent(
    xaml::FrameworkElement const &view,
    hstring const &eventName,
    JSValueArgWriter const &eventDataArgWriter) noexcept {
  folly::dynamic eventData; // default to NULLT
  if (eventDataArgWriter != nullptr) {
    auto eventDataWriter = winrt::make_self<DynamicWriter>();
    eventDataArgWriter(*eventDataWriter);
    eventData = eventDataWriter->TakeValue();
  }

  m_context->DispatchEvent(unbox_value<int64_t>(view.Tag()), to_string(eventName), std::move(eventData));
}

void ReactContext::CallJSFunction(
    hstring const &moduleName,
    hstring const &method,
    JSValueArgWriter const &paramsArgWriter) noexcept {
  auto paramsWriter = winrt::make_self<DynamicWriter>();
  paramsArgWriter(*paramsWriter);

  auto params = paramsWriter->TakeValue();
  m_context->CallJSFunction(to_string(moduleName), to_string(method), std::move(params));
}

void ReactContext::EmitJSEvent(
    hstring const &eventEmitterName,
    hstring const &eventName,
    JSValueArgWriter const &paramsArgWriter) noexcept {
  auto paramsWriter = winrt::make_self<DynamicWriter>();
  paramsWriter->WriteArrayBegin();
  paramsWriter->WriteString(winrt::to_hstring(eventName));
  paramsArgWriter(*paramsWriter);
  paramsWriter->WriteArrayEnd();
  auto params = paramsWriter->TakeValue();
  m_context->CallJSFunction(to_string(eventEmitterName), "emit", std::move(params));
}

} // namespace winrt::Microsoft::ReactNative
