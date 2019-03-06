// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "CalendarViewViewManager.h"
#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt {
  using namespace Windows::Foundation;
  using namespace Windows::UI::Text;
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
}

namespace react { namespace uwp {

class CalendarViewShadowNode : public ShadowNodeBase
{
  using Super = ShadowNodeBase;
public:
  CalendarViewShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic&& props) override;

private:
  static void OnSelectedDateChanged(IReactInstance& instance, int64_t tag, winrt::DateTime const& selectedDate);
};

void CalendarViewShadowNode::createView()
{
  Super::createView();

  auto calendarView = GetView().as<winrt::CalendarView>();
  auto wkinstance = GetViewManager()->GetReactInstance();

  calendarView.SelectedDatesChanged([=](winrt::CalendarView calendarView, winrt::CalendarViewSelectedDatesChangedEventArgs)
  {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
    {
      winrt::Collections::IVector<winrt::DateTime> selectedDates = calendarView.SelectedDates();
      if (selectedDates.Size() > 0)
        OnSelectedDateChanged(*instance, m_tag, selectedDates.GetAt(0));
    }
  });

  // Adjust day item style with zero values of MinWidth and MinHeight - without that adjustment CalendarView does not
  // auto-size below certain thresholds.
  auto dayItemStyle = winrt::Style(winrt::xaml_typename<winrt::CalendarViewDayItem>());
  winrt::Windows::Foundation::IInspectable boxed0(winrt::box_value<double>(0));
  dayItemStyle.Setters().Append(winrt::Setter(winrt::FrameworkElement::MinWidthProperty(), boxed0));
  dayItemStyle.Setters().Append(winrt::Setter(winrt::FrameworkElement::MinHeightProperty(), boxed0));
  calendarView.CalendarViewDayItemStyle(dayItemStyle);
}

void CalendarViewShadowNode::updateProperties(const folly::dynamic&& props)
{
  m_updating = true;
  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void CalendarViewShadowNode::OnSelectedDateChanged(IReactInstance& instance, int64_t tag, winrt::DateTime const& selectedDate)
{
  auto newValue = DateTimeToDynamic(selectedDate);

  if (!newValue.isNull())
  {
    folly::dynamic eventData = folly::dynamic::object("target", tag)("selectedDate", newValue);
    instance.DispatchEvent(tag, "topChange", std::move(eventData));
  }
  else
  {
    assert(0);
  }
}

CalendarViewViewManager::CalendarViewViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* CalendarViewViewManager::GetName() const
{
  return "RCTCalendarView";
}

folly::dynamic CalendarViewViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  // For the sake of consistency, please keep expressions below in alphabetical order (on property names).
  props.update(folly::dynamic::object
    ("calendarIdentifier", "string")
    ("calendarItemBackgroundColor", "number")
    ("calendarItemBorderColor", "number")
    ("calendarItemBorderThickness", "string")
    ("calendarItemColor", "number")
    ("dayItemFontFamily", "string")
    ("dayItemFontSize", "number")
    ("dayItemFontWeight", "number")
    ("dayOfWeekFormat", "string")
    ("firstDayOfWeek", "number")
    ("firstOfMonthLabelFontFamily", "string")
    ("firstOfMonthLabelFontSize", "number")
    ("firstOfMonthLabelFontWeight", "number")
    ("firstOfYearDecadeLabelFontFamily", "string")
    ("firstOfYearDecadeLabelFontSize", "number")
    ("firstOfYearDecadeLabelFontWeight", "number")
    ("focusBorderColor", "number")
    ("horizontalDayItemAlignment", "string")
    ("horizontalFirstOfMonthLabelAlignment", "string")
    ("hoverBorderColor", "number")
    ("isGroupLabelVisible", "boolean")
    ("isOutOfScopeEnabled", "boolean")
    ("isTodayHighlighted", "boolean")
    ("maxDate", "string")
    ("minDate", "string")
    ("monthYearItemFontFamily", "string")
    ("monthYearItemFontSize", "number")
    ("monthYearItemFontWeight", "number")
    ("numberOfWeeksInView", "number")
    ("outOfScopeColor", "number")
    ("outOfScopeBackgroundColor", "number")
    ("pressedBorderColor", "number")
    ("pressedColor", "number")
    ("selectedBorderColor", "number")
    ("selectedColor", "number")
    ("selectedDate", "string")
    ("selectedHoverBorderColor", "number")
    ("selectedPressedBorderColor", "number")
    ("todayColor", "number")
    ("todayFontWeight", "string")
    ("verticalDayItemAlignment", "string")
    ("verticalFirstOfMonthLabelAlignment", "string")
  );

  return props;
}

facebook::react::ShadowNode* CalendarViewViewManager::createShadow() const
{
  return new CalendarViewShadowNode();
}

XamlView CalendarViewViewManager::CreateViewCore(int64_t tag)
{
  auto calendarView = winrt::CalendarView();
  return calendarView;
}

void CalendarViewViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
{
  auto calendarView = nodeToUpdate->GetView().as<winrt::CalendarView>();
  if (calendarView == nullptr)
    return;

  for (auto& pair : reactDiffMap.items())
  {
    const folly::dynamic& propertyName = pair.first;
    const folly::dynamic& propertyValue = pair.second;

    // For the sake of consistency, please keep conditions below in alphabetical order (on property names).
    if (propertyName == "calendarIdentifier")
    {
      if (propertyValue.isString())
        calendarView.CalendarIdentifier(asHstring(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::CalendarIdentifierProperty());
      else
        assert(false);  // calendarIndentifier?: string - we don't expect anything but string
    }
    else if (propertyName == "calendarItemBackgroundColor")
    {
      if (propertyValue.isInt())
        calendarView.CalendarItemBackground(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::CalendarItemBackgroundProperty());
      else
        assert(false);  // calendarItemBackgroundColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "calendarItemBorderColor")
    {
      if (propertyValue.isInt())
        calendarView.CalendarItemBorderBrush(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::CalendarItemBorderBrushProperty());
      else
        assert(false);  // calendarItemBorderColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "calendarItemBorderThickness")
    {
      if (propertyValue.isInt() || propertyValue.isDouble())
      {
        double thickness = propertyValue.isInt() ? propertyValue.asInt() : propertyValue.asDouble();
        calendarView.CalendarItemBorderThickness(
          { thickness /*Left*/, thickness /*Top*/, thickness /*Right*/, thickness /*Bottom*/ });
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::CalendarItemBorderThicknessProperty());
      }
      else
      {
        assert(false);  // calendarItemBorderThickness?: number - we don't expect anything but int or double
      }
    }
    else if (propertyName == "calendarItemColor")
    {
      if (propertyValue.isInt())
        calendarView.CalendarItemForeground(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::CalendarItemForegroundProperty());
      else
        assert(false);  // calendarItemColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "dayItemFontFamily")
    {
      if (propertyValue.isString())
        calendarView.DayItemFontFamily(winrt::Media::FontFamily(asHstring(propertyValue)));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::DayItemFontFamilyProperty());
      else
        assert(false);  // dayItemFontFamily?: string - we don't expect anything but string
    }
    else if (propertyName == "dayItemFontSize")
    {
      if (propertyValue.isInt())
        calendarView.DayItemFontSize(propertyValue.asInt());
      else if (propertyValue.isDouble())
        calendarView.DayItemFontSize(propertyValue.asDouble());
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::DayItemFontSizeProperty());
      else
        assert(false);  // dayItemFontSize?: number - we don't expect anything but int or double
    }
    else if (propertyName == "dayItemFontWeight")
    {
      if (propertyValue.isInt())
      {
        winrt::FontWeight fontWeight;
        fontWeight.Weight = propertyValue.asInt();
        calendarView.DayItemFontWeight(fontWeight);
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::DayItemFontWeightProperty());
      }
      else
      {
        assert(false);  // dayItemFontWeight?: number - we don't expect anything but int
      }
    }
    else if (propertyName == "dayOfWeekFormat")
    {
      if (propertyValue.isString())
        calendarView.DayOfWeekFormat(asHstring(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::DayOfWeekFormatProperty());
      else
        assert(false);  // dayOfWeekFormat?: string - we don't expect anything but string
    }
    else if (propertyName == "firstDayOfWeek")
    {
      if (propertyValue.isInt())
      {
        int value = propertyValue.asInt();
        if (value >= static_cast<int>(winrt::Windows::Globalization::DayOfWeek::Sunday)
          && value <= static_cast<int>(winrt::Windows::Globalization::DayOfWeek::Saturday))
        {
          calendarView.FirstDayOfWeek(static_cast<winrt::Windows::Globalization::DayOfWeek>(value));
        }
        else
        {
          assert(false);  // firstDayOfWeek?: 0 | 1 | 2 | 3 | 4 | 5 | 6 - we don't expect anything else
        }
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::FirstDayOfWeekProperty());
      }
      else
      {
        assert(false);  // firstDayOfWeek?: 0 | 1 | 2 | 3 | 4 | 5 | 6 - we don't expect anything else
      }
    }
    else if (propertyName == "firstOfMonthLabelFontFamily")
    {
      if (propertyValue.isString())
        calendarView.FirstOfMonthLabelFontFamily(winrt::Media::FontFamily(asHstring(propertyValue)));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::FirstOfMonthLabelFontFamilyProperty());
      else
        assert(false);  // firstOfMonthLabelFontFamily?: string - we don't expect anything but string
    }
    else if (propertyName == "firstOfMonthLabelFontSize")
    {
      if (propertyValue.isInt())
        calendarView.FirstOfMonthLabelFontSize(propertyValue.asInt());
      else if (propertyValue.isDouble())
        calendarView.FirstOfMonthLabelFontSize(propertyValue.asDouble());
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::FirstOfMonthLabelFontSizeProperty());
      else
        assert(false);  // firstOfMonthLabelFontSize?: number - we don't expect anything but int or double
    }
    else if (propertyName == "firstOfMonthFontWeight")
    {
      if (propertyValue.isInt())
      {
        winrt::FontWeight fontWeight;
        fontWeight.Weight = propertyValue.asInt();
        calendarView.FirstOfMonthLabelFontWeight(fontWeight);
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::FirstOfMonthLabelFontWeightProperty());
      }
      else
      {
        assert(false);  // firstOfMonthFontWeight?: number - we don't expect anything but int
      }
    }
    else if (propertyName == "firstOfYearDecadeLabelFontFamily")
    {
      if (propertyValue.isString())
        calendarView.FirstOfMonthLabelFontFamily(winrt::Media::FontFamily(asHstring(propertyValue)));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::FirstOfMonthLabelFontFamilyProperty());
      else
        assert(false);  // firstOfYearDecadeLabelFontFamily?: string - we don't expect anything but string
    }
    else if (propertyName == "firstOfYearDecadeLabelFontSize")
    {
      if (propertyValue.isInt())
        calendarView.FirstOfYearDecadeLabelFontSize(propertyValue.asInt());
      else if (propertyValue.isDouble())
        calendarView.FirstOfYearDecadeLabelFontSize(propertyValue.asDouble());
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::FirstOfYearDecadeLabelFontSizeProperty());
      else
        assert(false);  // firstOfYearDecadeLabelFontSize?: number - we don't expect anything but int or double
    }
    else if (propertyName == "firstOfYearDecadeFontWeight")
    {
      if (propertyValue.isInt())
      {
        winrt::FontWeight fontWeight;
        fontWeight.Weight = propertyValue.asInt();
        calendarView.FirstOfYearDecadeLabelFontWeight(fontWeight);
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::FirstOfYearDecadeLabelFontWeightProperty());
      }
      else
      {
        assert(false);  // firstOfYearDecadeFontWeight?: number - we don't expect anything but int
      }
    }
    else if (propertyName == "focusBorderColor")
    {
      if (propertyValue.isInt())
        calendarView.FocusBorderBrush(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::FocusBorderBrushProperty());
      else
        assert(false);  // focusBorderColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "horizontalDayItemAlignment")
    {
      if (propertyValue.isString())
        calendarView.HorizontalDayItemAlignment(HorizontalAlignmentFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::HorizontalDayItemAlignmentProperty());
      else
        assert(false);  // horizontalDayItemAlignment?: "center" | "left" | "right" | "stretch" - we don't expect anything else
    }
    else if (propertyName == "horizontalFirstOfMonthLabelAlignment")
    {
      if (propertyValue.isString())
        calendarView.HorizontalFirstOfMonthLabelAlignment(HorizontalAlignmentFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::HorizontalFirstOfMonthLabelAlignmentProperty());
      else
        assert(false);  // horizontalFirstOfMonthLabelAlignment?: "center" | "left" | "right" | "stretch" - we don't expect anything else
    }
    else if (propertyName == "hoverBorderColor")
    {
      if (propertyValue.isString())
        calendarView.HoverBorderBrush(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::HoverBorderBrushProperty());
      else
        assert(false);  // hoverBorderColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "isGroupLabelVisible")
    {
      if (propertyValue.isBool())
        calendarView.IsGroupLabelVisible(propertyValue.asBool());
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::IsGroupLabelVisibleProperty());
      else
        assert(false);  // isGroupLabelVisible?: boolean - we don't expect anything but bool
    }
    else if (propertyName == "isOutOfScopeEnabled")
    {
      if (propertyValue.isBool())
        calendarView.IsOutOfScopeEnabled(propertyValue.asBool());
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::IsOutOfScopeEnabledProperty());
      else
        assert(false);  // isOutOfScopeEnabled?: boolean - we don't expect anything but bool
    }
    else if (propertyName == "isTodayHighlighted")
    {
      if (propertyValue.isBool())
        calendarView.IsTodayHighlighted(propertyValue.asBool());
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::IsTodayHighlightedProperty());
      else
        assert(false);  // isTodayHighlighted?: boolean - we don't expect anything but bool
    }
    else if (propertyName == "maxDate")
    {
      if (propertyValue.isString())
      {
        winrt::DateTime value = DateTimeFrom(propertyValue);
        if (value != winrt::DateTime())
          calendarView.MaxDate(value);
        else
          assert(false);  // we expect a valid date value
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::MaxDateProperty());
      }
      else
      {
        assert(false);  // maxDate?: string - we don't expect anything but string
      }
    }
    else if (propertyName == "minDate")
    {
      if (propertyValue.isString())
      {
        winrt::DateTime value = DateTimeFrom(propertyValue);
        if (value != winrt::DateTime())
          calendarView.MinDate(value);
        else
          assert(false);  // we expect a valid date value
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::MinDateProperty());
      }
      else
      {
        assert(false);  // minDate?: string - we don't expect anything but string
      }
    }
    else if (propertyName == "monthYearItemFontFamily")
    {
      if (propertyValue.isString())
        calendarView.MonthYearItemFontFamily(winrt::Media::FontFamily(asHstring(propertyValue)));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::MonthYearItemFontFamilyProperty());
      else
        assert(false);  // monthYearItemFontFamily?: string - we don't expect anything but string
    }
    else if (propertyName == "monthYearItemFontSize")
    {
      if (propertyValue.isInt())
        calendarView.MonthYearItemFontSize(propertyValue.asInt());
      else if (propertyValue.isDouble())
        calendarView.MonthYearItemFontSize(propertyValue.asDouble());
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::MonthYearItemFontSizeProperty());
      else
        assert(false);  // monthYearItemFontSize?: number - we don't expect anything but int or double
    }
    else if (propertyName == "monthYearItemFontWeight")
    {
      if (propertyValue.isInt())
      {
        winrt::FontWeight fontWeight;
        fontWeight.Weight = propertyValue.asInt();
        calendarView.MonthYearItemFontWeight(fontWeight);
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::MonthYearItemFontWeightProperty());
      }
      else
      {
        assert(false);  // monthYearItemFontWeight?: number - we don't expect anything but int
      }
    }
    else if (propertyName == "numberOfWeeksInView")
    {
      if (propertyValue.isInt())
      {
        int value = propertyValue.asInt();
        if (value >= 1 && value <= 10)
          calendarView.NumberOfWeeksInView(value);
        else
          assert(false);  // numberOfWeeksInView?: 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 - we don't expect anything else
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::NumberOfWeeksInViewProperty());
      }
      else
      {
        assert(false);  // numberOfWeeksInView?: 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 - we don't expect anything else
      }
    }
    else if (propertyName == "outOfScopeColor")
    {
      if (propertyValue.isString())
        calendarView.OutOfScopeForeground(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::OutOfScopeForegroundProperty());
      else
        assert(false);  // outOfScopeColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "outOfScopeBackgroundColor")
    {
      if (propertyValue.isString())
        calendarView.OutOfScopeBackground(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::OutOfScopeBackgroundProperty());
      else
        assert(false);  // outOfScopeBackgroundColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "pressedBorderColor")
    {
      if (propertyValue.isString())
        calendarView.PressedBorderBrush(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::PressedBorderBrushProperty());
      else
        assert(false);  // pressedBorderColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "pressedColor")
    {
      if (propertyValue.isString())
        calendarView.PressedForeground(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::PressedForegroundProperty());
      else
        assert(false);  // pressedColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "selectedBorderColor")
    {
      if (propertyValue.isString())
        calendarView.SelectedBorderBrush(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::SelectedBorderBrushProperty());
      else
        assert(false);  // selectedBorderColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "selectedColor")
    {
      if (propertyValue.isString())
        calendarView.SelectedForeground(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::SelectedForegroundProperty());
      else
        assert(false);  // selectedColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "selectedDate")
    {
      if (propertyValue.isString())
      {
        winrt::DateTime value = DateTimeFrom(propertyValue);
        if (value != winrt::DateTime())
        {
          calendarView.SelectedDates().Clear();
          calendarView.SelectedDates().InsertAt(0, value);
        }
      }
      else if (propertyValue.isNull())
      {
        calendarView.SelectedDates().Clear();
      }
      else
      {
        assert(false);  // selectedDate?: string - we don't expect anything but string
      }
    }
    else if (propertyName == "selectedHoverBorderColor")
    {
      if (propertyValue.isString())
        calendarView.SelectedHoverBorderBrush(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::SelectedHoverBorderBrushProperty());
      else
        assert(false);  // selectedHoverBorderColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "selectedPressedBorderColor")
    {
      if (propertyValue.isString())
        calendarView.SelectedPressedBorderBrush(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::SelectedPressedBorderBrushProperty());
      else
        assert(false);  // selectedPressedBorderColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "todayColor")
    {
      if (propertyValue.isString())
        calendarView.TodayForeground(BrushFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::TodayForegroundProperty());
      else
        assert(false);  // todayColor?: string - we don't expect anything but int after processColor conversion
    }
    else if (propertyName == "todayFontWeight")
    {
      if (propertyValue.isInt())
      {
        winrt::FontWeight fontWeight;
        fontWeight.Weight = propertyValue.asInt();
        calendarView.TodayFontWeight(fontWeight);
      }
      else if (propertyValue.isNull())
      {
        calendarView.ClearValue(winrt::CalendarView::TodayFontWeightProperty());
      }
      else
      {
        assert(false);  // todayFontWeight?: number - we don't expect anything but int
      }
    }
    else if (propertyName == "verticalDayItemAlignment")
    {
      if (propertyValue.isString())
        calendarView.VerticalDayItemAlignment(VerticalAlignmentFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::VerticalDayItemAlignmentProperty());
      else
        assert(false);  // verticalDayItemAlignment?: "bottom" | "center" | "stretch" | "top" - we don't expect anything else
    }
    else if (propertyName == "verticalFirstOfMonthLabelAlignment")
    {
      if (propertyValue.isString())
        calendarView.VerticalFirstOfMonthLabelAlignment(VerticalAlignmentFrom(propertyValue));
      else if (propertyValue.isNull())
        calendarView.ClearValue(winrt::CalendarView::VerticalFirstOfMonthLabelAlignmentProperty());
      else
        assert(false);  // verticalFirstOfMonthLabelAlignment?: "bottom" | "center" | "stretch" | "top" - we don't expect anything else
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

}}
