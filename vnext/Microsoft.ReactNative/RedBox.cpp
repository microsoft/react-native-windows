// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "RedBox.h"
#include <functional/functor.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.Web.Http.h>

#include <regex>
#include "Unicode.h"

namespace Mso::React {

struct RedBox : public std::enable_shared_from_this<RedBox> {
  RedBox(
      Mso::WeakPtr<IReactHost> weakReactHost,
      Mso::Functor<void(uint32_t)> &&onClosedCallback,
      ErrorInfo &&errorInfo) noexcept
      : m_weakReactHost(std::move(weakReactHost)),
        m_onClosedCallback(std::move(onClosedCallback)),
        m_errorInfo(std::move(errorInfo)) {}

  void Dismiss() noexcept {
    if (m_popup) {
      m_popup.IsOpen(false);
    }
  }

  void Reload() noexcept {
    if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
      reactHost->ReloadInstance();
    }
  }

  void WindowSizeChanged(winrt::Windows::UI::Core::WindowSizeChangedEventArgs const &args) noexcept {
    m_redboxContent.MaxHeight(args.Size().Height);
    m_redboxContent.Height(args.Size().Height);
    m_redboxContent.MaxWidth(args.Size().Width);
    m_redboxContent.Width(args.Size().Width);
  }

  void ShowNewJSError() noexcept {
    m_showing = true;
    m_popup = winrt::Windows::UI::Xaml::Controls::Primitives::Popup{};

    const winrt::hstring xamlString =
        L"<Grid Background='{ThemeResource ContentDialogBackground}'"
        L"  xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L"  xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'"
        L"  RequestedTheme='Dark'>"
        L"  <Grid.ColumnDefinitions>"
        L"    <ColumnDefinition Width='*'/>"
        L"    <ColumnDefinition Width='*'/>"
        L"  </Grid.ColumnDefinitions>"
        L"  <Grid.RowDefinitions>"
        L"    <RowDefinition Height='*'/>"
        L"    <RowDefinition Height='Auto'/>"
        L"  </Grid.RowDefinitions>"
        L"  <ScrollViewer Grid.Row='0' Grid.ColumnSpan='2' HorizontalAlignment='Stretch'>"
        L"    <StackPanel HorizontalAlignment='Stretch'>"
        L"      <StackPanel Background='#EECC0000' HorizontalAlignment='Stretch' Padding='15,35,15,15'>"
        L"        <TextBlock x:Name='ErrorMessageText' FontSize='14' Foreground='White' IsTextSelectionEnabled='true' TextWrapping='Wrap'/>"
        L"        <Border Padding='15,15,15,0'/>"
        L"        <TextBlock x:Name='ErrorStackText' FontSize='12' FontFamily='Consolas' Foreground='White' IsTextSelectionEnabled='true' TextWrapping='Wrap'/>"
        L"      </StackPanel>"
        L"      <StackPanel HorizontalAlignment='Stretch' x:Name='StackPanel' Margin='15' />"
        L"    </StackPanel>"
        L"  </ScrollViewer>"
        L"  <Button x:Name='DismissButton' Grid.Row='1' Grid.Column='0' HorizontalAlignment='Stretch' Margin='15' Style='{StaticResource ButtonRevealStyle}'>Dismiss</Button>"
        L"  <Button x:Name='ReloadButton' Grid.Row='1' Grid.Column='2' HorizontalAlignment='Stretch' Margin='15' Style='{StaticResource ButtonRevealStyle}'>Reload (NYI)</Button>"
        L"</Grid>";

    m_redboxContent = winrt::unbox_value<winrt::Windows::UI::Xaml::Controls::Grid>(
        winrt::Windows::UI::Xaml::Markup::XamlReader::Load(xamlString));
    m_errorMessageText =
        m_redboxContent.FindName(L"ErrorMessageText").as<winrt::Windows::UI::Xaml::Controls::TextBlock>();
    m_errorStackText = m_redboxContent.FindName(L"ErrorStackText").as<winrt::Windows::UI::Xaml::Controls::TextBlock>();
    m_stackPanel = m_redboxContent.FindName(L"StackPanel").as<winrt::Windows::UI::Xaml::Controls::StackPanel>();
    m_dismissButton = m_redboxContent.FindName(L"DismissButton").as<winrt::Windows::UI::Xaml::Controls::Button>();
    m_reloadButton = m_redboxContent.FindName(L"ReloadButton").as<winrt::Windows::UI::Xaml::Controls::Button>();

    m_tokenDismiss = m_dismissButton.Click([&](
        winrt::Windows::Foundation::IInspectable const & /*sender*/,
        winrt::Windows::UI::Xaml::RoutedEventArgs const & /*args*/) noexcept { Dismiss(); });

    m_tokenReload = m_reloadButton.Click([&](
        auto const & /*sender*/, winrt::Windows::UI::Xaml::RoutedEventArgs const & /*args*/) noexcept {
      Dismiss();
      Reload();
    });

    PopulateFrameStackUI();
    UpdateErorrMessageUI();

    // Do some kind of sizing logic...
    auto window = winrt::Windows::UI::Xaml::Window::Current();
    auto windowBounds = window.Bounds();
    m_redboxContent.MaxHeight(windowBounds.Height);
    m_redboxContent.Height(windowBounds.Height);
    m_redboxContent.MaxWidth(windowBounds.Width);
    m_redboxContent.Width(windowBounds.Width);
    m_tokenSizeChanged = window.SizeChanged(
        [&](auto const & /*sender*/, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const &args) {
          WindowSizeChanged(args);
        });

    m_tokenClosed = m_popup.Closed([wkThis = std::weak_ptr(shared_from_this())](
        auto const & /*sender*/, winrt::Windows::Foundation::IInspectable const & /*args*/) noexcept {
      if (auto pthis = wkThis.lock()) {
        pthis->OnPopupClosed();
      }
    });

    m_popup.Child(m_redboxContent);
    m_popup.IsOpen(true);
  }

  void UpdateError(const ErrorInfo &&info) noexcept {
    m_errorInfo = std::move(info);
    if (m_showing) {
      PopulateFrameStackUI();
    }
  }

  void OnPopupClosed() noexcept {
    m_showing = false;
    m_dismissButton.Click(m_tokenDismiss);
    m_reloadButton.Click(m_tokenReload);
    winrt::Windows::UI::Xaml::Window::Current().SizeChanged(m_tokenSizeChanged);
    m_popup.Closed(m_tokenClosed);
    m_onClosedCallback(GetId());
  }

  uint32_t GetId() const noexcept {
    return m_errorInfo.Id;
  }

 private:
  void UpdateErorrMessageUI() noexcept {
    const std::regex colorsRegex(
        "\\x1b\\[[0-9;]*m"); // strip out console colors which is often added to JS error messages
    std::string formated = std::regex_replace(m_errorInfo.Message, colorsRegex, "");

    auto json = folly::parseJson(m_errorInfo.Message);
    if (json.count("name") && json["name"] == "Error") {
      auto message = json["message"].asString();
      auto stack = json["stack"].asString().substr(ARRAYSIZE("Error: ") + message.length());
      m_errorMessageText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(message));
      m_errorStackText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(stack));
    } else {
      m_errorMessageText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(formated));
      m_errorStackText.Text(L"");
    }
  }

  void PopulateFrameStackUI() noexcept {
    m_stackPanel.Children().Clear();
    for (const auto frame : m_errorInfo.Callstack) {
      const winrt::hstring xamlFrameString =
          L"<StackPanel Margin='0,5,0,5'"
          L"  xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
          L"  xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'"
          L"  >"
          L"  <TextBlock x:Name='MethodText' FontSize='14' Foreground='White' TextWrapping='Wrap' FontFamily='Consolas'/>"
          L"  <TextBlock x:Name='FrameText' FontSize='14' Foreground='Gray' TextWrapping='Wrap' FontFamily='Consolas'/>"
          L"</StackPanel>";
      auto frameContent = winrt::unbox_value<winrt::Windows::UI::Xaml::Controls::StackPanel>(
          winrt::Windows::UI::Xaml::Markup::XamlReader::Load(xamlFrameString));
      auto methodText = frameContent.FindName(L"MethodText").as<winrt::Windows::UI::Xaml::Controls::TextBlock>();
      auto frameText = frameContent.FindName(L"FrameText").as<winrt::Windows::UI::Xaml::Controls::TextBlock>();
      methodText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(frame.Method));

      // When the user taps on a stack frame, tell the bundler to load that source in the users editor of choice
      frameContent.Tapped([weakReactHost = m_weakReactHost, f = frame](
                              winrt::Windows::Foundation::IInspectable const & /*sender*/,
                              winrt::Windows::UI::Xaml::Input::TappedRoutedEventArgs const & /*e*/) {
        if (auto reactHost = weakReactHost.GetStrongPtr()) {
          auto devSettings = reactHost->Options().DeveloperSettings;
          std::string stackFrameUri = "http://";
          stackFrameUri.append(devSettings.SourceBundleHost.empty() ? "localhost" : devSettings.SourceBundleHost);
          stackFrameUri.append(":");
          stackFrameUri.append(devSettings.SourceBundlePath.empty() ? "8081" : devSettings.SourceBundlePort);
          stackFrameUri.append("/open-stack-frame");

          winrt::Windows::Foundation::Uri uri{Microsoft::Common::Unicode::Utf8ToUtf16(stackFrameUri)};
          winrt::Windows::Web::Http::HttpClient httpClient{};
          winrt::Windows::Data::Json::JsonObject payload{};

          auto fileSanitized = f.File;
          // Bundler will not launch filenames containing :'s, so strip off the <drive letter>:
          if (fileSanitized[1] == ':') {
            fileSanitized.erase(fileSanitized.begin());
            fileSanitized.erase(fileSanitized.begin());
          }
          payload.SetNamedValue(
              L"file",
              winrt::Windows::Data::Json::JsonValue::CreateStringValue(
                  Microsoft::Common::Unicode::Utf8ToUtf16(fileSanitized)));
          payload.SetNamedValue(
              L"methodName",
              winrt::Windows::Data::Json::JsonValue::CreateStringValue(
                  Microsoft::Common::Unicode::Utf8ToUtf16(f.Method)));
          payload.SetNamedValue(L"lineNumber", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(f.Line));
          payload.SetNamedValue(L"column", winrt::Windows::Data::Json::JsonValue::CreateNumberValue(f.Column));
          winrt::Windows::Web::Http::HttpStringContent content(
              payload.ToString(),
              winrt::Windows::Storage::Streams::UnicodeEncoding::Utf8,
              L"application/json; charset=utf-8");
          httpClient.TryPostAsync(uri, content);
        }
      });

      std::stringstream stackFrameInfo;
      stackFrameInfo << frame.File << ":" << frame.Line << ":" << frame.Column;
      frameText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(stackFrameInfo.str()));
      m_stackPanel.Children().Append(frameContent);
    }
  }

  winrt::Windows::UI::Xaml::Controls::StackPanel m_stackPanel{nullptr};
  winrt::Windows::UI::Xaml::Controls::Primitives::Popup m_popup{nullptr};
  winrt::Windows::UI::Xaml::Controls::Grid m_redboxContent{nullptr};
  winrt::Windows::UI::Xaml::Controls::Button m_dismissButton{nullptr};
  winrt::Windows::UI::Xaml::Controls::Button m_reloadButton{nullptr};
  winrt::Windows::UI::Xaml::Controls::TextBlock m_errorMessageText{nullptr};
  winrt::Windows::UI::Xaml::Controls::TextBlock m_errorStackText{nullptr};

  bool m_showing = false;
  Mso::Functor<void(uint32_t)> m_onClosedCallback;
  winrt::event_token m_tokenClosed;
  winrt::event_token m_tokenDismiss;
  winrt::event_token m_tokenReload;
  winrt::event_token m_tokenSizeChanged;
  ErrorInfo m_errorInfo;
  Mso::WeakPtr<IReactHost> m_weakReactHost;
};

/*
 * This class is implemented such that the methods on IRedBoxHandler are thread safe.
 */
struct RedBoxHandler : public std::enable_shared_from_this<RedBoxHandler>, IRedBoxHandler {
  RedBoxHandler(
      Mso::WeakPtr<Mso::React::IReactHost> weakReactHost,
      std::shared_ptr<facebook::react::MessageQueueThread> uiMessageQueue) noexcept
      : m_weakReactHost(std::move(weakReactHost)), m_wkUIMessageQueue(std::move(uiMessageQueue)) {}

  ~RedBoxHandler() {
    // Hide any currently showing redboxes
    std::vector<std::shared_ptr<RedBox>> redboxes;
    {
      std::scoped_lock lock{m_lockRedBox};
      std::swap(m_redboxes, redboxes);
    }
    if (auto uiQueue = m_wkUIMessageQueue.lock()) {
      uiQueue->runOnQueue([redboxes = std::move(redboxes)]() {
        for (const auto redbox : redboxes) {
          redbox->Dismiss();
        }
      });
    }
  }

  virtual void showNewError(ErrorInfo &&info, ErrorType /*exceptionType*/) override {
    std::shared_ptr<RedBox> redbox(std::make_shared<RedBox>(
        m_weakReactHost,
        [wkthis = std::weak_ptr(shared_from_this())](uint32_t id) {
          if (auto pthis = wkthis.lock()) {
            pthis->onDismissedCallback(id);
          }
        },
        std::move(info)));
    {
      std::scoped_lock lock{m_lockRedBox};
      m_redboxes.push_back(std::move(redbox));
    }
    showTopJSError();
  }

  virtual bool isDevSupportEnabled() override {
    if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
      return reactHost->Options().DeveloperSettings.IsDevModeEnabled;
    }

    return false;
  }

  virtual void updateError(ErrorInfo &&info) override {
    std::shared_ptr<RedBox> redbox;
    {
      std::scoped_lock lock{m_lockRedBox};
      for (auto it = m_redboxes.begin(); it != m_redboxes.end(); ++it) {
        if ((*it)->GetId() == info.Id) {
          redbox = *it;
          break;
        }
      }
    }
    if (redbox) {
      if (auto uiQueue = m_wkUIMessageQueue.lock()) {
        uiQueue->runOnQueue([redboxCaptured = std::move(redbox), errorInfo = std::move(info)]() {
          redboxCaptured->UpdateError(std::move(errorInfo));
        });
      }
    }
  }

  virtual void dismissRedbox() override {
    if (auto uiQueue = m_wkUIMessageQueue.lock()) {
      uiQueue->runOnQueue([&]() {
        std::scoped_lock lock{m_lockRedBox};
        if (!m_redboxes.empty())
          m_redboxes[0]->Dismiss();
      });
    }
  }

 private:
  // When notified by a redbox that its been dismisssed
  void onDismissedCallback(uint32_t id) noexcept {
    // Save a local ref, so if we are removing the last redbox which could hold the last ref to the RedBoxHandler
    // We ensure that we exit the mutex before the handler is destroyed.
    std::shared_ptr<RedBox> redbox;
    {
      {
        std::scoped_lock lock{m_lockRedBox};
        for (auto it = m_redboxes.begin(); it != m_redboxes.end(); ++it) {
          if ((*it)->GetId() == id) {
            redbox = *it;
            it = m_redboxes.erase(it);
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
      if (!m_redboxes.empty()) {
        redbox = m_redboxes[0];
      }
    }

    if (auto uiQueue = m_wkUIMessageQueue.lock()) {
      if (m_showingRedBox || !redbox) // Only show one redbox at a time
        return;
      m_showingRedBox = true;
      uiQueue->runOnQueue([redboxCaptured = std::move(redbox)]() { redboxCaptured->ShowNewJSError(); });
    }
  }

  bool m_showingRedBox = false; // Access from UI Thread only
  std::mutex m_lockRedBox;
  std::vector<std::shared_ptr<RedBox>> m_redboxes; // Protected by m_lockRedBox
  const Mso::WeakPtr<IReactHost> m_weakReactHost;
  const std::weak_ptr<facebook::react::MessageQueueThread> m_wkUIMessageQueue;
};

std::shared_ptr<IRedBoxHandler> CreateRedBoxHandler(
    Mso::WeakPtr<IReactHost> &&weakReactHost,
    std::shared_ptr<facebook::react::MessageQueueThread> &&uiMessageQueue) noexcept {
  return std::make_shared<RedBoxHandler>(std::move(weakReactHost), std::move(uiMessageQueue));
}

} // namespace Mso::React
