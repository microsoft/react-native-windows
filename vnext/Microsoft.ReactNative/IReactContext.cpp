// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "IReactContext.h"
#include "DynamicWriter.h"
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
#include "XamlUIService.h"
#endif

#include "CallInvoker.h"
#include "Utils/Helpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

//=============================================================================
// ReactSettingsSnapshot implementation
//=============================================================================

ReactSettingsSnapshot::ReactSettingsSnapshot(Mso::CntPtr<const Mso::React::IReactSettingsSnapshot> &&settings) noexcept
    : m_settings{std::move(settings)} {}

bool ReactSettingsSnapshot::UseFastRefresh() const noexcept {
  return m_settings->UseFastRefresh();
}

bool ReactSettingsSnapshot::UseDirectDebugger() const noexcept {
  return m_settings->UseDirectDebugger();
}

bool ReactSettingsSnapshot::DebuggerBreakOnNextLine() const noexcept {
  return m_settings->DebuggerBreakOnNextLine();
}

uint16_t ReactSettingsSnapshot::DebuggerPort() const noexcept {
  return m_settings->DebuggerPort();
}

hstring ReactSettingsSnapshot::DebugBundlePath() const noexcept {
  return winrt::to_hstring(m_settings->DebugBundlePath());
}

hstring ReactSettingsSnapshot::BundleRootPath() const noexcept {
  return winrt::to_hstring(m_settings->BundleRootPath());
}

hstring ReactSettingsSnapshot::SourceBundleHost() const noexcept {
  return winrt::to_hstring(m_settings->SourceBundleHost());
}

uint16_t ReactSettingsSnapshot::SourceBundlePort() const noexcept {
  return m_settings->SourceBundlePort();
}

bool ReactSettingsSnapshot::RequestInlineSourceMap() const noexcept {
  return m_settings->RequestInlineSourceMap();
}

hstring ReactSettingsSnapshot::JavaScriptBundleFile() const noexcept {
  return winrt::to_hstring(m_settings->JavaScriptBundleFile());
}

hstring ReactSettingsSnapshot::BundleAppId() const noexcept {
  return winrt::to_hstring(m_settings->BundleAppId());
}

bool ReactSettingsSnapshot::RequestDevBundle() const noexcept {
  return m_settings->RequestDevBundle();
}

Mso::React::IReactSettingsSnapshot const &ReactSettingsSnapshot::GetInner() const noexcept {
  return *m_settings;
}

//=============================================================================
// ReactContext implementation
//=============================================================================

ReactContext::ReactContext(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept
    : m_context{std::move(context)}, m_settings{winrt::make<ReactSettingsSnapshot>(&m_context->SettingsSnapshot())} {}

IReactSettingsSnapshot ReactContext::SettingsSnapshot() noexcept {
  return m_settings;
}

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
#if defined(DEBUG) && defined(USE_FABRIC)
  VerifyElseCrashSz(
      !::Microsoft::ReactNative::IsFabricEnabled(Properties()),
      "ReactContext.JSRuntime is not supported on new arch, use ReactContext.CallInvoker instead.");
#endif
  return Properties().Get(ReactDispatcherHelper::JSDispatcherProperty()).try_as<IReactDispatcher>();
}

winrt::Microsoft::ReactNative::CallInvoker ReactContext::CallInvoker() noexcept {
  return winrt::Microsoft::ReactNative::implementation::CallInvoker::FromProperties(ReactPropertyBag(Properties()));
}

winrt::Windows::Foundation::IInspectable ReactContext::JSRuntime() noexcept {
#if defined(DEBUG) && defined(USE_FABRIC)
  VerifyElseCrashSz(
      !::Microsoft::ReactNative::IsFabricEnabled(Properties()),
      "ReactContext.JSRuntime is not supported on new arch, use ReactContext.CallInvoker instead.");
#endif
  return m_context->JsiRuntime();
}

LoadingState ReactContext::LoadingState() noexcept {
  switch (m_context->State()) {
    case Mso::React::ReactInstanceState::Loading:
    case Mso::React::ReactInstanceState::WaitingForDebugger:
      return LoadingState::Loading;
    case Mso::React::ReactInstanceState::Loaded:
      return LoadingState::Loaded;
    case Mso::React::ReactInstanceState::HasError:
      return LoadingState::HasError;
    case Mso::React::ReactInstanceState::Unloaded:
      return LoadingState::Unloaded;
    default:
      assert(false);
      return LoadingState::HasError;
  };
}

#if !defined(CORE_ABI) && !defined(USE_FABRIC)
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

Mso::React::IReactContext &ReactContext::GetInner() const noexcept {
  return *m_context;
}

} // namespace winrt::Microsoft::ReactNative::implementation
