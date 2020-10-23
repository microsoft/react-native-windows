// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <JSValueWriter.h>
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

namespace Microsoft::ReactNative {

class PickerShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  PickerShadowNode();
  void createView() override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;
  bool NeedsForceLayout() override;

 private:
  void RepopulateItems();
  static void OnSelectionChanged(
      const Mso::React::IReactContext &context,
      int64_t tag,
      folly::dynamic &&value,
      int32_t selectedIndex,
      folly::dynamic &&text);

  winrt::Microsoft::ReactNative::JSValueArray m_items;
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
  combobox.AllowFocusOnInteraction(true);

  m_comboBoxSelectionChangedRevoker = combobox.SelectionChanged(winrt::auto_revoke, [=](auto &&, auto &&) {
    if (!m_updating) {
      int32_t index = combobox.SelectedIndex();
      folly::dynamic value;
      if (index >= 0 && index < static_cast<int32_t>(m_items.size())) {
        if (m_items[index].AsObject()["value"].Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
          value = m_items[index].AsObject()["value"].AsString();
        } else if (
            m_items[index].AsObject()["value"].Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
            m_items[index].AsObject()["value"].Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
          value = m_items[index].AsObject()["value"].AsDouble();
        }
      }
      folly::dynamic text;
      if (s_isEditableComboboxSupported == react::uwp::TriBit::Set && index == -1)
        text = react::uwp::HstringToDynamic(combobox.Text());
      OnSelectionChanged(GetViewManager()->GetReactContext(), m_tag, std::move(value), index, std::move(text));
    }
  });

  m_comboBoxDropDownClosedRevoker = combobox.DropDownClosed(winrt::auto_revoke, [=](auto &&, auto &&) {
    // When the drop down closes, attempt to move focus to its anchor
    // textbox to prevent cases where focus can land on an outer flyout
    // content and therefore trigger a unexpected flyout dismissal
    xaml::Input::FocusManager::TryFocusAsync(combobox, xaml::FocusState::Programmatic);
  });
}

void PickerShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  m_updating = true;

  bool updateSelectedIndex = false;
  auto combobox = GetView().as<xaml::Controls::ComboBox>();
  for (auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;

    if (propertyName == "editable") {
      if (s_isEditableComboboxSupported == react::uwp::TriBit::Set) {
        if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
          combobox.IsEditable(propertyValue.AsBoolean());
        else if (propertyValue.IsNull())
          combobox.ClearValue(xaml::Controls::ComboBox::IsEditableProperty());
      }
    } else if (propertyName == "text") {
      if (s_isEditableComboboxSupported == react::uwp::TriBit::Set) {
        if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String)
          combobox.Text(react::uwp::asHstring(propertyValue));
        else if (propertyValue.IsNull())
          combobox.ClearValue(xaml::Controls::ComboBox::TextProperty());
      }
    } else if (propertyName == "enabled") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
        combobox.IsEnabled(propertyValue.AsBoolean());
    } else if (propertyName == "selectedIndex") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        m_selectedIndex = propertyValue.AsInt32();
      }
      updateSelectedIndex = true;
    } else if (propertyName == "items") {
      m_items = propertyValue.AsArray().Copy();
      RepopulateItems();
    }
  }

  // Update selectedIndex last, in case items and selectedIndex were both
  // changing
  if (updateSelectedIndex)
    combobox.SelectedIndex(m_selectedIndex);

  Super::updateProperties(props);
  m_updating = false;
}

void PickerShadowNode::RepopulateItems() {
  auto combobox = GetView().as<xaml::Controls::ComboBox>();

  auto comboBoxItems = combobox.Items();
  comboBoxItems.Clear();
  for (const auto &item : m_items) {
    if (!item["label"].IsNull()) {
      std::string label = item["label"].AsString();
      auto comboboxItem = xaml::Controls::ComboBoxItem();

      comboboxItem.Content(winrt::box_value(Microsoft::Common::Unicode::Utf8ToUtf16(label)));

      if (!item["textColor"].IsNull() && react::uwp::IsValidColorValue(item["textColor"]))
        comboboxItem.Foreground(react::uwp::BrushFrom(item["textColor"]));

      comboBoxItems.Append(comboboxItem);
    }
  }
  if (m_selectedIndex < static_cast<int32_t>(m_items.size()))
    combobox.SelectedIndex(m_selectedIndex);
}

/*static*/ void PickerShadowNode::OnSelectionChanged(
    const Mso::React::IReactContext &context,
    int64_t tag,
    folly::dynamic &&value,
    int32_t selectedIndex,
    folly::dynamic &&text) {
  folly::dynamic eventData = folly::dynamic::object("target", tag)("value", std::move(value))(
      "itemIndex", selectedIndex)("text", std::move(text));
  context.DispatchEvent(tag, "topChange", std::move(eventData));
}

bool PickerShadowNode::NeedsForceLayout() {
  return true;
}

PickerViewManager::PickerViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *PickerViewManager::GetName() const {
  return L"RCTPicker";
}

void PickerViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"editable", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"enabled", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"items", L"array");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"selectedIndex", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"text", L"string");
}

ShadowNode *PickerViewManager::createShadow() const {
  return new PickerShadowNode();
}

XamlView PickerViewManager::CreateViewCore(int64_t /*tag*/) {
  auto combobox = xaml::Controls::ComboBox();
  return combobox;
}

YGMeasureFunc PickerViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

} // namespace Microsoft::ReactNative
