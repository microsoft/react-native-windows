// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "DatePickerViewManager.h"

#include <Utils/ValueUtils.h>
#include <Utils/XamlDirect.h>

#include <IReactInstance.h>

namespace winrt {
using DayOfWeek = Windows::Globalization::DayOfWeek;
}

namespace react {
namespace uwp {

class DatePickerShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  DatePickerShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

 private:
  void OnDateChanged(
      IReactInstance &instance,
      int64_t tag,
      winrt::DateTime const &newDate);

  int64_t m_selectedTime, m_maxTime,
      m_minTime; // These values are expected to be in milliseconds
  int64_t m_timeZoneOffsetInSeconds =
      0; // Timezone offset is expected to be in seconds

  winrt::CalendarDatePicker::DateChanged_revoker
      m_dataPickerDateChangedRevoker{};
};

void DatePickerShadowNode::createView() {
  Super::createView();

  auto datePicker = GetView().as<winrt::CalendarDatePicker>();
  auto wkinstance = GetViewManager()->GetReactInstance();

  m_dataPickerDateChangedRevoker = datePicker.DateChanged(
      winrt::auto_revoke,
      [=](winrt::CalendarDatePicker /*picker*/,
          winrt::CalendarDatePickerDateChangedEventArgs args) {
        auto instance = wkinstance.lock();
        if (!m_updating && instance != nullptr && args.NewDate() != nullptr)
          OnDateChanged(*instance, m_tag, args.NewDate().Value());
      });
}

void DatePickerShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  const auto datePicker = GetXamlDirect().GetXamlDirectObject(
      GetView().as<winrt::CalendarDatePicker>());

  if (datePicker == nullptr)
    return;

  bool updateSelectedDate = false;
  bool updateMaxDate = false;
  bool updateMinDate = false;

  for (auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "dayOfWeekFormat") {
      if (propertyValue.isString())
        GetXamlDirect().SetStringProperty(
            datePicker,
            XDPropertyIndex::CalendarDatePicker_DayOfWeekFormat,
            asHstring(propertyValue));
      else if (propertyValue.isNull())
        GetXamlDirect().ClearProperty(
            datePicker, XDPropertyIndex::CalendarDatePicker_DayOfWeekFormat);
    } else if (propertyName == "dateFormat") {
      if (propertyValue.isString())
        GetXamlDirect().SetStringProperty(
            datePicker,
            XDPropertyIndex::CalendarDatePicker_DateFormat,
            asHstring(propertyValue));
      else if (propertyValue.isNull())
        GetXamlDirect().ClearProperty(
            datePicker, XDPropertyIndex::CalendarDatePicker_DateFormat);
    } else if (propertyName == "firstDayOfWeek") {
      if (propertyValue.isNumber())
        GetXamlDirect().SetEnumProperty(
            datePicker,
            XDPropertyIndex::CalendarDatePicker_FirstDayOfWeek,
            static_cast<int32_t>(propertyValue.asDouble()));
      else if (propertyValue.isNull())
        GetXamlDirect().ClearProperty(
            datePicker, XDPropertyIndex::CalendarDatePicker_FirstDayOfWeek);
    } else if (propertyName == "maxDate") {
      if (propertyValue.isNumber()) {
        m_maxTime = static_cast<int64_t>(propertyValue.asDouble());
        updateMaxDate = true;
      } else if (propertyValue.isNull()) {
        GetXamlDirect().ClearProperty(
            datePicker, XDPropertyIndex::CalendarDatePicker_MaxDate);
      }
    } else if (propertyName == "minDate") {
      if (propertyValue.isNumber()) {
        m_minTime = static_cast<int64_t>(propertyValue.asDouble());
        updateMinDate = true;
      } else if (propertyValue.isNull()) {
        GetXamlDirect().ClearProperty(
            datePicker, XDPropertyIndex::CalendarDatePicker_MinDate);
      }
    } else if (propertyName == "placeholderText") {
      if (propertyValue.isString())
        GetXamlDirect().SetStringProperty(
            datePicker,
            XDPropertyIndex::CalendarDatePicker_PlaceholderText,
            asHstring(propertyValue));
      else if (propertyValue.isNull())
        GetXamlDirect().ClearProperty(
            datePicker, XDPropertyIndex::CalendarDatePicker_PlaceholderText);
    } else if (propertyName == "selectedDate") {
      if (propertyValue.isNumber()) {
        m_selectedTime = static_cast<int64_t>(propertyValue.asDouble());
        updateSelectedDate = true;
      } else if (propertyValue.isNull()) {
        GetXamlDirect().ClearProperty(
            datePicker, XDPropertyIndex::CalendarDatePicker_Date);
      }
    } else if (propertyName == "timeZoneOffsetInSeconds") {
      if (propertyValue.isNumber())
        m_timeZoneOffsetInSeconds =
            static_cast<int64_t>(propertyValue.asDouble());
      else if (propertyValue.isNull())
        m_timeZoneOffsetInSeconds = 0;
    }
  }

  if (updateMaxDate)
    GetXamlDirect().SetDateTimeProperty(
        datePicker,
        XDPropertyIndex::CalendarDatePicker_MaxDate,
        DateTimeFrom(m_maxTime, m_timeZoneOffsetInSeconds));

  if (updateMinDate)
    GetXamlDirect().SetDateTimeProperty(
        datePicker,
        XDPropertyIndex::CalendarDatePicker_MinDate,
        DateTimeFrom(m_minTime, m_timeZoneOffsetInSeconds));

  if (updateSelectedDate)
    GetXamlDirect().SetDateTimeProperty(
        datePicker,
        XDPropertyIndex::CalendarDatePicker_Date,
        DateTimeFrom(m_selectedTime, m_timeZoneOffsetInSeconds));

  Super::updateProperties(std::move(props));
  m_updating = false;
}

void DatePickerShadowNode::OnDateChanged(
    IReactInstance &instance,
    int64_t tag,
    winrt::DateTime const &newDate) {
  auto timeInMilliseconds =
      DateTimeToDynamic(newDate, m_timeZoneOffsetInSeconds);
  if (!timeInMilliseconds.isNull()) {
    folly::dynamic eventData =
        folly::dynamic::object("target", tag)("newDate", timeInMilliseconds);
    instance.DispatchEvent(tag, "topChange", std::move(eventData));
  }
}

DatePickerViewManager::DatePickerViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *DatePickerViewManager::GetName() const {
  return "RCTDatePicker";
}

folly::dynamic DatePickerViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("dayOfWeekFormat", "string")(
      "dateFormat", "string")("firstDayOfWeek", "number")("maxDate", "number")(
      "minDate", "number")("placeholderText", "string")(
      "selectedDate", "number")("timeZoneOffsetInSeconds", "number"));

  return props;
}

facebook::react::ShadowNode *DatePickerViewManager::createShadow() const {
  return new DatePickerShadowNode();
}

XamlView DatePickerViewManager::CreateViewCore(int64_t tag) {
  auto datePicker = winrt::CalendarDatePicker();
  return datePicker;
}

YGMeasureFunc DatePickerViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

} // namespace uwp
} // namespace react
