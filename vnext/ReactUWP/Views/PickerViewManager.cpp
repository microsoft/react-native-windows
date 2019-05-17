// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "PickerViewManager.h"
#include <Views/ShadowNodeBase.h>

#include <Utils/ValueUtils.h>
#include "unicode.h"

#include <IReactInstance.h>

#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>


namespace winrt {
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
}

namespace react { namespace uwp {

enum class PickerCommands
{
  SetFocus = 1,
};

class PickerShadowNode : public ShadowNodeBase
{
  using Super = ShadowNodeBase;
public:
  PickerShadowNode();
  void createView() override;
  void updateProperties(const folly::dynamic&& props) override;

private:
  void RepopulateItems();
  static void OnSelectionChanged(IReactInstance& instance, int64_t tag, folly::dynamic&& value, int32_t selectedIndex, folly::dynamic&& text);

  folly::dynamic m_items;
  int32_t m_selectedIndex = -1;

  // FUTURE: remove when we can require RS5+
  bool m_isEditableComboboxSupported;
};

PickerShadowNode::PickerShadowNode()
  : Super()
{
  m_isEditableComboboxSupported = winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.UI.Xaml.Controls.ComboBox", L"IsEditableProperty");
}

void PickerShadowNode::createView()
{
  Super::createView();
  auto combobox = GetView().as<winrt::ComboBox>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  combobox.SelectionChanged([=](auto&&, auto&&)
  {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
    {
      int32_t index = combobox.SelectedIndex();
      folly::dynamic value;
      if (index >= 0 && index < static_cast<int32_t>(m_items.size()))
        value = m_items.at(index)["value"];
      folly::dynamic text;
      if (m_isEditableComboboxSupported && index == -1)
        text = HstringToDynamic(combobox.Text());
      OnSelectionChanged(*instance, m_tag, std::move(value), index, std::move(text));
    }
  });
}

void PickerShadowNode::updateProperties(const folly::dynamic&& props)
{
  m_updating = true;

  bool updateSelectedIndex = false;
  auto combobox = GetView().as<winrt::ComboBox>();
  for (auto& pair : props.items())
  {
    const folly::dynamic& propertyName = pair.first;
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName == "editable")
    {
      if (m_isEditableComboboxSupported)
      {
        if (propertyValue.isBool())
          combobox.IsEditable(propertyValue.asBool());
        else if (propertyValue.isNull())
          combobox.ClearValue(winrt::ComboBox::IsEditableProperty());
      }
    }
    else if (propertyName == "text")
    {
      if (m_isEditableComboboxSupported)
      {
        if (propertyValue.isString())
          combobox.Text(asHstring(propertyValue));
        else if (propertyValue.isNull())
          combobox.ClearValue(winrt::ComboBox::TextProperty());
      }
    }
    else if (propertyName == "enabled")
    {
      if (propertyValue.isBool())
        combobox.IsEnabled(propertyValue.asBool());
    }
    else if (propertyName == "selectedIndex")
    {
      if (propertyValue.isNumber())
      {
        auto selectedIndex = propertyValue.asInt();
        if (selectedIndex == static_cast<int32_t>(selectedIndex))
        {
          m_selectedIndex = static_cast<int32_t>(selectedIndex);
        }
        updateSelectedIndex = true;
      }
    }
    else if (propertyName == "items")
    {
      if (propertyValue.isArray())
      {
        m_items = propertyValue;
        RepopulateItems();
      }
    }
  }

  // Update selectedIndex last, in case items and selectedIndex were both changing
  if (updateSelectedIndex)
    combobox.SelectedIndex(m_selectedIndex);

  Super::updateProperties(std::move(props));
  m_updating = false;
}

void PickerShadowNode::RepopulateItems()
{
  auto combobox = GetView().as<winrt::ComboBox>();

  auto comboBoxItems = combobox.Items();
  comboBoxItems.Clear();
  for (const auto& item : m_items)
  {
    if (item.count("label"))
    {
      std::string label = item["label"].asString();
      auto comboboxItem = winrt::ComboBoxItem();
      comboboxItem.Content(winrt::box_value(facebook::react::unicode::utf8ToUtf16(label)));
      if (item.count("textColor"))
        comboboxItem.Foreground(BrushFrom(item["textColor"]));
      comboBoxItems.Append(comboboxItem);
    }
  }
  if (m_selectedIndex < static_cast<int32_t>(m_items.size()))
    combobox.SelectedIndex(m_selectedIndex);
}

/*static*/ void PickerShadowNode::OnSelectionChanged(IReactInstance& instance, int64_t tag, folly::dynamic&& value, int32_t selectedIndex, folly::dynamic&& text)
{
  folly::dynamic eventData = folly::dynamic::object
    ("target", tag)
    ("value", std::move(value))
    ("itemIndex", selectedIndex)
    ("text", std::move(text));
  instance.DispatchEvent(tag, "topChange", std::move(eventData));
}

PickerViewManager::PickerViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* PickerViewManager::GetName() const
{
  return "RCTPicker";
}

folly::dynamic PickerViewManager::GetCommands() const
{
  auto commands = Super::GetCommands();
  commands.update(folly::dynamic::object
    ("SetFocus", static_cast<std::underlying_type_t<PickerCommands>>(PickerCommands::SetFocus))
  );
  return commands;
}

folly::dynamic PickerViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("editable", "boolean")
    ("enabled", "boolean")
    ("items", "array")
    ("selectedIndex", "number")
    ("text", "string")
  );

  return props;
}

facebook::react::ShadowNode* PickerViewManager::createShadow() const
{
  return new PickerShadowNode();
}

XamlView PickerViewManager::CreateViewCore(int64_t tag)
{
  auto combobox = winrt::ComboBox();
  return combobox;
}

void PickerViewManager::DispatchCommand(XamlView viewToUpdate, int64_t commandId, const folly::dynamic& commandArgs)
{
  auto combobox = viewToUpdate.as<winrt::ComboBox>();
  if (combobox == nullptr)
    return;

  switch (static_cast<PickerCommands>(commandId))
  {
    case PickerCommands::SetFocus:
    {
      combobox.Focus(winrt::FocusState::Programmatic);
      break;
    }
  }
}

YGMeasureFunc PickerViewManager::GetYogaCustomMeasureFunc() const
{
  return DefaultYogaSelfMeasureFunc;
}

}}
