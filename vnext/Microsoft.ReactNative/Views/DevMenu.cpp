// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DevMenu.h"

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "IReactDispatcher.h"
#include "Modules/DevSettingsModule.h"

#include <ReactCoreInjection.h>
#include <Shobjidl.h>
#include "winrt/Windows.UI.Popups.h"

using namespace winrt::Windows::ApplicationModel;

namespace Microsoft::ReactNative {

struct IDevMenu {
  virtual void Show() noexcept = 0;
};

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

const wchar_t *FastRefreshLabel(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) noexcept {
  return Mso::React::ReactOptions::UseFastRefresh(reactContext->Properties()) ? L"Disable Fast Refresh"
                                                                              : L"Enable Fast Refresh";
}

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

    m_menu.ShowAsync({0, 0});
  }

 private:
  winrt::Windows::UI::Popups::PopupMenu m_menu{nullptr};
  const Mso::CntPtr<Mso::React::IReactContext> m_context;
};

DevMenuManager::DevMenuManager(Mso::CntPtr<Mso::React::IReactContext> const &reactContext) : m_context(reactContext) {}

void DevMenuManager::Init() noexcept {}

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
            return std::make_shared<WindowsPopupMenuDevMenu>(reactContext);
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
