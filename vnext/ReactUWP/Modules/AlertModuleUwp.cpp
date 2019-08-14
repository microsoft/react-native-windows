// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "AlertModuleUwp.h"
#include <winrt/Windows.UI.Xaml.Controls.h>

#include <Utils\ValueUtils.h>

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
          [](folly::dynamic args, Callback actionResult) {
            AlertModule::showAlert(args, actionResult);
          }),

  };
}

/*static*/ winrt::fire_and_forget AlertModule::showAlert(folly::dynamic &args, Callback actionResult) {

  std::wstring alertTitle;
  std::wstring alertContent;
  std::wstring alertPBT;
  std::wstring alertSBT;
  std::wstring alertCBT;

  for (auto arr : args) {
  
  for (auto &pair : arr.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;
        
      if (propertyName == "title") {
        alertTitle = react::uwp::asHstring(propertyValue);
      }
        
      if (propertyName == "message") {
        alertContent = react::uwp::asHstring(propertyValue);
      }
    
      if (propertyName == "buttonPositive") {
        alertPBT = react::uwp::asHstring(propertyValue);
      }
      if (propertyName == "buttonNegative") {
        alertSBT = react::uwp::asHstring(propertyValue);
      }
      if (propertyName == "buttonNeutral") {
        alertCBT = react::uwp::asHstring(propertyValue);
      }
  }
 }

  winrt::ContentDialog dialog{};
  dialog.Title(winrt::box_value(alertTitle.c_str()));
  dialog.Content(winrt::box_value(alertContent.c_str()));
  dialog.PrimaryButtonText(alertPBT.c_str());
  dialog.SecondaryButtonText(alertSBT.c_str());
  dialog.CloseButtonText(alertCBT.c_str());
  winrt::ContentDialogResult::Primary;
  auto result = co_await dialog.ShowAsync();

  switch (result) {
    case winrt::ContentDialogResult::Primary :
      actionResult({"positive"});
      break;
    case winrt::ContentDialogResult::Secondary :
      actionResult({"negative"});
      break;
    case winrt::ContentDialogResult::None :
      actionResult({"neutral"});
      break;
    default:
      break;
  }
}

AlertModule::AlertModule() {

}

} // namespace uwp
} // namespace react
