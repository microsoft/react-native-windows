// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "../../codegen/NativeDialogManagerWindowsSpec.g.h"
#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>
#include <queue>

namespace Microsoft::ReactNative {

REACT_MODULE(Alert)
struct Alert : public std::enable_shared_from_this<Alert> {
  using DialogOptions = ReactNativeSpecs::DialogManagerWindowsSpec_DialogOptions;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(showAlert)
  void showAlert(DialogOptions &&args, std::function<void(std::string)> result) noexcept;

 private:
  React::ReactContext m_context;

  struct AlertRequest {
    DialogOptions args;
    std::function<void(std::string)> result;

    AlertRequest(DialogOptions &&rargs, std::function<void(std::string)> rresult) noexcept
        : args(std::move(rargs)), result(rresult) {}
  };

  std::queue<AlertRequest> pendingAlerts{};

  void ProcessPendingAlertRequests() noexcept;
};

} // namespace Microsoft::ReactNative
