// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "DevMenu.h"
#include "DevMenuControl.h"
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

React::ReactPropertyId<React::ReactNonAbiValue<Mso::VoidFunctor>> ConfigureBundlerProperty() noexcept {
  static React::ReactPropertyId<React::ReactNonAbiValue<Mso::VoidFunctor>> propId{
      L"ReactNative.DevMenuManager", L"ConfigureBundler"};
  return propId;
}

/*static*/ void DevMenuManager::InitDevMenu(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
    Mso::VoidFunctor &&configureBundler) noexcept {
  auto devMenu = std::make_shared<DevMenuManager>(reactContext);
  devMenu->Init();
  React::ReactPropertyBag(reactContext->Properties()).Set(DevMenuManagerProperty(), devMenu);
  React::ReactPropertyBag(reactContext->Properties()).Set(ConfigureBundlerProperty(), std::move(configureBundler));
}

bool IsCtrlShiftD(winrt::Windows::System::VirtualKey key) noexcept {
  return (
      key == winrt::Windows::System::VirtualKey::D &&
      KeyboardHelper::IsModifiedKeyPressed(
          winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Shift) &&
      KeyboardHelper::IsModifiedKeyPressed(
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

  winrt::Microsoft::ReactNative::DevMenuControl devMenu{};

  devMenu.RemoteDebugText().Text(
      Mso::React::ReactOptions::UseWebDebugger(m_context->Properties()) ? L"Disable Remote JS Debugging"
                                                                        : L"Enable Remote JS Debugging");
  devMenu.RemoteDebugDesc().Text(
      L"When enabled runs the JS remotely in VSCode or Chrome based on what you attach to the packager.  This means that the JS may run with a different JS engine than it runs in on in the real application, in addition synchronous native module calls, and JSI native modules will not work.");

  devMenu.FastRefreshText().Text(
      Mso::React::ReactOptions::UseFastRefresh(m_context->Properties()) ? L"Disable Fast Refresh"
                                                                        : L"Enable Fast Refresh");

  devMenu.DirectDebugText().Text(
      Mso::React::ReactOptions::UseDirectDebugger(m_context->Properties()) ? L"Disable Direct Debugging"
                                                                           : L"Enable Direct Debugging");
  devMenu.DirectDebugDesc().Text(
      L"If using Chakra, this will allow Visual Studio to be attached directly to the application using \"Script Debugging\" to debug the JS running directly in this app.\nIf using V8/Hermes, this will enable standard JS debugging tools such as VSCode to attach to the application.");

  devMenu.BreakOnNextLineText().Text(
      Mso::React::ReactOptions::DebuggerBreakOnNextLine(m_context->Properties()) ? L"Disable Break on First Line"
                                                                                 : L"Enable Break on First Line");

  m_reloadJSRevoker = devMenu.Reload().Click(
      winrt::auto_revoke,
      [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto strongThis = wkThis.lock()) {
          strongThis->Hide();
          DevSettings::Reload(React::ReactPropertyBag(strongThis->m_context->Properties()));
        }
      });

  m_remoteDebugJSRevoker = devMenu.RemoteDebug().Click(
      winrt::auto_revoke,
      [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto strongThis = wkThis.lock()) {
          strongThis->Hide();
          Mso::React::ReactOptions::SetUseWebDebugger(
              strongThis->m_context->Properties(),
              !Mso::React::ReactOptions::UseWebDebugger(strongThis->m_context->Properties()));
          DevSettings::Reload(React::ReactPropertyBag(strongThis->m_context->Properties()));
        }
      });

  m_directDebuggingRevoker = devMenu.DirectDebug().Click(
      winrt::auto_revoke,
      [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto strongThis = wkThis.lock()) {
          strongThis->Hide();
          Mso::React::ReactOptions::SetUseDirectDebugger(
              strongThis->m_context->Properties(),
              !Mso::React::ReactOptions::UseDirectDebugger(strongThis->m_context->Properties()));
          DevSettings::Reload(React::ReactPropertyBag(strongThis->m_context->Properties()));
        }
      });

  m_breakOnNextLineRevoker = devMenu.BreakOnNextLine().Click(
      winrt::auto_revoke,
      [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto strongThis = wkThis.lock()) {
          strongThis->Hide();
          Mso::React::ReactOptions::SetDebuggerBreakOnNextLine(
              strongThis->m_context->Properties(),
              !Mso::React::ReactOptions::DebuggerBreakOnNextLine(strongThis->m_context->Properties()));
          DevSettings::Reload(React::ReactPropertyBag(strongThis->m_context->Properties()));
        }
      });

  m_fastRefreshRevoker = devMenu.FastRefresh().Click(
      winrt::auto_revoke,
      [wkThis = weak_from_this()](auto & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto strongThis = wkThis.lock()) {
          strongThis->Hide();
          Mso::React::ReactOptions::SetUseFastRefresh(
              strongThis->m_context->Properties(),
              !Mso::React::ReactOptions::UseFastRefresh(strongThis->m_context->Properties()));
          DevSettings::Reload(React::ReactPropertyBag(strongThis->m_context->Properties()));
        }
      });

  m_toggleInspectorRevoker = devMenu.Inspector().Click(
      winrt::auto_revoke,
      [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto strongThis = wkThis.lock()) {
          strongThis->Hide();
          DevSettings::ToggleElementInspector(*(strongThis->m_context));
        }
      });

  m_configBundlerRevoker = devMenu.ConfigBundler().Click(
      winrt::auto_revoke,
      [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
        if (auto strongThis = wkThis.lock()) {
          strongThis->Hide();
          React::ReactPropertyBag(strongThis->m_context->Properties()).Get(ConfigureBundlerProperty())();
        }
      });
  // Only show Configure Bundler when connected to a bundler
  devMenu.ConfigBundler().Visibility(
      (Mso::React::ReactOptions::UseFastRefresh(m_context->Properties()) ||
       Mso::React::ReactOptions::UseWebDebugger(m_context->Properties()))
          ? xaml::Visibility::Visible
          : xaml::Visibility::Collapsed);

  m_cancelRevoker = devMenu.Cancel().Click(
      winrt::auto_revoke, [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) {
        if (auto strongThis = wkThis.lock()) {
          strongThis->Hide();
        }
      });

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
