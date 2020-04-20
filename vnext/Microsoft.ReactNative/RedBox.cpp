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
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.Web.Http.h>

#include <boost/algorithm/string.hpp>
#include <regex>
#include "Unicode.h"

namespace xaml = winrt::Windows::UI::Xaml;
using namespace winrt::Windows::Foundation;

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
    m_popup = xaml::Controls::Primitives::Popup{};

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
        L"      <StackPanel Background='#EECC0000' HorizontalAlignment='Stretch' Padding='15,35,15,15' x:Name='StackPanelUpper'>"
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

    m_redboxContent = winrt::unbox_value<xaml::Controls::Grid>(xaml::Markup::XamlReader::Load(xamlString));
    m_errorMessageText = m_redboxContent.FindName(L"ErrorMessageText").as<xaml::Controls::TextBlock>();
    m_errorStackText = m_redboxContent.FindName(L"ErrorStackText").as<xaml::Controls::TextBlock>();
    m_stackPanel = m_redboxContent.FindName(L"StackPanel").as<xaml::Controls::StackPanel>();
    m_stackPanelUpper = m_redboxContent.FindName(L"StackPanelUpper").as<xaml::Controls::StackPanel>();
    m_dismissButton = m_redboxContent.FindName(L"DismissButton").as<xaml::Controls::Button>();
    m_reloadButton = m_redboxContent.FindName(L"ReloadButton").as<xaml::Controls::Button>();

    m_tokenDismiss = m_dismissButton.Click([&](
        IInspectable const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept { Dismiss(); });

    m_tokenReload = m_reloadButton.Click([&](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
      Dismiss();
      Reload();
    });

    PopulateFrameStackUI();
    UpdateErrorMessageUI();

    // Do some kind of sizing logic...
    auto window = xaml::Window::Current();
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
        auto const & /*sender*/, IInspectable const & /*args*/) noexcept {
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
    xaml::Window::Current().SizeChanged(m_tokenSizeChanged);
    m_popup.Closed(m_tokenClosed);
    m_onClosedCallback(GetId());
  }

  uint32_t GetId() const noexcept {
    return m_errorInfo.Id;
  }

 private:
  static bool IsMetroBundlerError(const std::string &message, const std::string &type) {
    // This string must be kept in sync with the one in formatBundlingError in
    // node_modules\metro\src\lib\formatBundlingError.js
    if (message.find_first_of("Metro Bundler has encountered an error") != message.npos) {
      return true;
    }
    return false;
  }

#define METRO_TROUBLESHOOTING_URL "http://aka.ms/RNWTroubleshootMetro"
#define _MAKE_WIDE_STR(x) L##x
#define MAKE_WIDE_STR(x) _MAKE_WIDE_STR(x)

  void UpdateErrorMessageUI() noexcept {
    try {
      auto json = folly::parseJson(m_errorInfo.Message);
      if (json.count("name") && json["name"] == "Error") {
        auto message = json["message"].asString();
        auto stack = json["stack"].asString().substr(ARRAYSIZE("Error: ") + message.length());
        m_errorMessageText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(message));
        m_errorStackText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(stack));
        return;
      } else if (json.count("type") && json["type"] == "InternalError") {
        auto message = json["message"].asString();
        m_errorMessageText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(message));

        if (IsMetroBundlerError(message, json["type"].asString())) {
          xaml::Documents::Hyperlink link;
          link.NavigateUri(Uri(MAKE_WIDE_STR(METRO_TROUBLESHOOTING_URL)));
          xaml::Documents::Run linkRun;

          linkRun.Text(Microsoft::Common::Unicode::Utf8ToUtf16(METRO_TROUBLESHOOTING_URL));
          link.Foreground(
              xaml::Media::SolidColorBrush(winrt::Windows::UI::ColorHelper::FromArgb(0xff, 0xff, 0xff, 0xff)));
          link.Inlines().Append(linkRun);
          xaml::Documents::Run normalRun;
          normalRun.Text(Microsoft::Common::Unicode::Utf8ToUtf16(json["type"].asString() + (" ─ See ")));
          m_errorStackText.Inlines().Append(normalRun);
          m_errorStackText.Inlines().Append(link);
        } else {
          m_errorStackText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(json["type"].asString()));
        }
        return;
      }
    } catch (...) {
      std::string doctype = "<!DOCTYPE HTML>";
      if (boost::istarts_with(m_errorInfo.Message, doctype)) {
        auto webView = xaml::Controls::WebView(xaml::Controls::WebViewExecutionMode::SameThread);

        winrt::hstring content(
            Microsoft::Common::Unicode::Utf8ToUtf16(m_errorInfo.Message.substr(doctype.length()).c_str()));

        webView.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
        webView.VerticalAlignment(xaml::VerticalAlignment::Stretch);
        webView.MinWidth(400);

        m_stackPanel.Children().Clear();
        m_stackPanel.Children().Append(webView);

        auto dispatcher = winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        // XAML doesn't currently provide a way to measure a WebView control,
        // So we're going to tell the WebView to measure itself by running some javascript,
        // and then we'll post a task back to XAML to set the XAML WebView minimum height.
        // The HTML we get from Metro doesn't have any styling, so we'll take advantage of
        // the fact that we're running javascript in the WebView, to set the
        // foreground/background to match the rest of the RedBox.
        // setProperty returns undefined so we continue the first expression with a comma
        // whereas the height expression gets executed because of the ||
        // (since the setProperty calls resulted in undefined).
        // Finally, it's important to note that JS expressions of that are not of string type
        // need to be manually converted to string for them to get marshaled properly back here.
        webView.NavigationCompleted(
            [=](IInspectable const &, xaml::Controls::WebViewNavigationCompletedEventArgs const &) {
              // #eecc0000 ARGB is #be0000 RGB (background doesn't seem to allow alpha channel in WebView)
              std::vector<winrt::hstring> args{
                  L"(document.body.style.setProperty('color', 'white'), "
                  L"document.body.style.setProperty('background', '#be0000')) "
                  L"|| document.documentElement.scrollHeight.toString()"};
              auto async = webView.InvokeScriptAsync(L"eval", std::move(args));

              async.Completed([=](IAsyncOperation<winrt::hstring> const &op, auto &&state) {
                auto result = op.GetResults();
                int documentHeight = _wtoi(result.c_str());
                dispatcher.TryEnqueue([=]() {
                  // Ensure the webview has a min height of the content it wants to show,
                  // and that the horizontal scrollbar that might exist, doesn't occlude the webview.
                  constexpr int horizontalScrollbarHeight = 12;
                  webView.MinHeight(documentHeight + horizontalScrollbarHeight);
                });
              });
            });

        webView.NavigateToString(content);

        m_stackPanel.Margin(xaml::ThicknessHelper::FromUniformLength(0));
        m_stackPanelUpper.Visibility(xaml::Visibility::Collapsed);

        return;
      }
    }
    // fall back to displaying the raw message string
    const std::regex colorsRegex(
        "\\x1b\\[[0-9;]*m"); // strip out console colors which is often added to JS error messages
    const std::string formated = std::regex_replace(m_errorInfo.Message, colorsRegex, "");
    m_errorMessageText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(formated));
    m_errorStackText.Text(L"");
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
      auto frameContent =
          winrt::unbox_value<xaml::Controls::StackPanel>(xaml::Markup::XamlReader::Load(xamlFrameString));
      auto methodText = frameContent.FindName(L"MethodText").as<xaml::Controls::TextBlock>();
      auto frameText = frameContent.FindName(L"FrameText").as<xaml::Controls::TextBlock>();
      methodText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(frame.Method));

      // When the user taps on a stack frame, tell the bundler to load that source in the users editor of choice
      frameContent.Tapped([weakReactHost = m_weakReactHost, f = frame](
                              IInspectable const & /*sender*/, xaml::Input::TappedRoutedEventArgs const & /*e*/) {
        if (auto reactHost = weakReactHost.GetStrongPtr()) {
          auto devSettings = reactHost->Options().DeveloperSettings;
          std::string stackFrameUri = "http://";
          stackFrameUri.append(devSettings.SourceBundleHost.empty() ? "localhost" : devSettings.SourceBundleHost);
          stackFrameUri.append(":");
          stackFrameUri.append(devSettings.SourceBundlePort.empty() ? "8081" : devSettings.SourceBundlePort);
          stackFrameUri.append("/open-stack-frame");

          Uri uri{Microsoft::Common::Unicode::Utf8ToUtf16(stackFrameUri)};
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

  xaml::Controls::StackPanel m_stackPanelUpper{nullptr};
  xaml::Controls::StackPanel m_stackPanel{nullptr};
  xaml::Controls::Primitives::Popup m_popup{nullptr};
  xaml::Controls::Grid m_redboxContent{nullptr};
  xaml::Controls::Button m_dismissButton{nullptr};
  xaml::Controls::Button m_reloadButton{nullptr};
  xaml::Controls::TextBlock m_errorMessageText{nullptr};
  xaml::Controls::TextBlock m_errorStackText{nullptr};

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
