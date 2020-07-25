// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Input.h>
#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>
#include "PickerViewManager.h"
#include "Unicode.h"
#include "XamlFeatures.h"

#include <IReactInstance.h>

#include <winrt/Windows.Foundation.Metadata.h>

namespace react::uwp {

class PickerShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  PickerShadowNode();
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;
  bool NeedsForceLayout() override;

 private:
  void RepopulateItems();
  static void OnSelectionChanged(
      IReactInstance &instance,
      int64_t tag,
      folly::dynamic &&value,
      int32_t selectedIndex,
      folly::dynamic &&text);

  folly::dynamic m_items;
  int32_t m_selectedIndex = -1;

  // FUTURE: remove when we can require RS5+
  static react::uwp::TriBit s_isEditableComboboxSupported;

  xaml::Controls::ComboBox::SelectionChanged_revoker m_comboBoxSelectionChangedRevoker{};
  xaml::Controls::ComboBox::DropDownClosed_revoker m_comboBoxDropDownClosedRevoker{};
};

react::uwp::TriBit PickerShadowNode::s_isEditableComboboxSupported = react::uwp::TriBit::Undefined;

PickerShadowNode::PickerShadowNode() : Super() {
  if (s_isEditableComboboxSupported == react::uwp::TriBit::Undefined) {
    s_isEditableComboboxSupported =
#ifndef USE_WINUI3
        winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(
            XAML_NAMESPACE_STR L".Controls.ComboBox", L"IsEditableProperty")
        ? react::uwp::TriBit::Set
        : react::uwp::TriBit::NotSet;
#else
        react::uwp::TriBit::Set;
#endif
  }
}

void PickerShadowNode::createView() {
  Super::createView();
  auto combobox = GetView().as<xaml::Controls::ComboBox>();
  combobox.TabIndex(0);
  auto wkinstance = GetViewManager()->GetReactInstance();

  combobox.AllowFocusOnInteraction(true);

  m_comboBoxSelectionChangedRevoker = combobox.SelectionChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr) {
      int32_t index = combobox.SelectedIndex();
      folly::dynamic value;
      if (index >= 0 && index < static_cast<int32_t>(m_items.size()))
        value = m_items.at(index)["value"];
      folly::dynamic text;
      if (s_isEditableComboboxSupported == react::uwp::TriBit::Set && index == -1)
        text = HstringToDynamic(combobox.Text());
      OnSelectionChanged(*instance, m_tag, std::move(value), index, std::move(text));
    }
  });

  m_comboBoxDropDownClosedRevoker = combobox.DropDownClosed(winrt::auto_revoke, [=](auto &&, auto &&) {
    // When the drop down closes, attempt to move focus to its anchor
    // textbox to prevent cases where focus can land on an outer flyout
    // content and therefore trigger a unexpected flyout dismissal
    xaml::Input::FocusManager::TryFocusAsync(combobox, xaml::FocusState::Programmatic);
  });
}

void PickerShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;

  bool updateSelectedIndex = false;
  auto combobox = GetView().as<xaml::Controls::ComboBox>();
  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "editable") {
      if (s_isEditableComboboxSupported == react::uwp::TriBit::Set) {
        if (propertyValue.isBool())
          combobox.IsEditable(propertyValue.asBool());
        else if (propertyValue.isNull())
          combobox.ClearValue(xaml::Controls::ComboBox::IsEditableProperty());
      }
    } else if (propertyName == "text") {
      if (s_isEditableComboboxSupported == react::uwp::TriBit::Set) {
        if (propertyValue.isString())
          combobox.Text(asHstring(propertyValue));
        else if (propertyValue.isNull())
          combobox.ClearValue(xaml::Controls::ComboBox::TextProperty());
      }
    } else if (propertyName == "enabled") {
      if (propertyValue.isBool())
        combobox.IsEnabled(propertyValue.asBool());
    } else if (propertyName == "selectedIndex") {
      if (propertyValue.isNumber()) {
        auto selectedIndex = propertyValue.asDouble();
        if (selectedIndex == static_cast<int32_t>(selectedIndex)) {
          m_selectedIndex = static_cast<int32_t>(selectedIndex);
        }
        updateSelectedIndex = true;
      }
    } else if (propertyName == "items") {
      if (propertyValue.isArray()) {
        m_items = propertyValue;
        RepopulateItems();
      }
    }
  }

  // Update selectedIndex last, in case items and selectedIndex were both
  // changing
  if (updateSelectedIndex)
    combobox.SelectedIndex(m_selectedIndex);

  Super::updateProperties(std::move(props));
  m_updating = false;
}

void PickerShadowNode::RepopulateItems() {
  auto combobox = GetView().as<xaml::Controls::ComboBox>();

  auto comboBoxItems = combobox.Items();
  comboBoxItems.Clear();
  for (const auto &item : m_items) {
    if (item.count("label")) {
      std::string label = item["label"].asString();
      auto comboboxItem = xaml::Controls::ComboBoxItem();

      comboboxItem.Content(winrt::box_value(Microsoft::Common::Unicode::Utf8ToUtf16(label)));

      if (item.count("textColor") && IsValidColorValue(item["textColor"]))
        comboboxItem.Foreground(BrushFrom(item["textColor"]));

      comboBoxItems.Append(comboboxItem);
    }
  }
  if (m_selectedIndex < static_cast<int32_t>(m_items.size()))
    combobox.SelectedIndex(m_selectedIndex);
}

/*static*/ void PickerShadowNode::OnSelectionChanged(
    IReactInstance &instance,
    int64_t tag,
    folly::dynamic &&value,
    int32_t selectedIndex,
    folly::dynamic &&text) {
  folly::dynamic eventData = folly::dynamic::object("target", tag)("value", std::move(value))(
      "itemIndex", selectedIndex)("text", std::move(text));
  instance.DispatchEvent(tag, "topChange", std::move(eventData));
}

bool PickerShadowNode::NeedsForceLayout() {
  return true;
}

PickerViewManager::PickerViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

const char *PickerViewManager::GetName() const {
  return "RCTPicker";
}

folly::dynamic PickerViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("editable", "boolean")("enabled", "boolean")("items", "array")(
      "selectedIndex", "number")("text", "string"));

  return props;
}

facebook::react::ShadowNode *PickerViewManager::createShadow() const {
  return new PickerShadowNode();
}

XamlView PickerViewManager::CreateViewCore(int64_t /*tag*/) {
  auto combobox = xaml::Controls::ComboBox();
  return combobox;
}

YGMeasureFunc PickerViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

} // namespace react::uwp
