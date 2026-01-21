// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
// ComboBox component for testing XAML popup positioning bug #15557
#include "pch.h"

#include "ComboBox.h"

#if defined(RNW_NEW_ARCH)

#include "codegen/react/components/SampleCustomComponent/ComboBox.g.h"

#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::SampleCustomComponent {

// ComboBox component to test popup positioning issue #15557
// When inside a ScrollView, the dropdown popup should appear at the correct position
// Bug 1: After scrolling, the popup appears at the wrong offset (FIXED via LayoutMetricsChanged)
// Bug 2: When popup is open and user scrolls, popup should dismiss (FIXED via SetXamlRoot + VisualTreeHelper)

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

    // Issue #15557 Bug 2 Fix: Register XamlRoot to enable popup dismissal when scroll begins.
    // This is the GENERIC pattern that ANY 3rd party XAML component should use:
    // 1. Create your XamlIsland and set its Content
    // 2. Call SetXamlRoot() with the content's XamlRoot
    // When the parent ScrollView starts scrolling, ContentIslandComponentView will use
    // VisualTreeHelper.GetOpenPopupsForXamlRoot() to find and close ALL open popups.
    // This works for ComboBox, DatePicker, TimePicker, Flyouts, etc. - any XAML popup!
    m_comboBox.Loaded([islandView, this](auto const &, auto const &) {
      // XamlRoot is available after the element is loaded
      if (auto xamlRoot = m_comboBox.XamlRoot()) {
        islandView.SetXamlRoot(xamlRoot);
      }
    });

    m_selectionChangedToken =
        m_comboBox.SelectionChanged([this](
                                        winrt::Windows::Foundation::IInspectable const &,
                                        winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const &) {
          if (auto emitter = EventEmitter()) {
            Codegen::ComboBox_OnSelectionChanged args;
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

 private:
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
  winrt::Microsoft::UI::Xaml::Controls::ComboBox m_comboBox{nullptr};
  winrt::event_token m_selectionChangedToken{};
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
