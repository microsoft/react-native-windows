// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "AlertModule.h"
#include "Unicode.h"

#include <winrt/Windows.UI.ViewManagement.h>
#include "Utils/Helpers.h"

#include <ReactCoreInjection.h>
#include <Shobjidl.h>
#include <winrt/Windows.UI.Popups.h>

namespace Microsoft::ReactNative {

void Alert::showAlert(
    DialogOptions &&args,
    std::function<void(std::string)> /*error*/,
    std::function<void(std::string, int)> result) noexcept {
  m_context.UIDispatcher().Post([weakThis = weak_from_this(), args = std::move(args), result]() mutable {
    if (auto strongThis = weakThis.lock()) {
      strongThis->pendingAlerts.emplace(std::move(args), result);
      if (strongThis->pendingAlerts.size() == 1) {
        strongThis->ProcessPendingAlertRequests();
      }
    }
  });
}

void Alert::ProcessPendingAlertRequestsMessageDialog() noexcept {
  const auto &pendingAlert = pendingAlerts.front();
  const auto &args = pendingAlert.args;
  const auto &result = pendingAlert.result;

  auto cancelable = args.cancelable.value_or(true);
  auto messageDialog = winrt::Windows::UI::Popups::MessageDialog(
      winrt::to_hstring(args.message.value_or(std::string{})), winrt::to_hstring(args.title.value_or(std::string{})));

  if (args.buttonPositive) {
    auto uicommand = winrt::Windows::UI::Popups::UICommand(winrt::to_hstring(args.buttonPositive.value()));
    uicommand.Id(winrt::box_value(m_constants.buttonPositive));
    messageDialog.Commands().Append(uicommand);
    if (args.defaultButton.value_or(0) == m_constants.buttonPositive) {
      messageDialog.DefaultCommandIndex(messageDialog.Commands().Size() - 1);
    }
  }
  if (args.buttonNegative) {
    auto uicommand = winrt::Windows::UI::Popups::UICommand(winrt::to_hstring(args.buttonNegative.value()));
    uicommand.Id(winrt::box_value(m_constants.buttonNegative));
    messageDialog.Commands().Append(uicommand);
    if (args.defaultButton.value_or(0) == m_constants.buttonNegative) {
      messageDialog.DefaultCommandIndex(messageDialog.Commands().Size() - 1);
    }
  }
  if (args.buttonNeutral) {
    auto uicommand = winrt::Windows::UI::Popups::UICommand(winrt::to_hstring(args.buttonNeutral.value()));
    uicommand.Id(winrt::box_value(m_constants.buttonNeutral));
    messageDialog.Commands().Append(uicommand);
    if (args.defaultButton.value_or(0) == m_constants.buttonNeutral) {
      messageDialog.DefaultCommandIndex(messageDialog.Commands().Size() - 1);
    }
  }

  if (!args.cancelable.value_or(true)) {
    messageDialog.CancelCommandIndex(0xffffffff /* -1 doesn't allow cancellation of message dialog */);
  }

  auto hwnd = winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::GetTopLevelWindowId(
      m_context.Properties().Handle());
  if (hwnd) {
    auto initializeWithWindow{messageDialog.as<::IInitializeWithWindow>()};
    initializeWithWindow->Initialize(reinterpret_cast<HWND>(hwnd));
  }

  auto asyncOp = messageDialog.ShowAsync();
  asyncOp.Completed(
      [result, this](
          const winrt::IAsyncOperation<winrt::Windows::UI::Popups::IUICommand> &asyncOp, winrt::AsyncStatus status) {
        auto uicommand = asyncOp.GetResults();
        result(m_constants.buttonClicked, winrt::unbox_value<int>(uicommand.Id()));
        pendingAlerts.pop();
        ProcessPendingAlertRequests();
      });
}

void Alert::ProcessPendingAlertRequests() noexcept {
  if (pendingAlerts.empty())
    return;

  // If we don't have xaml loaded, fallback to using MessageDialog
  ProcessPendingAlertRequestsMessageDialog();
}

Alert::Constants Alert::GetConstants() noexcept {
  return m_constants;
}

void Alert::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_constants.buttonClicked = "buttonClicked";
  m_constants.dismissed = "dismissed";
  m_constants.buttonPositive = -1;
  m_constants.buttonNegative = -2;
  m_constants.buttonNeutral = -3;
}

} // namespace Microsoft::ReactNative
