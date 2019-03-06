// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DatePickerViewManager.h"
#include <Views/ShadowNodeBase.h>

#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.Globalization.h>
#include <winrt/Windows.Globalization.DateTimeFormatting.h>
#include <winrt/Windows.Foundation.h>

namespace winrt {
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::Globalization;
  using namespace Windows::Globalization::DateTimeFormatting;
  using namespace Windows::Foundation;
}

namespace react { namespace uwp {

class DatePickerShadowNode : public ShadowNodeBase
{
  using Super = ShadowNodeBase;
public:
  DatePickerShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic&& props) override;

private:
  static void OnDateChanged(
    winrt::CalendarDatePicker const& picker,
    IReactInstance& instance,
    int64_t tag,
    winrt::DateTime const& newDate);
};

void DatePickerShadowNode::createView()
{
  Super::createView();

  auto datePicker = GetView().as<winrt::CalendarDatePicker>();
  auto wkinstance = GetViewManager()->GetReactInstance();

  datePicker.DateChanged([=](winrt::CalendarDatePicker picker, winrt::CalendarDatePickerDateChangedEventArgs args)
  {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr && args.NewDate() != nullptr)
      OnDateChanged(picker, *instance, m_tag, args.NewDate().Value());
  });
}

void DatePickerShadowNode::updateProperties(const folly::dynamic&& props)
{
  m_updating = true;
  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void DatePickerShadowNode::OnDateChanged(
  winrt::CalendarDatePicker const& picker,
  IReactInstance& instance,
  int64_t tag,
  winrt::DateTime const& newDate)
{
  auto newValue = DateTimeToDynamic(newDate);
  if (!newValue.isNull())
  {
    folly::dynamic eventData = folly::dynamic::object("target", tag)("newDate", newValue);
    instance.DispatchEvent(tag, "topChange", std::move(eventData));
  }
}

DatePickerViewManager::DatePickerViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* DatePickerViewManager::GetName() const
{
  return "RCTDatePicker";
}

folly::dynamic DatePickerViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("dayOfWeekFormat", "string")
    ("dateFormat", "string")
    ("firstDayOfWeek", "number")
    ("maxDate", "string")
    ("minDate", "string")
    ("placeholderText", "string")
    ("selectedDate", "string")
  );

  return props;
}

facebook::react::ShadowNode* DatePickerViewManager::createShadow() const
{
  return new DatePickerShadowNode();
}

XamlView DatePickerViewManager::CreateViewCore(int64_t tag)
{
  auto datePicker = winrt::CalendarDatePicker();
  return datePicker;
}

void DatePickerViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
{
  auto datePicker = nodeToUpdate->GetView().as<winrt::CalendarDatePicker>();
  if (datePicker == nullptr)
    return;

  for (auto& pair : reactDiffMap.items())
  {
    const folly::dynamic& propertyName = pair.first;
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName.asString() == "dayOfWeekFormat")
    {
      if (propertyValue.isString())
        datePicker.DayOfWeekFormat(asHstring(propertyValue));
    }
    else if (propertyName.asString() == "dateFormat")
    {
      if (propertyValue.isString())
        datePicker.DateFormat(asHstring(propertyValue));
    }
    else if (propertyName.asString() == "firstDayOfWeek")
    {
      if (propertyValue.isInt())
        datePicker.FirstDayOfWeek(static_cast<winrt::DayOfWeek>(propertyValue.asInt()));
    }
    else if (propertyName.asString() == "maxDate")
    {
      if (propertyValue.isString())
        datePicker.MaxDate(DateTimeFrom(propertyValue));
    }
    else if (propertyName.asString() == "minDate")
    {
      if (propertyValue.isString())
        datePicker.MinDate(DateTimeFrom(propertyValue));
    }
    else if (propertyName.asString() == "placeholderText")
    {
      if (propertyValue.isString())
        datePicker.PlaceholderText(asHstring(propertyValue));
    }
    else if (propertyName.asString() == "selectedDate")
    {
      if (propertyValue.isString())
        datePicker.Date(DateTimeFrom(propertyValue));
    }
  }
  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

}}
