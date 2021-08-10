// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Windows.System.h>
#include "JSValueWriter.h"
#include "LinkingManagerModule.h"
#include "ReactHost/React.h"

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::System;

namespace Microsoft::ReactNative {

//
// LinkingManagerModule helpers
//

static fire_and_forget openURLAsync(Uri uri, React::ReactPromise<React::JSValue> result) {
  if (co_await Launcher::LaunchUriAsync(uri)) {
    result.Resolve({true});
  } else {
    const auto message = L"Unable to open URL: " + uri.DisplayUri();
    result.Reject(message.c_str());
  }
}

static fire_and_forget canOpenURLAsync(Uri uri, React::ReactPromise<React::JSValue> result) {
  auto status = co_await Launcher::QueryUriSupportAsync(uri, LaunchQuerySupportType::Uri);
  if (status == LaunchQuerySupportStatus::Available) {
    result.Resolve({true});
  } else {
    result.Resolve({false});
  }
}

static std::mutex s_mutex;
static std::vector<LinkingManager *> s_linkingModules;

//
// LinkingManager
//
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

void LinkingManager::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void LinkingManager::openURL(std::string const &url, React::ReactPromise<React::JSValue> result) noexcept {
  Uri uri{winrt::to_hstring(url)};
  m_context.UIDispatcher().Post([uri, result]() mutable { openURLAsync(uri, result); });
}

void LinkingManager::canOpenURL(std::string const &url, React::ReactPromise<React::JSValue> result) noexcept {
  Uri uri{winrt::to_hstring(url)};
  m_context.UIDispatcher().Post([uri, result]() mutable { canOpenURLAsync(uri, result); });
}

void LinkingManager::getInitialURL(React::ReactPromise<React::JSValue> result) noexcept {
  const auto initialUrl = Mso::React::ReactOptions::InitialUrl(m_context.Properties().Handle());
  if (!initialUrl.empty()) {
    result.Resolve({winrt::to_string(initialUrl)});
  } else {
    result.Resolve({nullptr});
  }
}

void LinkingManager::openSettings(React::ReactPromise<React::JSValue> result) noexcept {
  assert(false);
}

void LinkingManager::addListener(std::string &&eventName) noexcept {
  // no-op
}

void LinkingManager::removeListeners(double count) noexcept {
  // no-op
}

/*static*/ void LinkingManager::OpenUri(Uri const &uri) {
  std::vector<LinkingManager *> modules;
  {
    std::scoped_lock lock{s_mutex};
    modules = s_linkingModules;
  }

  for (auto &module : modules) {
    module->HandleOpenUri(uri.AbsoluteUri());
  }
}

void LinkingManager::HandleOpenUri(winrt::hstring const &uri) const noexcept {
  m_context.CallJSFunction(
      L"RCTDeviceEventEmitter", L"emit", L"url", React::JSValueObject{{"url", to_string(uri)}});
}

} // namespace Microsoft::ReactNative
