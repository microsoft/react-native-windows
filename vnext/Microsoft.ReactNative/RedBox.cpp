// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "RedBox.h"
#include <ReactPropertyBag.h>
#include <boost/algorithm/string.hpp>
#include <functional/functor.h>
#include <regex>
#include "Base/FollyIncludes.h"
#include "DevServerHelper.h"
#include "RedBoxErrorInfo.h"
#include "Unicode.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>

#ifndef CORE_ABI

#include <ReactCoreInjection.h>
#include <Shobjidl.h>
#include <Utils/Helpers.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.Web.Http.h>
#include "CppWinRTIncludes.h"
#include "Utils/Helpers.h"
#endif // CORE_ABI
#include "XamlUtils.h"

using namespace winrt::Windows::Foundation;

namespace Mso::React {

using IInspectable = winrt::Windows::Foundation::IInspectable;

#ifndef CORE_ABI
struct RedBox : public std::enable_shared_from_this<RedBox> {
  RedBox(
      winrt::Microsoft::ReactNative::ReactPropertyBag &propBag,
      const Mso::WeakPtr<IReactHost> &weakReactHost,
      Mso::Functor<void(uint32_t)> &&onClosedCallback,
      ErrorInfo &&errorInfo) noexcept
      : m_weakReactHost(weakReactHost),
        m_propBag(propBag),
        m_onClosedCallback(std::move(onClosedCallback)),
        m_errorInfo(std::move(errorInfo)) {}

  void Reload() noexcept {
    if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
      reactHost->ReloadInstance();
    }
  }

  void Dismiss() noexcept
  {
    if (m_dialogAsync)
      m_dialogAsync.Cancel();
  }

  void ShowNewJsErrorUsingMessageDialog() noexcept {
    // Using MessageDialog is "easy", but it does mean we cannot update the message when symbols are resolved.
    // Ideally we'd have a dialog we could push UI updates to. -- Maybe we could load XAML and host the XAML dialog?

    std::stringstream ss;

    const std::regex colorsRegex(
        "(\\x1b\\[[0-9;]*m)|(\\\\u001b\\[\\d*m)"); // strip out console colors which is often added to JS error messages
    auto message = std::regex_replace(m_errorInfo.Message, colorsRegex, "");
    bool needsMessage = true;

    if (!message.empty() && message[0] == '{') {
      try {
        auto json = folly::parseJson(message);

        if (json.count("type") && json["type"] == "TransformError") {
          ss << "TransformError: " << std::endl;
          if (json.count("errors")) {
            for (const auto &err : json["errors"]) {
              ss << "Filename: " << err["filename"] << std::endl;
              ss << "lineNumber: " << err["lineNumber"] << std::endl;
              ss << err["description"] << std::endl;
            }
          }
          needsMessage = false;
        } else {
          message = folly::toPrettyJson(json);
          message = std::regex_replace(m_errorInfo.Message, colorsRegex, "");
        }
      } catch (...) {
      }
    }

    if (needsMessage)
      ss << message;
    ss << std::endl << std::endl;
    for (auto frame : m_errorInfo.Callstack) {
      ss << frame.Method << "\n" << frame.File << ":" << frame.Line << ":" << frame.Column << std::endl;
    }

    auto msg = winrt::Windows::UI::Popups::MessageDialog(winrt::to_hstring(ss.str()), L"React Native Error");
    auto hwnd = reinterpret_cast<HWND>(
        winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::GetTopLevelWindowId(m_propBag.Handle()));
    auto initializeWithWindow{msg.as<::IInitializeWithWindow>()};
    initializeWithWindow->Initialize(hwnd);
    msg.Commands().Append(winrt::Windows::UI::Popups::UICommand(
        L"Dismiss", [](winrt::Windows::UI::Popups::IUICommand const &command) {}));
    msg.Commands().Append(winrt::Windows::UI::Popups::UICommand(
        L"Reload", [wkHost = m_weakReactHost](winrt::Windows::UI::Popups::IUICommand const &command) {
          if (auto reactHost = wkHost.GetStrongPtr()) {
            reactHost->ReloadInstance();
          }
        }));
    m_dialogAsync = msg.ShowAsync();
  }

  void ShowNewJSError() noexcept {
    m_showing = true;
    ShowNewJsErrorUsingMessageDialog();
  }

  void UpdateError(const ErrorInfo &&info) noexcept {
    m_errorInfo = std::move(info);
  }

  uint32_t GetId() const noexcept {
    return m_errorInfo.Id;
  }

 private:

   winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::UI::Popups::IUICommand> m_dialogAsync { nullptr };

  winrt::Microsoft::ReactNative::ReactPropertyBag m_propBag;
  bool m_showing = false;
  Mso::Functor<void(uint32_t)> m_onClosedCallback;
  ErrorInfo m_errorInfo;
  Mso::WeakPtr<IReactHost> m_weakReactHost;
};

/*
 * This class is implemented such that the methods on IRedBoxHandler are thread safe.
 */
struct DefaultRedBoxHandler final : public std::enable_shared_from_this<DefaultRedBoxHandler>, IRedBoxHandler {
  DefaultRedBoxHandler(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &propBag,
      Mso::WeakPtr<Mso::React::IReactHost> &&weakReactHost,
      const Mso::React::IDispatchQueue2 &uiQueue) noexcept
      : m_weakReactHost{std::move(weakReactHost)}, m_uiQueue{&uiQueue}, m_propBag(propBag) {}

  ~DefaultRedBoxHandler() {
    // Hide any currently showing redBoxes
    std::vector<std::shared_ptr<RedBox>> redBoxes;
    {
      std::scoped_lock lock{m_lockRedBox};
      std::swap(m_redBoxes, redBoxes);
    }
    m_uiQueue->Post([redBoxes = std::move(redBoxes)]() {
      for (const auto &redBox : redBoxes) {
        redBox->Dismiss();
      }
    });
  }

  virtual void showNewError(ErrorInfo &&info, ErrorType /*exceptionType*/) override {
    // Check if the redbox has been suppressed
    if (!info.ExtraData.isNull()) {
      auto iterator = info.ExtraData.find("suppressRedBox");
      if (iterator != info.ExtraData.items().end()) {
        if (iterator->second.asBool()) {
          return;
        }
      }
    }

    std::shared_ptr<RedBox> redbox(std::make_shared<RedBox>(
        m_propBag,
        m_weakReactHost,
        [wkthis = std::weak_ptr(shared_from_this())](uint32_t id) {
          if (auto pthis = wkthis.lock()) {
            pthis->onDismissedCallback(id);
          }
        },
        std::move(info)));
    {
      std::scoped_lock lock{m_lockRedBox};
      m_redBoxes.push_back(std::move(redbox));
    }
    showTopJSError();
  }

  virtual bool isDevSupportEnabled() const override {
    if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
      return Mso::React::ReactOptions::UseDeveloperSupport(reactHost->Options().Properties);
    }

    return false;
  }

  virtual void updateError(ErrorInfo &&info) override {
    std::shared_ptr<RedBox> redbox;
    {
      std::scoped_lock lock{m_lockRedBox};
      for (auto it = m_redBoxes.begin(); it != m_redBoxes.end(); ++it) {
        if ((*it)->GetId() == info.Id) {
          redbox = *it;
          break;
        }
      }
    }

    if (redbox) {
      m_uiQueue->Post([redboxCaptured = std::move(redbox), errorInfo = std::move(info)]() {
        redboxCaptured->UpdateError(std::move(errorInfo));
      });
    }
  }

  virtual void dismissRedbox() override {
    m_uiQueue->Post([wkthis = std::weak_ptr(shared_from_this())]() {
      if (auto pthis = wkthis.lock()) {
        std::scoped_lock lock{pthis->m_lockRedBox};
        if (!pthis->m_redBoxes.empty())
          pthis->m_redBoxes[0]->Dismiss();
      }
    });
  }

 private:
  // When notified by a redbox that its been dismissed
  void onDismissedCallback(uint32_t id) noexcept {
    // Save a local ref, so if we are removing the last redbox which could hold the last ref to the DefaultRedBoxHandler
    // We ensure that we exit the mutex before the handler is destroyed.
    std::shared_ptr<RedBox> redbox;
    {
      {
        std::scoped_lock lock{m_lockRedBox};
        for (auto it = m_redBoxes.begin(); it != m_redBoxes.end(); ++it) {
          if ((*it)->GetId() == id) {
            redbox = *it;
            it = m_redBoxes.erase(it);
            break;
          }
        }
      }
      m_showingRedBox = false;
      showTopJSError(); // Show the next error if there is one
    }
  }

  void showTopJSError() noexcept {
    std::shared_ptr<RedBox> redbox;
    {
      std::scoped_lock lock{m_lockRedBox};
      if (!m_redBoxes.empty()) {
        redbox = m_redBoxes[0];
      }
    }

    if (m_showingRedBox || !redbox) // Only show one redbox at a time
      return;
    m_showingRedBox = true;

    m_uiQueue->Post([redboxCaptured = std::move(redbox)]() { redboxCaptured->ShowNewJSError(); });
  }

 private:
  Mso::CntPtr<const Mso::React::IDispatchQueue2> m_uiQueue;
  winrt::Microsoft::ReactNative::ReactPropertyBag m_propBag;
  bool m_showingRedBox{false}; // Access from UI Thread only
  std::mutex m_lockRedBox;
  std::vector<std::shared_ptr<RedBox>> m_redBoxes; // Protected by m_lockRedBox
  const Mso::WeakPtr<IReactHost> m_weakReactHost;
};
#endif // CORE_ABI

struct RedBoxHandler final : public Mso::React::IRedBoxHandler {
  RedBoxHandler(winrt::Microsoft::ReactNative::IRedBoxHandler const &redBoxHandler) : m_redBoxHandler(redBoxHandler) {}

  static_assert(
      static_cast<uint32_t>(Mso::React::ErrorType::JSFatal) ==
      static_cast<uint32_t>(winrt::Microsoft::ReactNative::RedBoxErrorType::JavaScriptFatal));
  static_assert(
      static_cast<uint32_t>(Mso::React::ErrorType::JSSoft) ==
      static_cast<uint32_t>(winrt::Microsoft::ReactNative::RedBoxErrorType::JavaScriptSoft));
  static_assert(
      static_cast<uint32_t>(Mso::React::ErrorType::Native) ==
      static_cast<uint32_t>(winrt::Microsoft::ReactNative::RedBoxErrorType::Native));

  virtual void showNewError(Mso::React::ErrorInfo &&info, Mso::React::ErrorType errorType) override {
    m_redBoxHandler.ShowNewError(
        winrt::make<RedBoxErrorInfo>(std::move(info)),
        static_cast<winrt::Microsoft::ReactNative::RedBoxErrorType>(static_cast<uint32_t>(errorType)));
  }
  virtual bool isDevSupportEnabled() const override {
    return m_redBoxHandler.IsDevSupportEnabled();
  }
  virtual void updateError(Mso::React::ErrorInfo &&info) override {
    m_redBoxHandler.UpdateError(winrt::make<RedBoxErrorInfo>(std::move(info)));
  }

  virtual void dismissRedbox() override {
    m_redBoxHandler.DismissRedBox();
  }

 private:
  winrt::Microsoft::ReactNative::IRedBoxHandler m_redBoxHandler;
};

std::shared_ptr<IRedBoxHandler> CreateRedBoxHandler(
    winrt::Microsoft::ReactNative::IRedBoxHandler const &redBoxHandler) noexcept {
  return std::make_shared<RedBoxHandler>(redBoxHandler);
}

std::shared_ptr<IRedBoxHandler> CreateDefaultRedBoxHandler(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &propBag,
    Mso::WeakPtr<IReactHost> &&weakReactHost,
    const Mso::React::IDispatchQueue2 &uiQueue) noexcept {
#ifndef CORE_ABI
  return std::make_shared<DefaultRedBoxHandler>(propBag, std::move(weakReactHost), uiQueue);
#else
  return nullptr;
#endif
}

} // namespace Mso::React
