// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ReactContext.h"
#include <winrt/Microsoft.ReactNative.h>
#include "Microsoft.ReactNative/IReactNotificationService.h"
#include "ReactInstanceWin.h"

namespace Mso::React {

//=============================================================================================
// ReactContext implementation
//=============================================================================================

ReactContext::ReactContext(
    Mso::WeakPtr<ReactInstanceWin> &&reactInstance,
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    winrt::Microsoft::ReactNative::IReactNotificationService const &notifications) noexcept
    : m_reactInstance{std::move(reactInstance)}, m_properties{properties}, m_notifications{notifications} {}

void ReactContext::Destroy() noexcept {
  if (auto notificationService =
          winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNotificationService>(m_notifications)) {
    notificationService->UnsubscribeAll();
  }
}

winrt::Microsoft::ReactNative::IReactPropertyBag ReactContext::Properties() noexcept {
  return m_properties;
}

winrt::Microsoft::ReactNative::IReactNotificationService ReactContext::Notifications() noexcept {
  return m_notifications;
}

void ReactContext::CallJSFunction(std::string &&module, std::string &&method, folly::dynamic &&params) noexcept {
#ifndef CORE_ABI // requires instance
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    instance->CallJsFunction(std::move(module), std::move(method), std::move(params));
  }
#endif
}

void ReactContext::DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept {
#ifndef CORE_ABI // requires instance
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    instance->DispatchEvent(viewTag, std::move(eventName), std::move(eventData));
  }
#endif
}

#ifndef CORE_ABI
ReactInstanceState ReactContext::State() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->State();
  }

  return ReactInstanceState::Unloaded;
}

bool ReactContext::IsLoaded() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->IsLoaded();
  }

  return false;
}

facebook::react::INativeUIManager *ReactContext::NativeUIManager() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->NativeUIManager();
  }

  return nullptr;
}

std::shared_ptr<facebook::react::Instance> ReactContext::GetInnerInstance() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->GetInnerInstance();
  }

  return nullptr;
}

bool ReactContext::UseWebDebugger() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->UseWebDebugger();
  }
  return false;
}

bool ReactContext::UseFastRefresh() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->UseFastRefresh();
  }
  return false;
}

bool ReactContext::UseDirectDebugger() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->UseDirectDebugger();
  }
  return false;
}

bool ReactContext::DebuggerBreakOnNextLine() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->DebuggerBreakOnNextLine();
  }
  return false;
}

uint16_t ReactContext::DebuggerPort() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->DebuggerPort();
  }
  return 0;
}

std::string ReactContext::DebugBundlePath() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->DebugBundlePath();
  }
  return {};
}

std::string ReactContext::BundleRootPath() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->BundleRootPath();
  }
  return {};
}

std::string ReactContext::SourceBundleHost() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->SourceBundleHost();
  }
  return {};
}

uint16_t ReactContext::SourceBundlePort() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->SourceBundlePort();
  }
  return 0;
}

std::string ReactContext::JavaScriptBundleFile() const noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    return instance->JavaScriptBundleFile();
  }
  return {};
}

#endif

} // namespace Mso::React
