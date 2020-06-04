// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactContext.h"
#include "DynamicWriter.h"
#include "XamlUIService.h"

namespace winrt::Microsoft::ReactNative::implementation {

ReactContext::ReactContext(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept : m_context{std::move(context)} {}

IReactPropertyBag ReactContext::Properties() noexcept {
  return m_context->Properties();
}

IReactNotificationService ReactContext::Notifications() noexcept {
  return m_context->Notifications();
}

IReactDispatcher ReactContext::UIDispatcher() noexcept {
  return Properties().Get(ReactDispatcherHelper::UIDispatcherProperty()).try_as<IReactDispatcher>();
}

// Deprecated: Use XamlUIService directly.
void ReactContext::DispatchEvent(
    xaml::FrameworkElement const &view,
    hstring const &eventName,
    JSValueArgWriter const &eventDataArgWriter) noexcept {
  auto xamlUIService = Properties()
                           .Get(XamlUIService::XamlUIServiceProperty().Handle())
                           .try_as<winrt::Microsoft::ReactNative::XamlUIService>();

  if (xamlUIService) {
    xamlUIService.DispatchEvent(view, eventName, eventDataArgWriter);
  }
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

Mso::CntPtr<Mso::React::IReactContext> &ReactContext::GetInner() {
  return m_context;
}

} // namespace winrt::Microsoft::ReactNative::implementation
