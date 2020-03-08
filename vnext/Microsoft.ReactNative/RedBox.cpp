// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "RedBox.h"
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include "Unicode.h"

namespace Mso::React {

std::function<void(facebook::react::JSExceptionInfo &&)> CreateRedBoxExceptionCallback(
    std::shared_ptr<facebook::react::MessageQueueThread> uiMessageQueue,
    std::function<void()>&& reloadCallback) noexcept {
  return [uiMessageQueue,
          reloadCallback2 = std::move(reloadCallback)](facebook::react::JSExceptionInfo &&jsExceptionInfo) noexcept {
    uiMessageQueue->runOnQueue([jsExceptionInfo, reloadCallback3 = std::move(reloadCallback2)]() {
      auto popup = winrt::Windows::UI::Xaml::Controls::Primitives::Popup{};

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
          L"    <RowDefinition Height='Auto'/>"
          L"    <RowDefinition Height='*'/>"
          L"    <RowDefinition Height='Auto'/>"
          L"  </Grid.RowDefinitions>"
          L"  <Grid Background='#EECC0000' Grid.Row='0' Grid.ColumnSpan='2' Padding='15,35,15,15'>"
          L"    <TextBlock x:Name='ErrorMessageText' FontSize='20' Foreground='White'/>"
          L"  </Grid>"
          L"  <ScrollViewer Grid.Row='1' Grid.ColumnSpan='2'>"
          L"    <StackPanel x:Name='StackPanel' Margin='15' />"
          L"  </ScrollViewer>"
          L"  <Button x:Name='DismissButton' Grid.Row='2' Grid.Column='0' HorizontalAlignment='Stretch' Margin='15' Style='{StaticResource ButtonRevealStyle}'>Dismiss</Button>"
          L"  <Button x:Name='ReloadButton' Grid.Row='2' Grid.Column='2' HorizontalAlignment='Stretch' Margin='15' Style='{StaticResource ButtonRevealStyle}'>Reload (NYI)</Button>"
          L"</Grid>";

      auto redboxContent = winrt::unbox_value<winrt::Windows::UI::Xaml::Controls::Grid>(
          winrt::Windows::UI::Xaml::Markup::XamlReader::Load(xamlString));
      auto errorMessageText =
          redboxContent.FindName(L"ErrorMessageText").as<winrt::Windows::UI::Xaml::Controls::TextBlock>();
      auto stackpanel = redboxContent.FindName(L"StackPanel").as<winrt::Windows::UI::Xaml::Controls::StackPanel>();
      auto dismissButton = redboxContent.FindName(L"DismissButton").as<winrt::Windows::UI::Xaml::Controls::Button>();
      auto reloadButton = redboxContent.FindName(L"ReloadButton").as<winrt::Windows::UI::Xaml::Controls::Button>();

      winrt::event_token tokenDismiss;
      tokenDismiss = dismissButton.Click(
          [popup](auto const & /*sender*/, winrt::Windows::UI::Xaml::RoutedEventArgs const & /*args*/) noexcept {
            popup.IsOpen(false);
          });

      winrt::event_token tokenReload;
      tokenReload = reloadButton.Click(
          [popup, capturedReloadCallback = std::move(reloadCallback3)](
              auto const & /*sender*/, winrt::Windows::UI::Xaml::RoutedEventArgs const & /*args*/) noexcept {
            popup.IsOpen(false);
            capturedReloadCallback();
          });

      for (const auto frame : jsExceptionInfo.callstack) {
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

        std::stringstream stackFrameInfo;
        stackFrameInfo << frame.File << ":" << frame.Line << ":" << frame.Column;
        frameText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(stackFrameInfo.str()));
        stackpanel.Children().Append(frameContent);
      }

      // Do some kind of sizing logic...
      auto window = winrt::Windows::UI::Xaml::Window::Current();
      auto windowBounds = window.Bounds();
      redboxContent.MaxHeight(windowBounds.Height);
      redboxContent.Height(windowBounds.Height);
      redboxContent.MaxWidth(windowBounds.Width);
      redboxContent.Width(windowBounds.Width);
      winrt::event_token tokenSizeChanged = window.SizeChanged(
          [redboxContent](auto const & /*sender*/, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const &args) {
            redboxContent.MaxHeight(args.Size().Height);
            redboxContent.Height(args.Size().Height);
            redboxContent.MaxWidth(args.Size().Width);
            redboxContent.Width(args.Size().Width);
          });

      // On close cleanup all the events that were hooked up
      // The closed token needs to be allocated so that it can be captured, otherwise we just capture the token before
      // its set to anything useful
      winrt::event_token *tokenClosed = new winrt::event_token;
      *tokenClosed = popup.Closed(
          [tokenDismiss, dismissButton, tokenClosed, popup, tokenSizeChanged, tokenReload, reloadButton](
              auto const & /*sender*/, winrt::Windows::Foundation::IInspectable const & /*args*/) noexcept {
            dismissButton.Click(tokenDismiss);
            reloadButton.Click(tokenReload);
            winrt::Windows::UI::Xaml::Window::Current().SizeChanged(tokenSizeChanged);
            popup.Closed(*tokenClosed);
            delete tokenClosed;
          });
      errorMessageText.Text(Microsoft::Common::Unicode::Utf8ToUtf16(jsExceptionInfo.exceptionMessage));

      popup.Child(redboxContent);
      popup.IsOpen(true);
    });
  };
}

} // namespace Mso::React
