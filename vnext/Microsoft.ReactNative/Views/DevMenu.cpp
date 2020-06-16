// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DevMenu.h"
#include "IReactDispatcher.h"
#include "Modules/DevSettingsModule.h"
#include "UI.Xaml.Controls.Primitives.h"
#include "UI.Xaml.Controls.h"
#include "UI.Xaml.Input.h"
#include "UI.Xaml.Markup.h"
#include "Utils/Helpers.h"
#include "Views/KeyboardEventHandler.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.UI.Xaml.Interop.h"

namespace Microsoft::ReactNative {

React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DevMenuManager>>> DevMenuManagerProperty() noexcept {
  static React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<DevMenuManager>>> propId{
      L"ReactNative.DevMenuManager", L"DevMenuManager"};
  return propId;
}

/*static*/ void DevMenuManager::InitDevMenu(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  auto devMenu = std::make_shared<DevMenuManager>(reactContext);
  devMenu->Init();
  React::ReactPropertyBag(reactContext->Properties()).Set(DevMenuManagerProperty(), devMenu);
}

bool IsCtrlShiftD(winrt::Windows::System::VirtualKey key) noexcept {
  return (
      key == winrt::Windows::System::VirtualKey::D &&
      react::uwp::KeyboardHelper::IsModifiedKeyPressed(
          winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Shift) &&
      react::uwp::KeyboardHelper::IsModifiedKeyPressed(
          winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Control));
}

void DevMenuManager::Init() noexcept {
  auto uiDispatcher = React::implementation::ReactDispatcher::GetUIDispatcher(m_context->Properties());
  uiDispatcher.Post([weakThis = weak_from_this()]() {
    if (auto strongThis = weakThis.lock()) {
      auto context = strongThis->m_context;

      if (react::uwp::Is19H1OrHigher()) {
        // XamlRoot added in 19H1
        if (auto xamlRoot = React::XamlUIService::GetXamlRoot(strongThis->m_context->Properties())) {
          if (auto rootContent = xamlRoot.Content()) {
            strongThis->m_keyDownRevoker = rootContent.KeyDown(
                winrt::auto_revoke, [context](const auto & /*sender*/, const xaml::Input::KeyRoutedEventArgs &args) {
                  if (IsCtrlShiftD(args.Key())) {
                    DevMenuManager::Show(context->Properties());
                  }
                });
            return;
          }
        }
      }

      if (auto coreWindow = winrt::CoreWindow::GetForCurrentThread()) {
        strongThis->m_coreDispatcherAKARevoker = coreWindow.Dispatcher().AcceleratorKeyActivated(
            winrt::auto_revoke, [context](const auto & /*sender*/, const winrt::AcceleratorKeyEventArgs &args) {
              if (IsCtrlShiftD(args.VirtualKey())) {
                DevMenuManager::Show(context->Properties());
              }
            });
      }
    }
  });
}

/*static*/ void DevMenuManager::Show(React::IReactPropertyBag const &properties) noexcept {
  (*React::ReactPropertyBag(properties).Get(DevMenuManagerProperty()))->CreateAndShowUI();
}

/*static*/ void DevMenuManager::Hide(React::IReactPropertyBag const &properties) noexcept {
  (*React::ReactPropertyBag(properties).Get(DevMenuManagerProperty()))->Hide();
}

DevMenuManager::DevMenuManager(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) : m_context(reactContext) {}

void DevMenuManager::CreateAndShowUI() noexcept {
  if (!Mso::React::ReactOptions::UseDeveloperSupport(m_context->Properties()))
    return;

  const winrt::hstring xamlString =
      LR"(
  <StackPanel
    xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'
    xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'
    Background='{ThemeResource ContentDialogBackground}'
    HorizontalAlignment='Center'
    MaxWidth='900'
    Margin='-10'
  >
  <StackPanel.Resources>
    <StaticResource x:Key='ButtonRevealBackground' ResourceKey='AppBarButtonRevealBackground' />
    <StaticResource x:Key='ButtonRevealBackgroundPointerOver' ResourceKey='AppBarButtonRevealBackgroundPointerOver' />
    <StaticResource x:Key='ButtonRevealBackgroundPressed' ResourceKey='AppBarButtonRevealBackgroundPressed' />
    <StaticResource x:Key='ButtonRevealBackgroundDisabled' ResourceKey='AppBarButtonRevealBackgroundDisabled' />
    <StaticResource x:Key='ButtonForeground' ResourceKey='AppBarButtonForeground' />
    <StaticResource x:Key='ButtonForegroundPointerOver' ResourceKey='AppBarButtonForegroundPointerOver' />
    <StaticResource x:Key='ButtonForegroundPressed' ResourceKey='AppBarButtonForegroundPressed' />
    <StaticResource x:Key='ButtonForegroundDisabled' ResourceKey='AppBarButtonForegroundDisabled' />
    <StaticResource x:Key='ButtonRevealBorderBrush' ResourceKey='AppBarButtonRevealBorderBrush' />
    <StaticResource x:Key='ButtonRevealBorderBrushPointerOver' ResourceKey='AppBarButtonRevealBorderBrushPointerOver' />
    <StaticResource x:Key='ButtonRevealBorderBrushPressed' ResourceKey='AppBarButtonRevealBorderBrushPressed' />
    <StaticResource x:Key='ButtonRevealBorderBrushDisabled' ResourceKey='AppBarButtonRevealBorderBrushDisabled' />
  </StackPanel.Resources>
  <StackPanel.Transitions>
    <TransitionCollection>
      <EntranceThemeTransition />
    </TransitionCollection>
  </StackPanel.Transitions>
    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='Reload' Style='{StaticResource ButtonRevealStyle}'>
      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>
        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE72C;'/>
        <TextBlock Grid.Column='1' Grid.Row='0'>Reload Javascript</TextBlock>
        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>Restarts the JS instance. Any javascript state will be lost.</TextBlock>
      </Grid>
    </Button>
    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='RemoteDebug' Style='{StaticResource ButtonRevealStyle}'>
      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>
        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE8AF;'/>
        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='RemoteDebugText'/>
        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>When enabled runs the JS remotely in VSCode or Chrome based on what you attach to the packager.  This means that the JS may run with a different JS engine than it runs in on in the real application, in addition synchronous native module calls, and JSI native modules will not work.</TextBlock>
      </Grid>
    </Button>
    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='DirectDebug' Style='{StaticResource ButtonRevealStyle}'>
      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>
        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xEBE8;'/>
        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='DirectDebugText'/>
        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>If using Chakra, this will allow Visual Studio to be attached directly to the application using \"Script Debugging\" to debug the JS running directly in this app.\nIf using V8/Hermes, this will enable standard JS debugging tools such as VSCode to attach to the application.</TextBlock>
      </Grid>
    </Button>
    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='BreakOnNextLine' Style='{StaticResource ButtonRevealStyle}'>
      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>
        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE769;'/>
        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='BreakOnNextLineText'/>
        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>If using V8/Hermes, the JS engine will break on the first statement, until you attach a debugger to it, and hit continue. (Requires Direct Debugging to be enabled)</TextBlock>
      </Grid>
    </Button>
    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='FastRefresh' Style='{StaticResource ButtonRevealStyle}'>
      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>
        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xEC58;'/>
        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='FastRefreshText'/>
        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>When loading a bundle from a bundler server that is watching files, this will cause the instance to be reloaded with new bundles when a file changes.</TextBlock>
      </Grid>
    </Button>
    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='Inspector' Style='{StaticResource ButtonRevealStyle}'>
      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>
        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE773;'/>
        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='InspectorText'>Toggle Inspector</TextBlock>
        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>Will bring up an overlay that lets you tap on any UI element and see information about it</TextBlock>
      </Grid>
    </Button>
    <Button HorizontalAlignment='Stretch' x:Name='Cancel' Style='{StaticResource ButtonRevealStyle}'>Cancel</Button>
  </StackPanel>)";
  auto devMenu = winrt::unbox_value<xaml::Controls::Panel>(xaml::Markup::XamlReader::Load(xamlString));

  auto remoteDebugJSText = devMenu.FindName(L"RemoteDebugText").as<xaml::Controls::TextBlock>();
  remoteDebugJSText.Text(
      Mso::React::ReactOptions::UseWebDebugger(m_context->Properties()) ? L"Disable Remote JS Debugging"
                                                                        : L"Enable Remote JS Debugging");

  auto fastRefreshText = devMenu.FindName(L"FastRefreshText").as<xaml::Controls::TextBlock>();
  fastRefreshText.Text(
      Mso::React::ReactOptions::UseFastRefresh(m_context->Properties()) ? L"Disable Fast Refresh"
                                                                        : L"Enable Fast Refresh");

  auto directDebugText = devMenu.FindName(L"DirectDebugText").as<xaml::Controls::TextBlock>();
  directDebugText.Text(
      Mso::React::ReactOptions::UseDirectDebugger(m_context->Properties()) ? L"Disable Direct Debugging"
                                                                           : L"Enable Direct Debugging");

  auto breakOnNextLineText = devMenu.FindName(L"BreakOnNextLineText").as<xaml::Controls::TextBlock>();
  breakOnNextLineText.Text(
      Mso::React::ReactOptions::DebuggerBreakOnNextLine(m_context->Properties()) ? L"Disable Break on First Line"
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
        Mso::React::ReactOptions::SetDebuggerBreakOnNextLine(
            m_context->Properties(), !Mso::React::ReactOptions::DebuggerBreakOnNextLine(m_context->Properties()));
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
        DevSettings::ToggleElementInspector(*m_context);
      });

  auto cancelButton = devMenu.FindName(L"Cancel").as<xaml::Controls::Button>();
  m_cancelRevoker = cancelButton.Click(
      winrt::auto_revoke, [this](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) { Hide(); });

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
  m_context->CallJSFunction("RCTNativeAppEventEmitter", "emit", folly::dynamic::array("RCTDevMenuShown"));
}

void DevMenuManager::Hide() noexcept {
  if (!m_flyout)
    return;
  m_flyout.Hide();
  m_flyout = nullptr;
}

} // namespace Microsoft::ReactNative
