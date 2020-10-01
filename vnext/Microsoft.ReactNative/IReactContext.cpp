// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactContext.h"
#include "DynamicWriter.h"
#ifndef CORE_ABI
#include "XamlUIService.h"
#endif

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

IReactDispatcher ReactContext::JSDispatcher() noexcept {
  return Properties().Get(ReactDispatcherHelper::JSDispatcherProperty()).try_as<IReactDispatcher>();
}

#ifndef CORE_ABI
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
#endif

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

#ifndef CORE_ABI
bool ReactContext::UseWebDebugger() const noexcept {
  return m_context->UseWebDebugger();
}

bool ReactContext::UseFastRefresh() const noexcept {
  return m_context->UseFastRefresh();
}

bool ReactContext::UseDirectDebugger() const noexcept {
  return m_context->UseDirectDebugger();
}

bool ReactContext::DebuggerBreakOnNextLine() const noexcept {
  return m_context->DebuggerBreakOnNextLine();
}

uint16_t ReactContext::DebuggerPort() const noexcept {
  return m_context->DebuggerPort();
}

hstring ReactContext::DebugBundlePath() const noexcept {
  return winrt::to_hstring(m_context->DebugBundlePath());
}

hstring ReactContext::BundleRootPath() const noexcept {
  return winrt::to_hstring(m_context->BundleRootPath());
}

hstring ReactContext::SourceBundleHost() const noexcept {
  return winrt::to_hstring(m_context->SourceBundleHost());
}

uint16_t ReactContext::SourceBundlePort() const noexcept {
  return m_context->SourceBundlePort();
}

hstring ReactContext::JavaScriptBundleFile() const noexcept {
  return winrt::to_hstring(m_context->JavaScriptBundleFile());
}
#endif

Mso::React::IReactContext &ReactContext::GetInner() const noexcept {
  return *m_context;
}

} // namespace winrt::Microsoft::ReactNative::implementation
