// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "codegen/NativeDialogManagerWindowsSpec.g.h"
#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>
#include <queue>

namespace Microsoft::ReactNative {

REACT_MODULE(Alert)
struct Alert : public std::enable_shared_from_this<Alert> {
  using ModuleSpec = ReactNativeSpecs::DialogManagerWindowsSpec;
  using DialogOptions = ReactNativeSpecs::DialogManagerWindowsSpec_DialogOptions;
  using Constants = ReactNativeSpecs::DialogManagerWindowsSpec_Constants;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(showAlert)
  void showAlert(
      DialogOptions &&args,
      std::function<void(std::string)> error,
      std::function<void(std::string, int)> result) noexcept;

  REACT_GET_CONSTANTS(GetConstants)
  Constants GetConstants() noexcept;

 private:
  React::ReactContext m_context;
  Constants m_constants;

  struct AlertRequest {
    DialogOptions args;
    std::function<void(std::string, int)> result;

    AlertRequest(DialogOptions &&rargs, std::function<void(std::string, int)> rresult) noexcept
        : args(std::move(rargs)), result(rresult) {}
  };

  std::queue<AlertRequest> pendingAlerts{};

  void ProcessPendingAlertRequests() noexcept;
};

} // namespace Microsoft::ReactNative
