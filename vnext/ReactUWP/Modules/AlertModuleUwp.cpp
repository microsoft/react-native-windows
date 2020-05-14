// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "AlertModuleUwp.h"
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <Utils\ValueUtils.h>

#include <winrt/Windows.UI.Xaml.Controls.h>

#pragma warning(push)
#pragma warning(disable : 4146)
#include <cxxreact/JsArgumentHelpers.h>
#pragma warning(pop)
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

AlertModule::AlertModule() {}

AlertModule::~AlertModule() = default;

std::string AlertModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> AlertModule::getConstants() {
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method> AlertModule::getMethods() {
  return {
      Method(
          "showAlert", [](folly::dynamic args, Callback actionResult) { AlertModule::showAlert(args, actionResult); }),

  };
}

/*static*/ winrt::fire_and_forget AlertModule::showAlert(folly::dynamic &args, Callback actionResult) {
  winrt::hstring alertTitle;
  winrt::hstring alertContent;
  winrt::hstring alertPrimaryButtonText;
  winrt::hstring alertSecondaryButtonText;
  winrt::hstring alertCloseButtonText;

  for (auto arg : args) {
    for (auto &pair : arg.items()) {
      const std::string &propertyName = pair.first.getString();
      const folly::dynamic &propertyValue = pair.second;

      if (propertyName == "title") {
        alertTitle = react::uwp::asHstring(propertyValue);
      } else if (propertyName == "message") {
        alertContent = react::uwp::asHstring(propertyValue);
      } else if (propertyName == "buttonPositive") {
        alertPrimaryButtonText = react::uwp::asHstring(propertyValue);
      } else if (propertyName == "buttonNegative") {
        alertSecondaryButtonText = react::uwp::asHstring(propertyValue);
      } else if (propertyName == "buttonNeutral") {
        alertCloseButtonText = react::uwp::asHstring(propertyValue);
      }
    }
  }

  winrt::ContentDialog dialog{};
  dialog.Title(winrt::box_value(alertTitle.c_str()));
  dialog.Content(winrt::box_value(alertContent.c_str()));
  dialog.PrimaryButtonText(alertPrimaryButtonText.c_str());
  dialog.SecondaryButtonText(alertSecondaryButtonText.c_str());
  dialog.CloseButtonText(alertCloseButtonText.c_str());
  auto result = co_await dialog.ShowAsync();

  switch (result) {
    case winrt::ContentDialogResult::Primary:
      actionResult({"positive"});
      break;
    case winrt::ContentDialogResult::Secondary:
      actionResult({"negative"});
      break;
    case winrt::ContentDialogResult::None:
      actionResult({"neutral"});
      break;
    default:
      break;
  }
}

} // namespace uwp
} // namespace react
