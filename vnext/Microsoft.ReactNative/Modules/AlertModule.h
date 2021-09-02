// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "../../codegen/NativeDialogManagerAndroidSpec.g.h"
#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>
#include <queue>

namespace Microsoft::ReactNative {

REACT_STRUCT_INHERITED(WindowsDialogOptions, ReactNativeSpecs::DialogManagerAndroidSpec_DialogOptions)
struct WindowsDialogOptions : ReactNativeSpecs::DialogManagerAndroidSpec_DialogOptions {
  REACT_FIELD(defaultButton)
  int defaultButton;
};

REACT_MODULE(Alert)
struct Alert : public std::enable_shared_from_this<Alert> {
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(showAlert)
  void showAlert(WindowsDialogOptions &&args, std::function<void(std::string)> result) noexcept;

 private:
  React::ReactContext m_context;

  struct AlertRequest {
    WindowsDialogOptions args;
    std::function<void(std::string)> result;

    AlertRequest(WindowsDialogOptions &&rargs, std::function<void(std::string)> rresult) noexcept
        : args(std::move(rargs)), result(rresult) {}
  };

  std::queue<AlertRequest> pendingAlerts{};

  void ProcessPendingAlertRequests() noexcept;
};

} // namespace Microsoft::ReactNative
