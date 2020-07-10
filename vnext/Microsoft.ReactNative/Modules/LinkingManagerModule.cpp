// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/ValueUtils.h>
#include <winrt/Windows.System.h>
#include "LinkingManagerModule.h"
#include "Unicode.h"

#pragma warning(push)
#pragma warning(disable : 4146)
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#pragma warning(pop)

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

using Callback = facebook::xplat::module::CxxModule::Callback;

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace ::Microsoft::Common::Unicode;

namespace react::uwp {

//
// LinkingManagerModule helpers
//

static fire_and_forget openURLAsync(Uri uri, Callback success, Callback error) {
  if (co_await Launcher::LaunchUriAsync(uri)) {
    success({true});
  } else {
    error({folly::dynamic::object("code", 1)("message", "Unable to open URL: " + Utf16ToUtf8(uri.DisplayUri()))});
  }
}

static fire_and_forget canOpenURLAsync(Uri uri, Callback success, Callback /*error*/) {
  auto status = co_await Launcher::QueryUriSupportAsync(uri, LaunchQuerySupportType::Uri);
  if (status == LaunchQuerySupportStatus::Available) {
    success({true});
  } else {
    success({false});
  }
}

//
// LinkingManagerModule
//
const char *LinkingManagerModule::name = "LinkingManager";
/*static*/ std::mutex LinkingManagerModule::s_mutex;
/*static*/ Uri LinkingManagerModule::s_initialUri{nullptr};
/*static*/ std::vector<LinkingManagerModule *> LinkingManagerModule::s_linkingModules;

LinkingManagerModule::LinkingManagerModule() noexcept {
  std::scoped_lock lock{s_mutex};
  s_linkingModules.push_back(this);
}

LinkingManagerModule::~LinkingManagerModule() noexcept {
  std::scoped_lock lock{s_mutex};
  auto it = std::find(s_linkingModules.begin(), s_linkingModules.end(), this);
  if (it != s_linkingModules.end()) {
    s_linkingModules.erase(it);
  }
}

/*static*/ void LinkingManagerModule::OpenUri(winrt::Windows::Foundation::Uri const &uri) noexcept {
  if (!s_initialUri) {
    s_initialUri = uri;
  }

  std::vector<LinkingManagerModule *> modules;
  {
    std::scoped_lock lock{s_mutex};
    modules = s_linkingModules;
  }

  for (auto &module : modules) {
    module->HandleOpenUri(uri.AbsoluteUri());
  }
}

void LinkingManagerModule::HandleOpenUri(winrt::hstring const &uri) noexcept {
  if (auto instance = getInstance().lock()) {
    instance->callJSFunction(
        "RCTDeviceEventEmitter", "emit", folly::dynamic::array("url", folly::dynamic::object("url", to_string(uri))));
  }
}

std::string LinkingManagerModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> LinkingManagerModule::getConstants() {
  return {};
}

auto LinkingManagerModule::getMethods() -> std::vector<Method> {
  return {
      Method(
          "openURL",
          [](folly::dynamic args, Callback successCallback, Callback errorCallback) {
            winrt::Windows::Foundation::Uri uri(Utf8ToUtf16(facebook::xplat::jsArgAsString(args, 0)));
            openURLAsync(uri, successCallback, errorCallback);
          }),
      Method(
          "canOpenURL",
          [](folly::dynamic args, Callback successCallback, Callback errorCallback) {
            winrt::Windows::Foundation::Uri uri(Utf8ToUtf16(facebook::xplat::jsArgAsString(args, 0)));
            canOpenURLAsync(uri, successCallback, errorCallback);
          }),
      Method(
          "getInitialURL",
          [](folly::dynamic /*args*/, Callback successCallback, Callback /*errorCallback*/) {
            if (s_initialUri) {
              successCallback({to_string(s_initialUri.AbsoluteUri())});
            } else {
              successCallback({nullptr});
            }
          }),
  };
}

} // namespace react::uwp
