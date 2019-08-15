// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <cxxreact/MessageQueueThread.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace react {
namespace uwp {

class AlertModule final : public facebook::xplat::module::CxxModule {
 public:

  AlertModule();
  virtual ~AlertModule();
  
  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static const char *name;

 private:
  static winrt::fire_and_forget showAlert(folly::dynamic &args, Callback actionResult);

};

} // namespace uwp
} // namespace react
