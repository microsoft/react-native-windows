// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ConfigureBundlerDlg.h"
#include "DevServerHelper.h"
#include "IReactDispatcher.h"
#include "UI.Xaml.Controls.Primitives.h"
#include "UI.Xaml.Controls.h"
#include "UI.Xaml.Input.h"
#include "UI.Xaml.Markup.h"
#include "Unicode.h"
#include "Utils/Helpers.h"
#include "winrt/Windows.UI.Xaml.Interop.h"

namespace Microsoft::ReactNative {

struct ConfigureBundlerDlg : winrt::implements<ConfigureBundlerDlg, winrt::IInspectable> {
  ConfigureBundlerDlg(Mso::WeakPtr<Mso::React::IReactHost> weakReactHost) : m_weakReactHost(std::move(weakReactHost)) {
    if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
      auto uiDispatcher = winrt::Microsoft::ReactNative::implementation::ReactDispatcher::GetUIDispatcher(
          reactHost->Options().Properties);
      uiDispatcher.Post([pThis = get_strong()]() { pThis->CreateAndShowUI(); });
    }
  }

 private:
  void CreateAndShowUI() noexcept {
    if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
      const winrt::hstring xamlString =
          LR"(
  <StackPanel
    xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'
    xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'
    Background='{ThemeResource ContentDialogBackground}'
    HorizontalAlignment='Center'
    MinWidth="{ThemeResource ContentDialogMinWidth}"
    MaxWidth="{ThemeResource ContentDialogMaxWidth}"
    MinHeight="{ThemeResource ContentDialogMinHeight}"
    MaxHeight="{ThemeResource ContentDialogMaxHeight}"
    Margin='-10'
    Padding="{ThemeResource ContentDialogPadding}"
  >
    <StackPanel.Transitions>
      <TransitionCollection>
        <EntranceThemeTransition />
      </TransitionCollection>
    </StackPanel.Transitions>
    <TextBlock FontSize="20" Margin="{ThemeResource ContentDialogTitleMargin}">Configure Bundler</TextBlock>
    <TextBlock Opacity='0.5' TextWrapping="Wrap" Margin="{ThemeResource ContentDialogTitleMargin}">Provide a custom bundler address, port and entrypoint.<LineBreak/>Applying these settings will cause the instance to be reloaded.</TextBlock>
    <TextBox x:Name="Host" Header="Bundler host:" PlaceholderText="localhost"/>
    <TextBox x:Name="Port" Header="Bundler port:" PlaceholderText="8081" Margin="0,4,0,0"/>
    <TextBox x:Name="EntryPoint" Header="Entrypoint:" PlaceholderText="index.windows" Margin="0,4,0,0"/>
    <StackPanel Orientation="Horizontal" HorizontalAlignment="Right" Margin="{ThemeResource ContentDialogCommandSpaceMargin}">
      <Button x:Name="Apply" Style="{StaticResource AccentButtonStyle}" Content="Apply Changes" />
      <Button x:Name="Reset" Content="Reset to Default" Margin="4,0,0,0"/>
      <Button x:Name="Cancel" Content="Cancel" Margin="4,0,0,0"/>
    </StackPanel>
  </StackPanel>)";
      auto configureUI = winrt::unbox_value<xaml::Controls::StackPanel>(xaml::Markup::XamlReader::Load(xamlString));

      auto options = reactHost->Options();

      auto host = configureUI.FindName(L"Host").as<xaml::Controls::TextBox>();

      host.PlaceholderText(
          Microsoft::Common::Unicode::Utf8ToUtf16(facebook::react::DevServerHelper::DefaultPackagerHost));
      host.Text(Microsoft::Common::Unicode::Utf8ToUtf16(options.DeveloperSettings.SourceBundleHost));

      auto port = configureUI.FindName(L"Port").as<xaml::Controls::TextBox>();
      port.PlaceholderText(std::to_wstring(facebook::react::DevServerHelper::DefaultPackagerPort));
      if (options.DeveloperSettings.SourceBundlePort)
        port.Text(std::to_wstring(options.DeveloperSettings.SourceBundlePort));

      auto entry = configureUI.FindName(L"EntryPoint").as<xaml::Controls::TextBox>();
      entry.PlaceholderText(L"index.windows");
      entry.Text(Microsoft::Common::Unicode::Utf8ToUtf16(options.DeveloperSettings.SourceBundleName));

      auto applyButton = configureUI.FindName(L"Apply").as<xaml::Controls::Button>();
      applyButton.Click([this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
          auto options = reactHost->Options();
          options.DeveloperSettings.SourceBundleHost =
              Microsoft::Common::Unicode::Utf16ToUtf8(m_flyout.Content()
                                                          .as<xaml::FrameworkElement>()
                                                          .FindName(L"Host")
                                                          .as<xaml::Controls::TextBox>()
                                                          .Text()
                                                          .c_str());

          auto portText =
              m_flyout.Content().as<xaml::FrameworkElement>().FindName(L"Port").as<xaml::Controls::TextBox>().Text();
          options.DeveloperSettings.SourceBundlePort = portText.empty()
              ? facebook::react::DevServerHelper::DefaultPackagerPort
              : static_cast<uint16_t>(std::stoi(std::wstring(portText)));
          options.DeveloperSettings.SourceBundleName =
              Microsoft::Common::Unicode::Utf16ToUtf8(m_flyout.Content()
                                                          .as<xaml::FrameworkElement>()
                                                          .FindName(L"EntryPoint")
                                                          .as<xaml::Controls::TextBox>()
                                                          .Text()
                                                          .c_str());
          Hide();
          reactHost->ReloadInstanceWithOptions(std::move(options));
        }
      });

      auto resetButton = configureUI.FindName(L"Reset").as<xaml::Controls::Button>();
      resetButton.Click([this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
          m_flyout.Content().as<xaml::FrameworkElement>().FindName(L"Host").as<xaml::Controls::TextBox>().Text(L"");
          m_flyout.Content().as<xaml::FrameworkElement>().FindName(L"Port").as<xaml::Controls::TextBox>().Text(L"");
          m_flyout.Content()
              .as<xaml::FrameworkElement>()
              .FindName(L"EntryPoint")
              .as<xaml::Controls::TextBox>()
              .Text(L"");
        }
      });

      auto cancelButton = configureUI.FindName(L"Cancel").as<xaml::Controls::Button>();
      cancelButton.Click([this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept { Hide(); });

      m_flyout = xaml::Controls::Flyout{};
      m_flyout.Content(configureUI);
      if (react::uwp::Is19H1OrHigher()) {
        // ShouldConstrainToRootBounds added in 19H1
        m_flyout.ShouldConstrainToRootBounds(false);
      }

      configureUI.Tag(*this);
      m_flyout.Closing([](xaml::Controls::Primitives::FlyoutBase const &sender,
                          xaml::Controls::Primitives::FlyoutBaseClosingEventArgs const & /*args*/) noexcept {
        // Remove the tag to disconnect the ref cycle
        sender.as<xaml::Controls::Flyout>().Content().as<xaml::FrameworkElement>().Tag(nullptr);
      });

      xaml::UIElement root{nullptr};
      auto xamlRoot = winrt::Microsoft::ReactNative::XamlUIService::GetXamlRoot(reactHost->Options().Properties);
      if (xamlRoot) {
        m_flyout.XamlRoot(xamlRoot);
        root = xamlRoot.Content();
      } else {
        auto window = xaml::Window::Current();
        root = window.Content();
      }

      m_flyout.LightDismissOverlayMode(xaml::Controls::LightDismissOverlayMode::On);
      configureUI.XYFocusKeyboardNavigation(xaml::Input::XYFocusKeyboardNavigationMode::Enabled);

      auto style = xaml::Style(winrt::xaml_typename<xaml::Controls::FlyoutPresenter>());
      style.Setters().Append(xaml::Setter(
          xaml::Controls::ScrollViewer::HorizontalScrollBarVisibilityProperty(),
          winrt::box_value(xaml::Controls::ScrollBarVisibility::Disabled)));
      style.Setters().Append(xaml::Setter(
          xaml::Controls::ScrollViewer::VerticalScrollBarVisibilityProperty(),
          winrt::box_value(xaml::Controls::ScrollBarVisibility::Disabled)));
      style.Setters().Append(xaml::Setter(
          xaml::Controls::ScrollViewer::HorizontalScrollModeProperty(),
          winrt::box_value(xaml::Controls::ScrollMode::Disabled)));
      style.Setters().Append(xaml::Setter(
          xaml::Controls::ScrollViewer::VerticalScrollModeProperty(),
          winrt::box_value(xaml::Controls::ScrollMode::Disabled)));

      m_flyout.FlyoutPresenterStyle(style);
      m_flyout.ShowAt(root.as<xaml::FrameworkElement>());
    }
  }
  void Hide() noexcept {
    if (!m_flyout)
      return;
    auto localFlyout = m_flyout;
    m_flyout = nullptr;
    localFlyout.Hide();
  }

  const Mso::WeakPtr<Mso::React::IReactHost> m_weakReactHost;

  xaml::Controls::Flyout m_flyout{nullptr};
};

void ShowConfigureBundlerDialog(Mso::WeakPtr<Mso::React::IReactHost> weakReactHost) {
  winrt::make<ConfigureBundlerDlg>(weakReactHost);
}

} // namespace Microsoft::ReactNative
