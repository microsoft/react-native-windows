// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "PickerViewManager.h"
#include "ShadowNodeBase.h"

#include <Utils/ValueUtils.h>
#include "UnicodeConversion.h"

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
  PickerShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic&& props) override;

private:
  void RepopulateItems();
  static void OnSelectionChanged(IReactInstance& instance, int64_t tag, folly::dynamic&& value, int32_t selectedIndex);
  bool m_updating = false;
  folly::dynamic m_items;
};

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
      if (index >= 0 && index < m_items.size())
        value = m_items.at(index)["value"];
      OnSelectionChanged(*instance, m_tag, std::move(value), index);
    }
  });
}

void PickerShadowNode::updateProperties(const folly::dynamic&& props)
{
  m_updating = true;
  auto combobox = GetView().as<winrt::ComboBox>();
  for (auto& pair : props.items())
  {
    const folly::dynamic& propertyName = pair.first;
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName.asString() == "enabled")
    {
      if (propertyValue.isBool())
        combobox.IsEnabled(propertyValue.asBool());
    }
    else if (propertyName.asString() == "selectedIndex")
    {
      if (propertyValue.isNumber())
        combobox.SelectedIndex(propertyValue.asInt());
    }
    else if (propertyName.asString() == "items")
    {
      if (propertyValue.isArray())
      {
        m_items = propertyValue;
        RepopulateItems();
      }
    }
  }

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
      comboboxItem.Content(winrt::box_value(facebook::react::UnicodeConversion::Utf8ToUtf16(label)));
      if (item.count("textColor"))
        comboboxItem.Foreground(BrushFrom(item["textColor"]));
      comboBoxItems.Append(comboboxItem);
    }
  }
}

/*static*/ void PickerShadowNode::OnSelectionChanged(IReactInstance& instance, int64_t tag, folly::dynamic&& value, int32_t selectedIndex)
{
  folly::dynamic eventData = folly::dynamic::object
    ("target", tag)
    ("value", std::move(value))
    ("itemIndex", selectedIndex);
  instance.DispatchEvent(tag, "topChange", std::move(eventData));
}

PickerViewManager::PickerViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : FrameworkElementViewManager(reactInstance)
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
    ("enabled", "boolean")
    ("items", "array")
    ("selectedIndex", "number")
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
