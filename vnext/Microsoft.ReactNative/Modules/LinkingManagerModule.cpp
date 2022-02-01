// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/ValueUtils.h>
#include <winrt/Windows.System.h>
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

/*static*/ fire_and_forget LinkingManager::canOpenURL(
    std::string url,
    ::React::ReactPromise<::React::JSValue> result) noexcept {
  winrt::Windows::Foundation::Uri uri(Utf8ToUtf16(url));
  auto status = co_await Launcher::QueryUriSupportAsync(uri, LaunchQuerySupportType::Uri);
  if (status == LaunchQuerySupportStatus::Available) {
    result.Resolve(true);
  } else {
    result.Resolve(false);
  }
}

fire_and_forget openUrlAsync(std::string url, ::React::ReactPromise<::React::JSValue> result) noexcept {
  try {
    winrt::Windows::Foundation::Uri uri(Utf8ToUtf16(url));

    if (co_await Launcher::LaunchUriAsync(uri)) {
      result.Resolve({});
    } else {
      result.Reject(("Unable to open URL: " + url).c_str());
    }
  } catch (winrt::hresult_error &e) {
    result.Reject(("Unable to open URL: " + url + "error: " + winrt::to_string(e.message())).c_str());
  }
}

void LinkingManager::openURL(std::string &&url, ::React::ReactPromise<::React::JSValue> &&result) noexcept {
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
  m_context.EmitJSEvent(L"RCTDeviceEventEmitter", L"url", React::JSValueObject{{"url", winrt::to_string(uri)}});
}

/*static*/ void LinkingManager::openSettings(::React::ReactPromise<::React::JSValue> &&result) noexcept {
  result.Reject(L"Could not open settings.  Not suported on this platform.");
}

/*static*/ void LinkingManager::addListener(std::string eventName) noexcept {
  // no-op
}

/*static*/ void LinkingManager::removeListeners(double count) noexcept {
  // no-op
}

/*static*/ void LinkingManager::getInitialURL(::React::ReactPromise<::React::JSValue> &&result) noexcept {
  if (s_initialUri) {
    result.Resolve(to_string(s_initialUri.AbsoluteUri()));
  } else {
    result.Resolve(nullptr);
  }
}

} // namespace Microsoft::ReactNative
