// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
// ComboBox component for testing XAML popup positioning bug #15557
#include "pch.h"

#include "ComboBox.h"

#if defined(RNW_NEW_ARCH)

#include "codegen/react/components/SampleCustomComponent/ComboBox.g.h"

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>

namespace winrt::SampleCustomComponent {

// ComboBox component to test popup positioning issue #15557
// When inside a ScrollView, the dropdown popup should appear at the correct position
// Bug 1: After scrolling, the popup appears at the wrong offset (FIXED via ViewChanged + transform update)
// Bug 2: When popup is open and user scrolls, popup should dismiss (FIXED via ViewChanged + DismissPopups)

struct ComboBoxComponentView : public winrt::implements<ComboBoxComponentView, winrt::IInspectable>,
                               Codegen::BaseComboBox<ComboBoxComponentView> {
  void InitializeContentIsland(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};
    m_comboBox = winrt::Microsoft::UI::Xaml::Controls::ComboBox{};

    // Add default items
    m_comboBox.Items().Append(winrt::box_value(L"Option 1 - Select me after scrolling"));
    m_comboBox.Items().Append(winrt::box_value(L"Option 2 - Test popup position"));
    m_comboBox.Items().Append(winrt::box_value(L"Option 3 - Bug #15557"));
    m_comboBox.Items().Append(winrt::box_value(L"Option 4 - Popup should be here"));
    m_comboBox.Items().Append(winrt::box_value(L"Option 5 - Not somewhere else!"));

    m_comboBox.PlaceholderText(L"Click to open dropdown...");
    m_comboBox.FontSize(20);
    m_comboBox.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Stretch);
    m_comboBox.VerticalAlignment(winrt::Microsoft::UI::Xaml::VerticalAlignment::Center);

    m_xamlIsland.Content(m_comboBox);
    islandView.Connect(m_xamlIsland.ContentIsland());

    // Issue #15557 Bug 2 Fix: Subscribe to Mounted event to walk parent tree and register
    // for ViewChanged on parent ScrollViews. When scroll position changes, dismiss any open popups.
    // This pattern keeps ContentIslandComponentView XAML-agnostic - the 3P component handles its own popups.
    m_mountedToken = islandView.Mounted(
        [this](
            const winrt::Windows::Foundation::IInspectable &,
            const winrt::Microsoft::ReactNative::ComponentView &view) { RegisterForParentScrollViewEvents(view); });

    m_unmountedToken = islandView.Unmounted(
        [this](const winrt::Windows::Foundation::IInspectable &, const winrt::Microsoft::ReactNative::ComponentView &) {
          UnregisterFromParentScrollViewEvents();
        });

    m_selectionChangedToken =
        m_comboBox.SelectionChanged([this](
                                        winrt::Windows::Foundation::IInspectable const &,
                                        winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const &) {
          if (auto emitter = EventEmitter()) {
            Codegen::ComboBoxEventEmitter::OnSelectionChanged args;
            args.selectedIndex = m_comboBox.SelectedIndex();
            if (m_comboBox.SelectedItem()) {
              auto selectedText = winrt::unbox_value<winrt::hstring>(m_comboBox.SelectedItem());
              args.selectedValue = winrt::to_string(selectedText);
            } else {
              args.selectedValue = "";
            }
            emitter->onSelectionChanged(args);
          }
        });
  }

  // Issue #15557: Walk up the parent tree to find ScrollViews and subscribe to ViewChanged.
  // When scroll position changes, dismiss any open popups (light dismiss on scroll).
  void RegisterForParentScrollViewEvents(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    auto parent = view.Parent();
    while (parent) {
      if (auto scrollView = parent.try_as<winrt::Microsoft::ReactNative::Composition::ScrollViewComponentView>()) {
        auto token = scrollView.ViewChanged(
            [this](const winrt::IInspectable &, const winrt::IInspectable &) { DismissPopups(); });
        m_viewChangedSubscriptions.push_back({scrollView, token});
      }
      parent = parent.Parent();
    }
  }

  // Issue #15557: Unsubscribe from parent ScrollView ViewChanged events
  void UnregisterFromParentScrollViewEvents() noexcept {
    for (auto &subscription : m_viewChangedSubscriptions) {
      if (auto scrollView = subscription.scrollView.get()) {
        scrollView.ViewChanged(subscription.token);
      }
    }
    m_viewChangedSubscriptions.clear();
  }

  // Dismiss any open popups for this component's XamlRoot
  void DismissPopups() noexcept {
    if (auto xamlRoot = m_comboBox.XamlRoot()) {
      auto openPopups = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(xamlRoot);
      for (const auto &popup : openPopups) {
        if (popup.IsOpen()) {
          popup.IsOpen(false);
        }
      }
    }
  }

 private:
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
  winrt::Microsoft::UI::Xaml::Controls::ComboBox m_comboBox{nullptr};
  winrt::event_token m_selectionChangedToken{};
  winrt::event_token m_mountedToken{};
  winrt::event_token m_unmountedToken{};

  // Issue #15557: Store ViewChanged subscriptions to parent ScrollViews for light dismiss
  struct ViewChangedSubscription {
    winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::ScrollViewComponentView> scrollView;
    winrt::event_token token;
  };
  std::vector<ViewChangedSubscription> m_viewChangedSubscriptions;
};

} // namespace winrt::SampleCustomComponent

void RegisterComboBoxComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  winrt::SampleCustomComponent::Codegen::RegisterComboBoxNativeComponent<
      winrt::SampleCustomComponent::ComboBoxComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::SampleCustomComponent::ComboBoxComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
      });
}

#endif // defined(RNW_NEW_ARCH)
