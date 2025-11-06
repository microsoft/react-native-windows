// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/ValueUtils.h>
#include <winrt/Windows.System.h>
#include "../../Shared/InputValidation.h"
#include "LinkingManagerModule.h"
#include "Unicode.h"

#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace ::Microsoft::Common::Unicode;

namespace Microsoft::ReactNative {

//
// LinkingManager
//

void LinkingManager::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

/*static*/ std::mutex LinkingManager::s_mutex;
/*static*/ Uri LinkingManager::s_initialUri{nullptr};
/*static*/ std::vector<LinkingManager *> LinkingManager::s_linkingModules;

LinkingManager::LinkingManager() noexcept {
  std::scoped_lock lock{s_mutex};
  s_linkingModules.push_back(this);
}

LinkingManager::~LinkingManager() noexcept {
  std::scoped_lock lock{s_mutex};
  auto it = std::find(s_linkingModules.begin(), s_linkingModules.end(), this);
  if (it != s_linkingModules.end()) {
    s_linkingModules.erase(it);
  }
}

/*static*/ fire_and_forget LinkingManager::canOpenURL(std::wstring url, ::React::ReactPromise<bool> result) noexcept {
  // SDL Compliance: Validate URL (P0 - CVSS 6.5)
  // RNW is a developer platform - allow localhost by default for Metro, tests, and dev scenarios.
  // Production apps can define RNW_STRICT_SDL to block localhost if needed.
  try {
    std::string urlUtf8 = Utf16ToUtf8(url);
#ifdef RNW_STRICT_SDL
    // Strict SDL mode: block localhost for production apps
    ::Microsoft::ReactNative::InputValidation::URLValidator::ValidateURL(
        urlUtf8, ::Microsoft::ReactNative::InputValidation::AllowedSchemes::LINKING_SCHEMES, false);
#else
    // Developer-friendly: allow localhost for Metro, tests, and development
    ::Microsoft::ReactNative::InputValidation::URLValidator::ValidateURL(
        urlUtf8, ::Microsoft::ReactNative::InputValidation::AllowedSchemes::LINKING_SCHEMES, true);
#endif
  } catch (const std::exception &ex) {
    result.Reject(ex.what());
    co_return;
  }

  winrt::Windows::Foundation::Uri uri(url);
  auto status = co_await Launcher::QueryUriSupportAsync(uri, LaunchQuerySupportType::Uri);
  if (status == LaunchQuerySupportStatus::Available) {
    result.Resolve(true);
  } else {
    result.Resolve(false);
  }
}

fire_and_forget openUrlAsync(std::wstring url, ::React::ReactPromise<void> result) noexcept {
  try {
    winrt::Windows::Foundation::Uri uri(url);

    if (co_await Launcher::LaunchUriAsync(uri)) {
      result.Resolve();
    } else {
      result.Reject(("Unable to open URL: " + Utf16ToUtf8(url)).c_str());
    }
  } catch (winrt::hresult_error &e) {
    result.Reject(("Unable to open URL: " + Utf16ToUtf8(url) + "error: " + winrt::to_string(e.message())).c_str());
  }
}

void LinkingManager::openURL(std::wstring &&url, ::React::ReactPromise<void> &&result) noexcept {
  // VALIDATE URL - arbitrary launch PROTECTION (P0 Critical - CVSS 7.5)
  try {
    std::string urlUtf8 = Utf16ToUtf8(url);
#ifdef _DEBUG
    // Allow localhost in debug builds for Metro development
    ::Microsoft::ReactNative::InputValidation::URLValidator::ValidateURL(
        urlUtf8, ::Microsoft::ReactNative::InputValidation::AllowedSchemes::LINKING_SCHEMES, true);
#else
    ::Microsoft::ReactNative::InputValidation::URLValidator::ValidateURL(
        urlUtf8, ::Microsoft::ReactNative::InputValidation::AllowedSchemes::LINKING_SCHEMES, false);
#endif
  } catch (const std::exception &ex) {
    result.Reject(ex.what());
    return;
  }

  m_context.UIDispatcher().Post(
      [url = std::move(url), result = std::move(result)]() { openUrlAsync(std::move(url), std::move(result)); });
}

/*static*/ void LinkingManager::OpenUri(winrt::Windows::Foundation::Uri const &uri) noexcept {
  if (!s_initialUri) {
    s_initialUri = uri;
  }

  std::vector<LinkingManager *> modules;
  {
    std::scoped_lock lock{s_mutex};
    modules = s_linkingModules;
  }

  for (auto &module : modules) {
    module->HandleOpenUri(uri.AbsoluteUri());
  }
}

void LinkingManager::HandleOpenUri(winrt::hstring const &uri) noexcept {
  // SDL Compliance: Validate URI before emitting event (P2 - CVSS 4.0)
  try {
    std::string uriUtf8 = winrt::to_string(uri);
#ifdef _DEBUG
    // Allow localhost in debug builds for Metro development
    ::Microsoft::ReactNative::InputValidation::URLValidator::ValidateURL(
        uriUtf8, ::Microsoft::ReactNative::InputValidation::AllowedSchemes::LINKING_SCHEMES, true);
#else
    ::Microsoft::ReactNative::InputValidation::URLValidator::ValidateURL(
        uriUtf8, ::Microsoft::ReactNative::InputValidation::AllowedSchemes::LINKING_SCHEMES, false);
#endif
  } catch (const std::exception &) {
    // Silently ignore invalid URIs to prevent crashes
    return;
  }

  m_context.EmitJSEvent(L"RCTDeviceEventEmitter", L"url", React::JSValueObject{{"url", winrt::to_string(uri)}});
}

/*static*/ void LinkingManager::openSettings(::React::ReactPromise<void> &&result) noexcept {
  result.Reject(L"Could not open settings.  Not suported on this platform.");
}

/*static*/ void LinkingManager::addListener(std::string eventName) noexcept {
  // no-op
}

/*static*/ void LinkingManager::removeListeners(double count) noexcept {
  // no-op
}

/*static*/ void LinkingManager::getInitialURL(::React::ReactPromise<std::optional<std::string>> &&result) noexcept {
  if (s_initialUri) {
    result.Resolve(to_string(s_initialUri.AbsoluteUri()));
  } else {
    result.Resolve(std::nullopt);
  }
}

} // namespace Microsoft::ReactNative
