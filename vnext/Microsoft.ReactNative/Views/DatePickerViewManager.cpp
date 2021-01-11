// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "DatePickerViewManager.h"

#include <JsValueWriter.h>
#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

namespace winrt {
using DayOfWeek = Windows::Globalization::DayOfWeek;
}

namespace Microsoft::ReactNative {

class DatePickerShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  DatePickerShadowNode() = default;
  void createView() override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;

 private:
  void OnDateChanged(const Mso::React::IReactContext &context, int64_t tag, winrt::DateTime const &newDate);

  int64_t m_selectedTime{0}, m_maxTime{0}, m_minTime{0}; // These values are expected to be in milliseconds
  int64_t m_timeZoneOffsetInSeconds = 0; // Timezone offset is expected to be in seconds

  xaml::Controls::CalendarDatePicker::DateChanged_revoker m_dataPickerDateChangedRevoker{};
};

void DatePickerShadowNode::createView() {
  Super::createView();

  auto datePicker = GetView().as<xaml::Controls::CalendarDatePicker>();

  m_dataPickerDateChangedRevoker = datePicker.DateChanged(
      winrt::auto_revoke,
      [=](xaml::Controls::CalendarDatePicker /*picker*/, xaml::Controls::CalendarDatePickerDateChangedEventArgs args) {
        if (!m_updating && args.NewDate() != nullptr)
          OnDateChanged(GetViewManager()->GetReactContext(), m_tag, args.NewDate().Value());
      });
}

void DatePickerShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  m_updating = true;

  auto datePicker = GetView().as<xaml::Controls::CalendarDatePicker>();
  if (datePicker == nullptr)
    return;

  bool updateSelectedDate = false;
  bool updateMaxDate = false;
  bool updateMinDate = false;

  for (auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;

    if (propertyName == "dayOfWeekFormat") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String)
        datePicker.DayOfWeekFormat(react::uwp::asHstring(propertyValue));
      else if (propertyValue.IsNull())
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::DayOfWeekFormatProperty());
    } else if (propertyName == "dateFormat") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String)
        datePicker.DateFormat(react::uwp::asHstring(propertyValue));
      else if (propertyValue.IsNull())
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::DateFormatProperty());
    } else if (propertyName == "firstDayOfWeek") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
        datePicker.FirstDayOfWeek(static_cast<winrt::DayOfWeek>(propertyValue.AsInt64()));
      else if (propertyValue.IsNull())
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::FirstDayOfWeekProperty());
    } else if (propertyName == "maxDate") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        m_maxTime = static_cast<int64_t>(propertyValue.AsInt64());
        updateMaxDate = true;
      } else if (propertyValue.IsNull()) {
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::MaxDateProperty());
      }
    } else if (propertyName == "minDate") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        m_minTime = propertyValue.AsInt64();
        updateMinDate = true;
      } else if (propertyValue.IsNull()) {
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::MinDateProperty());
      }
    } else if (propertyName == "placeholderText") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String)
        datePicker.PlaceholderText(react::uwp::asHstring(propertyValue));
      else if (propertyValue.IsNull())
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::PlaceholderTextProperty());
    } else if (propertyName == "selectedDate") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        m_selectedTime = propertyValue.AsInt64();
        updateSelectedDate = true;
      } else if (propertyValue.IsNull()) {
        datePicker.ClearValue(xaml::Controls::CalendarDatePicker::DateProperty());
      }
    } else if (propertyName == "timeZoneOffsetInSeconds") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
        m_timeZoneOffsetInSeconds = propertyValue.AsInt64();
      else if (propertyValue.IsNull())
        m_timeZoneOffsetInSeconds = 0;
    }
  }

  if (updateMaxDate)
    datePicker.MaxDate(react::uwp::DateTimeFrom(m_maxTime, m_timeZoneOffsetInSeconds));

  if (updateMinDate)
    datePicker.MinDate(react::uwp::DateTimeFrom(m_minTime, m_timeZoneOffsetInSeconds));

  if (updateSelectedDate)
    datePicker.Date(react::uwp::DateTimeFrom(m_selectedTime, m_timeZoneOffsetInSeconds));

  Super::updateProperties(props);
  m_updating = false;
}

void DatePickerShadowNode::OnDateChanged(
    const Mso::React::IReactContext &context,
    int64_t tag,
    winrt::DateTime const &newDate) {
  auto timeInMilliseconds = react::uwp::DateTimeToDynamic(newDate, m_timeZoneOffsetInSeconds);
  if (!timeInMilliseconds.isNull()) {
    folly::dynamic eventData = folly::dynamic::object("target", tag)("newDate", timeInMilliseconds);
    context.DispatchEvent(tag, "topChange", std::move(eventData));
  }
}

DatePickerViewManager::DatePickerViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *DatePickerViewManager::GetName() const {
  return L"RCTDatePicker";
}

void DatePickerViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"dayOfWeekFormat", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"dateFormat", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"firstDayOfWeek", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"maxDate", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"minDate", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"placeholderText", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"selectedDate", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"timeZoneOffsetInSeconds", L"number");
}

ShadowNode *DatePickerViewManager::createShadow() const {
  return new DatePickerShadowNode();
}

XamlView DatePickerViewManager::CreateViewCore(int64_t /*tag*/) {
  auto datePicker = xaml::Controls::CalendarDatePicker();
  return datePicker;
}

YGMeasureFunc DatePickerViewManager::GetYogaCustomMeasureFunc() const {
  return DefaultYogaSelfMeasureFunc;
}

} // namespace Microsoft::ReactNative
