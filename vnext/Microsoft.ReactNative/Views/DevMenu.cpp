// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DevMenu.h"
#include "Modules/DevSettingsModule.h"
#include "UI.Xaml.Controls.Primitives.h"
#include "UI.Xaml.Controls.h"
#include "UI.Xaml.Input.h"
#include "UI.Xaml.Interop.h"
#include "UI.Xaml.Markup.h"
#include "Utils/Helpers.h"
#include "IReactDispatcher.h"
#include "winrt/Windows.UI.Core.h"
#include "Views/KeyboardEventHandler.h"

namespace Microsoft::ReactNative {

React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DevMenuManager>>> DevMenuManagerProperty() noexcept
{
  static React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DevMenuManager>>> propId {
      L"ReactNative.DevMenuManager", L"DevMenuManager" };
  return propId;
}

/*static*/ void DevMenuManager::InitDevMenu(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept
{
  auto devMenu = std::make_shared<DevMenuManager>(reactContext);
  devMenu->Init();
  React::ReactPropertyBag(reactContext->Properties()).Set(DevMenuManagerProperty(), devMenu);
}

void DevMenuManager::Init() noexcept {
  auto uiDispatcher = React::implementation::ReactDispatcher::GetUIDispatcher(m_context->Properties());
  uiDispatcher.Post([weakThis = weak_from_this()]() {
    if (auto pThis = weakThis.lock()) {
      auto context = pThis->m_context;

      if (react::uwp::Is19H1OrHigher()) {
        // XamlRoot added in 19H1
        if (auto xamlRoot = React::XamlUIService::GetXamlRoot(pThis->m_context->Properties())) {
          if (auto rootContent = xamlRoot.Content()) {
            pThis->m_keyDownRevoker = rootContent.KeyDown(
                winrt::auto_revoke, [context](const auto & /*sender*/, const xaml::Input::KeyRoutedEventArgs args) {
                  if (args.Key() == winrt::Windows::System::VirtualKey::D &&
                      react::uwp::KeyboardHelper::IsModifiedKeyPressed(
                          winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Shift) &&
                      react::uwp::KeyboardHelper::IsModifiedKeyPressed(
                          winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Control)) {
                    DevMenuManager::Show(context->Properties());
                  }
                });
            return;
          }
        }
      }

      auto coreWindow = winrt::CoreWindow::GetForCurrentThread();
      pThis->m_coreDispatcherAKARevoker = coreWindow.Dispatcher().AcceleratorKeyActivated(
          winrt::auto_revoke, [context](const auto & /*sender*/, const winrt::AcceleratorKeyEventArgs &args) {
            if ((args.VirtualKey() == winrt::Windows::System::VirtualKey::D) &&
                react::uwp::KeyboardHelper::IsModifiedKeyPressed(
                    winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Shift) &&
                react::uwp::KeyboardHelper::IsModifiedKeyPressed(
                    winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Control)) {
              DevMenuManager::Show(context->Properties());
            }
          });
    }
  });
}

/*static*/ void DevMenuManager::Show(React::IReactPropertyBag const &properties) noexcept {
  (*React::ReactPropertyBag(properties).Get(DevMenuManagerProperty()))->CreateAndShowUI();
}

/*static*/ void DevMenuManager::Hide(React::IReactPropertyBag const &properties) noexcept {
  (*React::ReactPropertyBag(properties).Get(DevMenuManagerProperty()))->Hide();
}

DevMenuManager::DevMenuManager(Mso::CntPtr<Mso::React::IReactContext> const &reactContext)
    : m_context(reactContext)
{
}

void DevMenuManager::CreateAndShowUI() noexcept {
  const winrt::hstring xamlString =
      L"  <StackPanel"
      L"    xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
      L"    xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'"
      L"    Background='{ThemeResource ContentDialogBackground}'"
      L"    HorizontalAlignment='Center'"
      L"    MaxWidth='900'"
      L"    Margin='-10'"
      L"  >"
      L"  <StackPanel.Resources>"
      L"    <StaticResource x:Key='ButtonRevealBackground' ResourceKey='AppBarButtonRevealBackground' />"
      L"    <StaticResource x:Key='ButtonRevealBackgroundPointerOver' ResourceKey='AppBarButtonRevealBackgroundPointerOver' />"
      L"    <StaticResource x:Key='ButtonRevealBackgroundPressed' ResourceKey='AppBarButtonRevealBackgroundPressed' />"
      L"    <StaticResource x:Key='ButtonRevealBackgroundDisabled' ResourceKey='AppBarButtonRevealBackgroundDisabled' />"
      L"    <StaticResource x:Key='ButtonForeground' ResourceKey='AppBarButtonForeground' />"
      L"    <StaticResource x:Key='ButtonForegroundPointerOver' ResourceKey='AppBarButtonForegroundPointerOver' />"
      L"    <StaticResource x:Key='ButtonForegroundPressed' ResourceKey='AppBarButtonForegroundPressed' />"
      L"    <StaticResource x:Key='ButtonForegroundDisabled' ResourceKey='AppBarButtonForegroundDisabled' />"
      L"    <StaticResource x:Key='ButtonRevealBorderBrush' ResourceKey='AppBarButtonRevealBorderBrush' />"
      L"    <StaticResource x:Key='ButtonRevealBorderBrushPointerOver' ResourceKey='AppBarButtonRevealBorderBrushPointerOver' />"
      L"    <StaticResource x:Key='ButtonRevealBorderBrushPressed' ResourceKey='AppBarButtonRevealBorderBrushPressed' />"
      L"    <StaticResource x:Key='ButtonRevealBorderBrushDisabled' ResourceKey='AppBarButtonRevealBorderBrushDisabled' />"
      L"  </StackPanel.Resources>"
      L"  <StackPanel.Transitions>"
      L"    <TransitionCollection>"
      L"      <EntranceThemeTransition />"
      L"    </TransitionCollection>"
      L"  </StackPanel.Transitions>"
      L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='Reload' Style='{StaticResource ButtonRevealStyle}'>"
      L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
      L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE72C;'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='0'>Reload Javascript</TextBlock>"
      L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>Restarts the JS instance. Any javascript state will be lost.</TextBlock>"
      L"      </Grid>"
      L"    </Button>"
      L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='RemoteDebug' Style='{StaticResource ButtonRevealStyle}'>"
      L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
      L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE8AF;'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='RemoteDebugText'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>When enabled runs the JS remotely in VSCode or Chrome based on what you attach to the packager.  This means that the JS may run with a different JS engine than it runs in on in the real application, in addition synchronous native module calls, and JSI native modules will not work.</TextBlock>"
      L"      </Grid>"
      L"    </Button>"
      L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='DirectDebug' Style='{StaticResource ButtonRevealStyle}'>"
      L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
      L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xEBE8;'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='DirectDebugText'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>If using Chakra, this will allow Visual Studio to be attached directly to the application using \"Script Debugging\" to debug the JS running directly in this app.\nIf using V8/Hermes, this will enable standard JS debugging tools such as VSCode to attach to the application.</TextBlock>"
      L"      </Grid>"
      L"    </Button>"
      L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='BreakOnNextLine' Style='{StaticResource ButtonRevealStyle}'>"
      L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
      L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE769;'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='BreakOnNextLineText'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>If using V8/Hermes, the JS engine will break on the first statement, until you attach a debugger to it, and hit continue. (Requires Direct Debugging to be enabled)</TextBlock>"
      L"      </Grid>"
      L"    </Button>"
      L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='FastRefresh' Style='{StaticResource ButtonRevealStyle}'>"
      L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
      L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xEC58;'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='FastRefreshText'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>When loading a bundle from a bundler server that is watching files, this will cause the instance to be reloaded with new bundles when a file changes.</TextBlock>"
      L"      </Grid>"
      L"    </Button>"
      L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='Inspector' Style='{StaticResource ButtonRevealStyle}'>"
      L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
      L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE773;'/>"
      L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='InspectorText'>Toggle Inspector</TextBlock>"
      L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>Will bring up an overlay that lets you tap on any UI element and see information about it</TextBlock>"
      L"      </Grid>"
      L"    </Button>"
      L"    <Button HorizontalAlignment='Stretch' x:Name='Cancel' Style='{StaticResource ButtonRevealStyle}'>Cancel</Button>"
      L"  </StackPanel>";
  auto devMenu = winrt::unbox_value<xaml::Controls::Panel>(xaml::Markup::XamlReader::Load(xamlString));

  auto remoteDebugJSText = devMenu.FindName(L"RemoteDebugText").as<xaml::Controls::TextBlock>();
  remoteDebugJSText.Text(Mso::React::ReactOptions::UseWebDebugger(m_context->Properties()) ? L"Disable Remote JS Debugging" : L"Enable Remote JS Debugging");

  auto fastRefreshText = devMenu.FindName(L"FastRefreshText").as<xaml::Controls::TextBlock>();
  fastRefreshText.Text(
      Mso::React::ReactOptions::UseFastRefresh(m_context->Properties())
          ? L"Disable Fast Refresh"
          : L"Enable Fast Refresh");

  auto directDebugText = devMenu.FindName(L"DirectDebugText").as<xaml::Controls::TextBlock>();
  directDebugText.Text(
      Mso::React::ReactOptions::UseDirectDebugger(m_context->Properties())
          ? L"Disable Direct Debugging"
          : L"Enable Direct Debugging");

  auto breakOnNextLineText = devMenu.FindName(L"BreakOnNextLineText").as<xaml::Controls::TextBlock>();
  breakOnNextLineText.Text(
      Mso::React::ReactOptions::DebuggerBreakOnNextLine(m_context->Properties())
          ? L"Disable Break on First Line"
          : L"Enable Break on First Line");

  auto reloadJSButton = devMenu.FindName(L"Reload").as<xaml::Controls::Button>();
  m_reloadJSRevoker = reloadJSButton.Click(
      winrt::auto_revoke, [this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        Hide();
        DevSettings::Reload(React::ReactPropertyBag(m_context->Properties()));
      });

  auto remoteDebugJSButton = devMenu.FindName(L"RemoteDebug").as<xaml::Controls::Button>();
  m_remoteDebugJSRevoker = remoteDebugJSButton.Click(
      winrt::auto_revoke, [this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        Hide();
        Mso::React::ReactOptions::SetUseWebDebugger(
            m_context->Properties(), !Mso::React::ReactOptions::UseWebDebugger(m_context->Properties()));
        DevSettings::Reload(React::ReactPropertyBag(m_context->Properties()));
      });

  auto directDebugButton = devMenu.FindName(L"DirectDebug").as<xaml::Controls::Button>();
  m_directDebuggingRevoker = directDebugButton.Click(
      winrt::auto_revoke, [this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        Hide();
        Mso::React::ReactOptions::SetUseDirectDebugger(
            m_context->Properties(), !Mso::React::ReactOptions::UseDirectDebugger(m_context->Properties()));
        DevSettings::Reload(React::ReactPropertyBag(m_context->Properties()));
      });

  auto breakOnNextLineButton = devMenu.FindName(L"BreakOnNextLine").as<xaml::Controls::Button>();
  m_breakOnNextLineRevoker = breakOnNextLineButton.Click(
        winrt::auto_revoke, [this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        Hide();
        Mso::React::ReactOptions::SetDebuggerBreakOnNextLine(m_context->Properties(), !Mso::React::ReactOptions::DebuggerBreakOnNextLine(m_context->Properties()));
        DevSettings::Reload(React::ReactPropertyBag(m_context->Properties()));
      });

  auto fastRefreshButton = devMenu.FindName(L"FastRefresh").as<xaml::Controls::Button>();
  m_fastRefreshRevoker = fastRefreshButton.Click(
      winrt::auto_revoke, [this](auto & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        Hide();
        Mso::React::ReactOptions::SetUseFastRefresh(
            m_context->Properties(), !Mso::React::ReactOptions::UseFastRefresh(m_context->Properties()));
        DevSettings::Reload(React::ReactPropertyBag(m_context->Properties()));
      });

  auto toggleInspector = devMenu.FindName(L"Inspector").as<xaml::Controls::Button>();
  m_toggleInspectorRevoker = toggleInspector.Click(
      winrt::auto_revoke, [this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        Hide();
        DevSettings::ToggleElementInspector(m_context);
      });

  auto cancelButton = devMenu.FindName(L"Cancel").as<xaml::Controls::Button>();
  m_cancelRevoker = cancelButton.Click(
      winrt::auto_revoke,
      [this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) { Hide(); });

  m_flyout = xaml::Controls::Flyout{};
  m_flyout.Content(devMenu);
  if (react::uwp::Is19H1OrHigher()) {
    // ShouldConstrainToRootBounds added in 19H1
    m_flyout.ShouldConstrainToRootBounds(false);
  }

  xaml::UIElement root{nullptr};
  auto xamlRoot = React::XamlUIService::GetXamlRoot(m_context->Properties());
  if (xamlRoot) {
    m_flyout.XamlRoot(xamlRoot);
    root = xamlRoot.Content();
  } else {
    auto window = xaml::Window::Current();
    root = window.Content();
  }

  m_flyout.LightDismissOverlayMode(xaml::Controls::LightDismissOverlayMode::On);
  devMenu.XYFocusKeyboardNavigation(xaml::Input::XYFocusKeyboardNavigationMode::Enabled);

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

  // Notify instance that dev menu is shown -- This is used to trigger a connection to dev tools
  m_context->CallJSFunction(
        "RCTNativeAppEventEmitter", "emit", folly::dynamic::array("RCTDevMenuShown"));
  }

void DevMenuManager::Hide() noexcept
{
    if (!m_flyout)
      return;
    m_flyout.Hide();
    m_flyout = nullptr;
  }

} // namespace Microsoft::ReactNative
