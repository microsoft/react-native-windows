// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DevMenu.h"

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "HermesSamplingProfiler.h"
#include "IReactDispatcher.h"
#include "Modules/DevSettingsModule.h"

#ifndef CORE_ABI
#include <XamlUtils.h>
#include "DevMenuControl.h"
#include "UI.Xaml.Controls.Primitives.h"
#include "UI.Xaml.Controls.h"
#include "UI.Xaml.Input.h"
#include "UI.Xaml.Markup.h"
#include "Utils/Helpers.h"
#include "Utils/KeyboardUtils.h"
#include "winrt/Windows.UI.Core.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#endif // CORE_ABI

#if defined(CORE_ABI) || defined(USE_FABRIC)
#include <ReactCoreInjection.h>
#include <Shobjidl.h>
#include "winrt/Windows.UI.Popups.h"
#endif

using namespace winrt::Windows::ApplicationModel;

namespace Microsoft::ReactNative {

struct IDevMenu {
  virtual void Show() noexcept = 0;
};

#ifndef CORE_ABI
bool IsCtrlShiftD(winrt::Windows::System::VirtualKey key) noexcept {
  return (
      key == winrt::Windows::System::VirtualKey::D &&
      IsModifiedKeyPressed(winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Shift) &&
      IsModifiedKeyPressed(winrt::CoreWindow::GetForCurrentThread(), winrt::Windows::System::VirtualKey::Control));
}
#endif // CORE_ABI

React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<IDevMenu>>> DevMenuImplProperty() noexcept {
  static React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<IDevMenu>>> propId{
      L"ReactNative.DevMenuManager", L"DevMenuImpl"};
  return propId;
}

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

void ToggleDirectDebugger(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  Mso::React::ReactOptions::SetUseDirectDebugger(
      reactContext->Properties(), !Mso::React::ReactOptions::UseDirectDebugger(reactContext->Properties()));
  DevSettings::Reload(React::ReactPropertyBag(reactContext->Properties()));
}

const wchar_t *DirectDebuggerLabel(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  return Mso::React::ReactOptions::UseDirectDebugger(reactContext->Properties()) ? L"Disable Direct Debugging"
                                                                                 : L"Enable Direct Debugging";
}

void ToggleBreakOnNextLine(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  Mso::React::ReactOptions::SetDebuggerBreakOnNextLine(
      reactContext->Properties(), !Mso::React::ReactOptions::DebuggerBreakOnNextLine(reactContext->Properties()));
  DevSettings::Reload(React::ReactPropertyBag(reactContext->Properties()));
}

const wchar_t *BreakOnNextLineLabel(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  return Mso::React::ReactOptions::DebuggerBreakOnNextLine(reactContext->Properties()) ? L"Disable Break on First Line"
                                                                                       : L"Enable Break on First Line";
}

void ToggleFastRefresh(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  Mso::React::ReactOptions::SetUseFastRefresh(
      reactContext->Properties(), !Mso::React::ReactOptions::UseFastRefresh(reactContext->Properties()));
  DevSettings::Reload(React::ReactPropertyBag(reactContext->Properties()));
}

winrt::fire_and_forget ToggleHermesProfiler(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  if (!Microsoft::ReactNative::HermesSamplingProfiler::IsStarted()) {
    Microsoft::ReactNative::HermesSamplingProfiler::Start(reactContext);
  } else {
    auto traceFilePath = co_await Microsoft::ReactNative::HermesSamplingProfiler::Stop(reactContext);
    auto uiDispatcher = React::implementation::ReactDispatcher::GetUIDispatcher(reactContext->Properties());
    uiDispatcher.Post([traceFilePath]() {
      DataTransfer::DataPackage data;
      data.SetText(winrt::to_hstring(traceFilePath));
      DataTransfer::Clipboard::SetContentWithOptions(data, nullptr);
    });
  }
}

const wchar_t *FastRefreshLabel(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  return Mso::React::ReactOptions::UseFastRefresh(reactContext->Properties()) ? L"Disable Fast Refresh"
                                                                              : L"Enable Fast Refresh";
}

const wchar_t *HermesProfilerLabel(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  return !Microsoft::ReactNative::HermesSamplingProfiler::IsStarted() ? L"Start Hermes sampling profiler"
                                                                      : L"Stop and copy trace path to clipboard";
}

#ifndef CORE_ABI
struct InAppXamlDevMenu : public IDevMenu, public std::enable_shared_from_this<InAppXamlDevMenu> {
  InAppXamlDevMenu(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) : m_context(reactContext) {}

 private:
  void Show() noexcept override {
    winrt::Microsoft::ReactNative::DevMenuControl devMenu{};

    devMenu.FastRefreshText().Text(FastRefreshLabel(m_context));
    if (Mso::React::ReactOptions::JsiEngine(m_context->Properties()) == Mso::React::JSIEngine::Hermes) {
      devMenu.SamplingProfilerText().Text(HermesProfilerLabel(m_context));
      devMenu.SamplingProfilerIcon().Glyph(
          !Microsoft::ReactNative::HermesSamplingProfiler::IsStarted() ? L"\ue1e5" : L"\ue15b");

      std::ostringstream os;
      if (Microsoft::ReactNative::HermesSamplingProfiler::IsStarted()) {
        os << "Hermes Sampling profiler is running!";
      } else {
        os << "Click to start.";
      }

      auto lastTraceFilePath = Microsoft::ReactNative::HermesSamplingProfiler::GetLastTraceFilePath();
      if (!lastTraceFilePath.empty()) {
        os << std::endl
           << "Samples from last invocation are stored at " << winrt::to_string(lastTraceFilePath)
           << "  (path copied to clipboard).";
        os << std::endl << "Navigate to \"edge:\\tracing\" and load the trace file.";
      }

      devMenu.SamplingProfilerDescText().Text(winrt::to_hstring(os.str()));
    }

    devMenu.DirectDebugText().Text(DirectDebuggerLabel(m_context));
    devMenu.DirectDebugDesc().Text(
        L"If using Chakra, this will allow Visual Studio to be attached directly to the application using \"Script Debugging\" to debug the JS running directly in this app.\nIf using V8/Hermes, this will enable standard JS debugging tools such as VSCode to attach to the application.");

    devMenu.BreakOnNextLineText().Text(BreakOnNextLineLabel(m_context));

    m_reloadJSRevoker = devMenu.Reload().Click(
        winrt::auto_revoke,
        [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
          if (auto strongThis = wkThis.lock()) {
            strongThis->Hide();
            DevSettings::Reload(React::ReactPropertyBag(strongThis->m_context->Properties()));
          }
        });

    m_directDebuggingRevoker = devMenu.DirectDebug().Click(
        winrt::auto_revoke,
        [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
          if (auto strongThis = wkThis.lock()) {
            strongThis->Hide();
            ToggleDirectDebugger(strongThis->m_context);
          }
        });

    m_breakOnNextLineRevoker = devMenu.BreakOnNextLine().Click(
        winrt::auto_revoke,
        [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
          if (auto strongThis = wkThis.lock()) {
            strongThis->Hide();
            ToggleBreakOnNextLine(strongThis->m_context);
          }
        });

    m_fastRefreshRevoker = devMenu.FastRefresh().Click(
        winrt::auto_revoke,
        [wkThis = weak_from_this()](auto & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
          if (auto strongThis = wkThis.lock()) {
            strongThis->Hide();
            ToggleFastRefresh(strongThis->m_context);
          }
        });

    if (Mso::React::ReactOptions::JsiEngine(m_context->Properties()) == Mso::React::JSIEngine::Hermes) {
      m_samplingProfilerRevoker = devMenu.SamplingProfiler().Click(
          winrt::auto_revoke,
          [wkThis = weak_from_this()](auto & /*sender*/, xaml::RoutedEventArgs const & /*args*/) noexcept {
            if (auto strongThis = wkThis.lock()) {
              strongThis->Hide();
              ToggleHermesProfiler(strongThis->m_context);
            }
          });
    } else {
      devMenu.SamplingProfiler().Visibility(xaml::Visibility::Collapsed);
    }

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
        winrt::auto_revoke,
        [wkThis = weak_from_this()](auto const & /*sender*/, xaml::RoutedEventArgs const & /*args*/) {
          if (auto strongThis = wkThis.lock()) {
            strongThis->Hide();
          }
        });

    m_flyout = xaml::Controls::Flyout{};
    m_flyout.Content(devMenu);
    if (Is19H1OrHigher()) {
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
  }

  void Hide() noexcept {
    if (!m_flyout)
      return;
    m_flyout.Hide();
    m_flyout = nullptr;
  }

 private:
  const Mso::CntPtr<Mso::React::IReactContext> m_context;
  xaml::Controls::Flyout m_flyout{nullptr};
  xaml::Controls::Button::Click_revoker m_cancelRevoker{};
  xaml::Controls::Button::Click_revoker m_toggleInspectorRevoker{};
  xaml::Controls::Button::Click_revoker m_configBundlerRevoker{};
  xaml::Controls::Button::Click_revoker m_reloadJSRevoker{};
  xaml::Controls::Button::Click_revoker m_fastRefreshRevoker{};
  xaml::Controls::Button::Click_revoker m_directDebuggingRevoker{};
  xaml::Controls::Button::Click_revoker m_breakOnNextLineRevoker{};
  xaml::Controls::Button::Click_revoker m_samplingProfilerRevoker{};
};
#endif // CORE_ABI

#if defined(CORE_ABI) || defined(USE_FABRIC)
struct WindowsPopupMenuDevMenu : public IDevMenu, public std::enable_shared_from_this<WindowsPopupMenuDevMenu> {
  WindowsPopupMenuDevMenu(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) : m_context(reactContext) {}

  void Show() noexcept override {
    m_menu = winrt::Windows::UI::Popups::PopupMenu();

    auto hwnd =
        winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::GetTopLevelWindowId(m_context->Properties());
    if (hwnd) {
      auto initializeWithWindow{m_menu.as<::IInitializeWithWindow>()};
      initializeWithWindow->Initialize(reinterpret_cast<HWND>(hwnd));
    }

    m_menu.Commands().Append(winrt::Windows::UI::Popups::UICommand(
        L"Reload", [reactContext = m_context](winrt::Windows::UI::Popups::IUICommand const &) {
          DevSettings::Reload(React::ReactPropertyBag(reactContext->Properties()));
        }));

    m_menu.Commands().Append(winrt::Windows::UI::Popups::UICommand(
        DirectDebuggerLabel(m_context),
        [context = m_context](winrt::Windows::UI::Popups::IUICommand const &) { ToggleDirectDebugger(context); }));

    m_menu.Commands().Append(winrt::Windows::UI::Popups::UICommand(
        BreakOnNextLineLabel(m_context),
        [context = m_context](winrt::Windows::UI::Popups::IUICommand const &) { ToggleBreakOnNextLine(context); }));

    m_menu.Commands().Append(winrt::Windows::UI::Popups::UICommand(
        FastRefreshLabel(m_context),
        [context = m_context](winrt::Windows::UI::Popups::IUICommand const &) { ToggleFastRefresh(context); }));

    m_menu.Commands().Append(winrt::Windows::UI::Popups::UICommand(
        L"Toggle Inspector", [context = m_context](winrt::Windows::UI::Popups::IUICommand const &) {
          DevSettings::ToggleElementInspector(*context);
        }));

    if (Mso::React::ReactOptions::JsiEngine(m_context->Properties()) == Mso::React::JSIEngine::Hermes) {
      m_menu.Commands().Append(winrt::Windows::UI::Popups::UICommand(
          HermesProfilerLabel(m_context),
          [context = m_context](winrt::Windows::UI::Popups::IUICommand const &) { ToggleHermesProfiler(context); }));
    }

    m_menu.ShowAsync({0, 0});
  }

 private:
  winrt::Windows::UI::Popups::PopupMenu m_menu{nullptr};
  const Mso::CntPtr<Mso::React::IReactContext> m_context;
};
#endif // defined(CORE_ABI) || defined(USE_FABRIC)

DevMenuManager::DevMenuManager(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) : m_context(reactContext) {}

void DevMenuManager::Init() noexcept {
#ifndef CORE_ABI
  auto uiDispatcher = React::implementation::ReactDispatcher::GetUIDispatcher(m_context->Properties());
  uiDispatcher.Post([weakThis = weak_from_this()]() {
    if (auto strongThis = weakThis.lock()) {
      auto context = strongThis->m_context;

      if (Is19H1OrHigher()) {
        // XamlRoot added in 19H1
        if (auto xamlRoot = React::XamlUIService::GetXamlRoot(strongThis->m_context->Properties())) {
          if (auto rootContent = xamlRoot.Content()) {
            strongThis->m_keyDownRevoker = rootContent.KeyDown(
                winrt::auto_revoke, [context](const auto & /*sender*/, const xaml::Input::KeyRoutedEventArgs &args) {
                  if (IsCtrlShiftD(args.Key())) {
                    DevMenuManager::Show(context);
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
                DevMenuManager::Show(context);
              }
            });
      }
    }
  });
#endif // CORE_ABI
}

/*static*/ void DevMenuManager::InitDevMenu(
    Mso::CntPtr<Mso::React::IReactContext> const &reactContext,
    Mso::VoidFunctor &&configureBundler) noexcept {
  auto devMenuManager = std::make_shared<DevMenuManager>(reactContext);
  devMenuManager->Init();
  React::ReactPropertyBag(reactContext->Properties()).Set(DevMenuManagerProperty(), devMenuManager);
  React::ReactPropertyBag(reactContext->Properties()).Set(ConfigureBundlerProperty(), std::move(configureBundler));
}

std::shared_ptr<IDevMenu> GetOrCreateDevMenu(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  return React::ReactPropertyBag(reactContext->Properties())
      .GetOrCreate(
          DevMenuImplProperty(),
          [reactContext]() -> std::shared_ptr<IDevMenu> {
#ifndef CORE_ABI
            if (xaml::TryGetCurrentUwpXamlApplication()) {
              auto devMenu = std::make_shared<InAppXamlDevMenu>(reactContext);
              return devMenu;
            } else
#endif // CORE_ABI

#if defined(CORE_ABI) || defined(USE_FABRIC)
            {
              auto devMenu = std::make_shared<WindowsPopupMenuDevMenu>(reactContext);
              return devMenu;
            }
#endif
            return nullptr;
          })
      .Value();
}

/*static*/ void DevMenuManager::Show(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  if (!Mso::React::ReactOptions::UseDeveloperSupport(reactContext->Properties()))
    return;

  if (auto devMenu = GetOrCreateDevMenu(reactContext)) {
    devMenu->Show();
  }

  // Notify instance that dev menu is shown -- This is used to trigger a connection to dev tools
  reactContext->CallJSFunction("RCTNativeAppEventEmitter", "emit", folly::dynamic::array("RCTDevMenuShown"));
}

} // namespace Microsoft::ReactNative
