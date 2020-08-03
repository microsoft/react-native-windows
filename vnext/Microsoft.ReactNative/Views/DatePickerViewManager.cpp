// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "DatePickerViewManager.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

namespace winrt {
using DayOfWeek = Windows::Globalization::DayOfWeek;
}

namespace react::uwp {

class DatePickerShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  DatePickerShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

 private:
  void OnDateChanged(IReactInstance &instance, int64_t tag, winrt::DateTime const &newDate);

  int64_t m_selectedTime{0}, m_maxTime{0}, m_minTime{0}; // These values are expected to be in milliseconds
  int64_t m_timeZoneOffsetInSeconds = 0; // Timezone offset is expected to be in seconds

  xaml::Controls::CalendarDatePicker::DateChanged_revoker m_dataPickerDateChangedRevoker{};
};

void DatePickerShadowNode::createView() {
  Super::createView();

  auto datePicker = GetView().as<xaml::Controls::CalendarDatePicker>();
  auto wkinstance = GetViewManager()->GetReactInstance();

  m_dataPickerDateChangedRevoker = datePicker.DateChanged(
      winrt::auto_revoke,
      [=](xaml::Controls::CalendarDatePicker /*picker*/, xaml::Controls::CalendarDatePickerDateChangedEventArgs args) {
        auto instance = wkinstance.lock();
        if (!m_updating && instance != nullptr && args.NewDate() != nullptr)
          OnDateChanged(*instance, m_tag, args.NewDate().Value());
      });
}

void DatePickerShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;

  auto datePicker = GetView().as<xaml::Controls::CalendarDatePicker>();
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
        datePicker.DayOfWeekFormat(asHstring(propertyValue));
      else if (propertyValue.isNull())
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::DayOfWeekFormatProperty());
    } else if (propertyName == "dateFormat") {
      if (propertyValue.isString())
        datePicker.DateFormat(asHstring(propertyValue));
      else if (propertyValue.isNull())
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::DateFormatProperty());
    } else if (propertyName == "firstDayOfWeek") {
      if (propertyValue.isNumber())
        datePicker.FirstDayOfWeek(static_cast<winrt::DayOfWeek>(static_cast<int64_t>(propertyValue.asDouble())));
      else if (propertyValue.isNull())
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::FirstDayOfWeekProperty());
    } else if (propertyName == "maxDate") {
      if (propertyValue.isNumber()) {
        m_maxTime = static_cast<int64_t>(propertyValue.asDouble());
        updateMaxDate = true;
      } else if (propertyValue.isNull()) {
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::MaxDateProperty());
      }
    } else if (propertyName == "minDate") {
      if (propertyValue.isNumber()) {
        m_minTime = static_cast<int64_t>(propertyValue.asDouble());
        updateMinDate = true;
      } else if (propertyValue.isNull()) {
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::MinDateProperty());
      }
    } else if (propertyName == "placeholderText") {
      if (propertyValue.isString())
        datePicker.PlaceholderText(asHstring(propertyValue));
      else if (propertyValue.isNull())
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::PlaceholderTextProperty());
    } else if (propertyName == "selectedDate") {
      if (propertyValue.isNumber()) {
        m_selectedTime = static_cast<int64_t>(propertyValue.asDouble());
        updateSelectedDate = true;
      } else if (propertyValue.isNull()) {
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::DateProperty());
      }
    } else if (propertyName == "timeZoneOffsetInSeconds") {
      if (propertyValue.isNumber())
        m_timeZoneOffsetInSeconds = static_cast<int64_t>(propertyValue.asDouble());
      else if (propertyValue.isNull())
        m_timeZoneOffsetInSeconds = 0;
    }
  }

  if (updateMaxDate)
    datePicker.MaxDate(DateTimeFrom(m_maxTime, m_timeZoneOffsetInSeconds));

  if (updateMinDate)
    datePicker.MinDate(DateTimeFrom(m_minTime, m_timeZoneOffsetInSeconds));

  if (updateSelectedDate)
    datePicker.Date(DateTimeFrom(m_selectedTime, m_timeZoneOffsetInSeconds));

  Super::updateProperties(std::move(props));
  m_updating = false;
}

void DatePickerShadowNode::OnDateChanged(IReactInstance &instance, int64_t tag, winrt::DateTime const &newDate) {
  auto timeInMilliseconds = DateTimeToDynamic(newDate, m_timeZoneOffsetInSeconds);
  if (!timeInMilliseconds.isNull()) {
    folly::dynamic eventData = folly::dynamic::object("target", tag)("newDate", timeInMilliseconds);
    instance.DispatchEvent(tag, "topChange", std::move(eventData));
  }
}

DatePickerViewManager::DatePickerViewManager(const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *DatePickerViewManager::GetName() const {
  return "RCTDatePicker";
}

folly::dynamic DatePickerViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("dayOfWeekFormat", "string")("dateFormat", "string")("firstDayOfWeek", "number")(
      "maxDate", "number")("minDate", "number")("placeholderText", "string")("selectedDate", "number")(
      "timeZoneOffsetInSeconds", "number"));

  return props;
}

facebook::react::ShadowNode *DatePickerViewManager::createShadow() const {
  return new DatePickerShadowNode();
}

XamlView DatePickerViewManager::CreateViewCore(int64_t /*tag*/) {
  auto datePicker = xaml::Controls::CalendarDatePicker();
  return datePicker;
}

YGMeasureFunc DatePickerViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

} // namespace react::uwp
