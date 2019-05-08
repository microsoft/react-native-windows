// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "LinkingManagerModule.h"
#include <Utils/ValueUtils.h>
#include <winrt/Windows.System.h>
#include "unicode.h"

#pragma warning(push)
#pragma warning(disable : 4146)
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#pragma warning(pop)

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

using Callback = facebook::xplat::module::CxxModule::Callback;

namespace react { namespace uwp {

//
// LinkingManagerModule helpers
//

static winrt::fire_and_forget openURLAsync(winrt::Windows::Foundation::Uri uri, Callback success, Callback error)
{
  if (co_await winrt::Windows::System::Launcher::LaunchUriAsync(uri))
  {
    success({ true });
  }
  else
  {
    error({});
  }
}

static winrt::fire_and_forget canOpenURLAsync(winrt::Windows::Foundation::Uri uri, Callback success, Callback error)
{
  winrt::Windows::System::LaunchQuerySupportStatus status = co_await winrt::Windows::System::Launcher::QueryUriSupportAsync(uri, winrt::Windows::System::LaunchQuerySupportType::Uri);
  if (status == winrt::Windows::System::LaunchQuerySupportStatus::Available)
  {
    success({ true });
  }
  else
  {
    error({});
  }
}

//
// LinkingManagerModule
//
const char* LinkingManagerModule::name = "LinkingManager";

LinkingManagerModule::LinkingManagerModule()
{
}

LinkingManagerModule::~LinkingManagerModule() = default;

std::string LinkingManagerModule::getName()
{
  return name;
}

std::map<std::string, folly::dynamic> LinkingManagerModule::getConstants()
{
  return {};
}

auto LinkingManagerModule::getMethods() -> std::vector<Method>
{
  return {
    Method("openURL", [](folly::dynamic args, Callback successCallback, Callback errorCallback)
    {
      winrt::Windows::Foundation::Uri uri(facebook::react::unicode::utf8ToUtf16(facebook::xplat::jsArgAsString(args, 0)));
      openURLAsync(uri, successCallback, errorCallback);
    }),
    Method("canOpenURL", [](folly::dynamic args, Callback successCallback, Callback errorCallback)
    {
      winrt::Windows::Foundation::Uri uri(facebook::react::unicode::utf8ToUtf16(facebook::xplat::jsArgAsString(args, 0)));
      canOpenURLAsync(uri, successCallback, errorCallback);
    }),
    Method("getInitialURL", [](folly::dynamic /*args*/, Callback successCallback, Callback /*errorCallback*/)
    {
      successCallback({ nullptr });
    }),
  };
}

} }
