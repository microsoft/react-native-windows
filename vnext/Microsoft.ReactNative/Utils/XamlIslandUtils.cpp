// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utils/XamlIslandUtils.h"

#include <UI.Xaml.Input.h>

namespace Microsoft::ReactNative {

struct CustomAppBarButton : xaml::Controls::AppBarButtonT<CustomAppBarButton> {
  void OnPointerExited(xaml::Input::PointerRoutedEventArgs const &e) {
    // This method crashes in the superclass, so we purposely don't call super. But the superclass
    // implementation likely cancels a timer that will show the submenu shortly after pointer enter.
    // Since we don't have access to that timer, instead we reset the Flyout property, which resets
    // the timer. This also fixes a crash where you can get a zombie submenu showing if the app
    // loses focus while this timer is scheduled to show the submenu.
    if (auto flyout = this->Flyout()) {
      this->Flyout(nullptr);
      this->Flyout(flyout);
    }

    // The superclass implementation resets the button to the normal state, so we do this ourselves.
    this->SetValue(xaml::Controls::Primitives::ButtonBase::IsPointerOverProperty(), winrt::box_value(false));
    xaml::VisualStateManager::GoToState(*this, L"Normal", false);
  }

  void OnPointerPressed(xaml::Input::PointerRoutedEventArgs const &e) {
    // Clicking AppBarButton by default will dismiss the menu, but since we only use this class for
    // submenus we override it to be a no-op so it behaves like MenuFlyoutSubItem.
  }
};

void FixProofingMenuCrashForXamlIsland(xaml::Controls::Primitives::FlyoutBase const &flyout) {
  flyout.Opening([](winrt::IInspectable const &sender, auto &&) {
    const auto &flyout = sender.as<winrt::Microsoft::UI::Xaml::Controls::TextCommandBarFlyout>();
    if (const auto &textBox = flyout.Target().try_as<xaml::Controls::TextBox>()) {
      const auto &commands = flyout.SecondaryCommands();
      for (uint32_t i = 0; i < commands.Size(); ++i) {
        if (const auto &appBarButton = commands.GetAt(i).try_as<xaml::Controls::AppBarButton>()) {
          if (!appBarButton.Flyout()) {
            // This works around a loss of focus from the target element when clicking on
            // on the menu items.
            // https://github.com/microsoft/microsoft-ui-xaml/issues/5818
            appBarButton.Click([weakCommandBarFlyout = winrt::make_weak(flyout)](auto &&...) {
              if (auto flyout = weakCommandBarFlyout.get()) {
                xaml::Input::FocusManager::TryFocusAsync(flyout.Target(), xaml::FocusState::Programmatic);
              }
            });
          } else if (appBarButton.Flyout() == textBox.ProofingMenuFlyout()) {
            if (textBox.IsSpellCheckEnabled()) {
              // Replace the AppBarButton for the proofing menu with one that doesn't crash
              const auto customAppBarButton = winrt::make<CustomAppBarButton>();
              customAppBarButton.Label(appBarButton.Label());
              customAppBarButton.Icon(appBarButton.Icon());
              customAppBarButton.Flyout(appBarButton.Flyout());
              commands.RemoveAt(i);
              commands.InsertAt(i, customAppBarButton);
            } else {
              // Remove proofing menu option if spell-check is disabled
              commands.RemoveAt(i);
            }

            // There is only one proofing menu option
            break;
          }
        }
      }
    }
  });
}

void FixMenuThemeForXamlIsland(xaml::Controls::Primitives::FlyoutBase const &flyout) {
  flyout.Opening([](winrt::IInspectable const &sender, auto &&) {
    const auto &flyout = sender.as<winrt::Microsoft::UI::Xaml::Controls::TextCommandBarFlyout>();
    auto theme = xaml::ElementTheme::Default;
    if (const auto targetElement = flyout.Target().try_as<xaml::UIElement>()) {
      if (const auto content = targetElement.XamlRoot().Content().try_as<xaml::FrameworkElement>()) {
        theme = content.ActualTheme();
      }
    }

    const auto commands = flyout.SecondaryCommands();
    for (uint32_t i = 0; i < commands.Size(); ++i) {
      if (const auto &appBarButton = commands.GetAt(i).try_as<xaml::Controls::AppBarButton>()) {
        // Workaround Xaml Islands bug with dark theme and CommandBarFlyout:
        // https://github.com/microsoft/microsoft-ui-xaml/issues/5320
        appBarButton.RequestedTheme(theme);
      }
    }
  });
}

} // namespace Microsoft::ReactNative
