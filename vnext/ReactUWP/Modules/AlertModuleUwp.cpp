// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AlertModuleUwp.h"
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <Utils\ValueUtils.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
} // namespace winrt

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace react {
namespace uwp {
//
// Alert
//

const char *AlertModule::name = "RCTAlert";

AlertModule::~AlertModule() = default;

std::string AlertModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> AlertModule::getConstants() {
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method>
AlertModule::getMethods() {
  return {
      Method(
          "showAlert",
          [](folly::dynamic args) {

          }),

  };
}

AlertModule::AlertModule() {
  winrt::Button button{};
  //winrt::ContentDialog dialog{};
  // dialog.Title(winrt::box_value(L"title"));
  // dialog.Content(winrt::box_value(L"content"));
  // dialog.PrimaryButtonText(L"primary");
  // dialog.CloseButtonText(L"close");

  // dialog.ShowAsync();
}

} // namespace uwp
} // namespace react
