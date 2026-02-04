// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "PickerXaml.h"

#if defined(RNW_NEW_ARCH)

#include "codegen/react/components/SampleCustomComponent/PickerXaml.g.h"

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <limits>

namespace winrt::SampleCustomComponent {

// State to store the measured size
struct PickerXamlStateData : winrt::implements<PickerXamlStateData, winrt::IInspectable> {
  PickerXamlStateData(winrt::Windows::Foundation::Size ds) : desiredSize(ds) {}
  winrt::Windows::Foundation::Size desiredSize;
};

struct PickerXamlComponentView : winrt::implements<PickerXamlComponentView, winrt::IInspectable>,
                                 Codegen::BasePickerXaml<PickerXamlComponentView> {
  void InitializeContentIsland(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) {
    // Create ComboBox for picker functionality
    m_comboBox = winrt::Microsoft::UI::Xaml::Controls::ComboBox();
    m_comboBox.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Stretch);

    // For editable ComboBox, trigger selection change always (not just on commit)
    m_comboBox.SelectionChangedTrigger(winrt::Microsoft::UI::Xaml::Controls::ComboBoxSelectionChangedTrigger::Always);

    // Listen for size changes on the comboBox
    m_comboBox.SizeChanged([this](auto const & /*sender*/, auto const & /*args*/) { RefreshSize(); });

    // Listen for selection changes
    m_selectionChangedRevoker = m_comboBox.SelectionChanged(
        winrt::auto_revoke, [this](const auto & /*sender*/, const auto & /*args*/) { EmitPickerSelectEvent(); });

    // Listen for text submitted (when user presses Enter in editable mode)
    m_textSubmittedRevoker = m_comboBox.TextSubmitted(
        winrt::auto_revoke, [this](const auto & /*sender*/, const auto & /*args*/) { EmitPickerSelectEvent(); });

    m_island = winrt::Microsoft::UI::Xaml::XamlIsland{};
    m_island.Content(m_comboBox);
    islandView.Connect(m_island.ContentIsland());
    m_islandView = winrt::make_weak(islandView);
  }

  void UpdateProps(
      const winrt::Microsoft::ReactNative::ComponentView &view,
      const winrt::com_ptr<Codegen::PickerXamlProps> &newProps,
      const winrt::com_ptr<Codegen::PickerXamlProps> &oldProps) noexcept override {
    BasePickerXaml::UpdateProps(view, newProps, oldProps);

    // Suspend event handlers during programmatic updates to avoid triggering
    // change events. Using RAII ensures handlers are always re-attached.
    WithEventsSuspended([&]() {
      // Always update items on first render, or when size changes
      // (We can't compare items directly as the struct lacks operator==)
      if (!oldProps || oldProps->items.size() != newProps->items.size() || m_comboBox.Items().Size() == 0) {
        m_comboBox.Items().Clear();
        m_items.clear();

        for (const auto &item : newProps->items) {
          // Store item data
          m_items.push_back(item);

          // Add item to ComboBox
          auto comboBoxItem = winrt::Microsoft::UI::Xaml::Controls::ComboBoxItem();
          comboBoxItem.Content(winrt::box_value(winrt::to_hstring(item.label)));
          m_comboBox.Items().Append(comboBoxItem);
        }
      }

      // Always update selected index on first render or when changed
      if (!oldProps || oldProps->selectedIndex != newProps->selectedIndex ||
          m_comboBox.SelectedIndex() != newProps->selectedIndex) {
        const int32_t selectedIndex = newProps->selectedIndex;
        if (selectedIndex >= 0 && selectedIndex < static_cast<int32_t>(m_comboBox.Items().Size())) {
          m_comboBox.SelectedIndex(selectedIndex);
        } else {
          m_comboBox.SelectedIndex(-1);
        }
      }
    });

    RefreshSize();
  }

  void UpdateState(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept override {
    m_state = newState;
  }

 private:
  void RefreshSize() {
    m_comboBox.Measure(winrt::Windows::Foundation::Size{
        std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});

    const auto desiredSize = m_comboBox.DesiredSize();

    if (m_state) {
      const auto currentState = winrt::get_self<PickerXamlStateData>(m_state.Data());
      if (desiredSize != currentState->desiredSize) {
        m_state.UpdateStateWithMutation([desiredSize](winrt::Windows::Foundation::IInspectable /*data*/) {
          return winrt::make<PickerXamlStateData>(desiredSize);
        });
      }
    }
  }

  void EmitPickerSelectEvent() {
    if (auto eventEmitter = this->EventEmitter()) {
      const int32_t selectedIndex = m_comboBox.SelectedIndex();

      Codegen::PickerXaml_OnPickerSelect eventArgs;
      eventArgs.itemIndex = selectedIndex;

      // Get the selected item value and text if available
      if (selectedIndex >= 0 && selectedIndex < static_cast<int32_t>(m_items.size())) {
        eventArgs.value = m_items[selectedIndex].value.value_or("");
        eventArgs.text = m_items[selectedIndex].label;
      } else {
        eventArgs.text = "";
        eventArgs.value = "";
      }

      eventEmitter->onPickerSelect(eventArgs);
    }
  }

  winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView> m_islandView;
  winrt::Microsoft::UI::Xaml::XamlIsland m_island{nullptr};
  winrt::Microsoft::UI::Xaml::Controls::ComboBox m_comboBox{nullptr};
  winrt::Microsoft::ReactNative::IComponentState m_state{nullptr};
  std::vector<Codegen::PickerXamlSpec_PickerXamlProps_items> m_items;
  winrt::Microsoft::UI::Xaml::Controls::ComboBox::SelectionChanged_revoker m_selectionChangedRevoker;
  winrt::Microsoft::UI::Xaml::Controls::ComboBox::TextSubmitted_revoker m_textSubmittedRevoker;

  // RAII helper to temporarily suspend event handlers during programmatic updates.
  // This avoids triggering change events when we're setting values from props.
  template <typename TAction>
  void WithEventsSuspended(TAction action) {
    m_selectionChangedRevoker.revoke();
    m_textSubmittedRevoker.revoke();

    action();

    m_selectionChangedRevoker = m_comboBox.SelectionChanged(
        winrt::auto_revoke, [this](const auto & /*sender*/, const auto & /*args*/) { EmitPickerSelectEvent(); });
    m_textSubmittedRevoker = m_comboBox.TextSubmitted(
        winrt::auto_revoke, [this](const auto & /*sender*/, const auto & /*args*/) { EmitPickerSelectEvent(); });
  }
};

} // namespace winrt::SampleCustomComponent

void RegisterPickerXamlComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  winrt::SampleCustomComponent::Codegen::RegisterPickerXamlNativeComponent<
      winrt::SampleCustomComponent::PickerXamlComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.as<winrt::Microsoft::ReactNative::IReactViewComponentBuilder>().XamlSupport(true);
        // Use SetContentIslandComponentViewInitializer
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::SampleCustomComponent::PickerXamlComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
        // Set up initial state with zero size
        builder.as<winrt::Microsoft::ReactNative::IReactViewComponentBuilder>().SetInitialStateDataFactory(
            [](const winrt::Microsoft::ReactNative::IComponentProps & /*props*/) noexcept {
              return winrt::make<winrt::SampleCustomComponent::PickerXamlStateData>(
                  winrt::Windows::Foundation::Size{0, 0});
            });

        // Register the measure function - reads from state
        builder.as<winrt::Microsoft::ReactNative::IReactViewComponentBuilder>().SetMeasureContentHandler(
            [](winrt::Microsoft::ReactNative::ShadowNode const &shadowNode,
               winrt::Microsoft::ReactNative::LayoutContext const &,
               winrt::Microsoft::ReactNative::LayoutConstraints const &) noexcept {
              const auto currentState =
                  winrt::get_self<winrt::SampleCustomComponent::PickerXamlStateData>(shadowNode.StateData());

              if (currentState && currentState->desiredSize.Width > 0) {
                // Return the measured size from state
                return currentState->desiredSize;
              }

              // Return a default size if we don't have a measurement yet
              return winrt::Windows::Foundation::Size{100, 32};
            });

        // Handle state updates
        builder.as<winrt::Microsoft::ReactNative::IReactViewComponentBuilder>().SetUpdateStateHandler(
            [](const winrt::Microsoft::ReactNative::ComponentView &view,
               const winrt::Microsoft::ReactNative::IComponentState &newState) {
              const auto islandView = view.as<winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView>();
              const auto userData = islandView.UserData().as<winrt::SampleCustomComponent::PickerXamlComponentView>();
              userData->UpdateState(view, newState);
            });
      });
}

#endif // defined(RNW_NEW_ARCH)
