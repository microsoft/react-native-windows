#include "pch.h"

#include "XamlCalendarView.h"

#include <winrt/Microsoft.ReactNative.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::XamlCalendarView
{

struct CalendarView : public winrt::implements<CalendarView, winrt::Microsoft::ReactNative::Xaml::IXamlControl, winrt::IInspectable>, 
                    Codegen::BaseCalendarView<CalendarView> {

  winrt::Microsoft::UI::Xaml::UIElement GetXamlElement() {
    if (m_calendarView == nullptr) {
      m_calendarView = winrt::Microsoft::UI::Xaml::Controls::CalendarView();
    }
    return m_calendarView;
  }

  private:
    winrt::Microsoft::UI::Xaml::Controls::CalendarView m_calendarView{ nullptr };
};


// See https://microsoft.github.io/react-native-windows/docs/native-modules for details on writing native modules

void XamlCalendarView::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

double XamlCalendarView::multiply(double a, double b) noexcept {
  return a * b;
}

void RegisterCalendarViewNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  Codegen::RegisterCalendarViewNativeComponent<winrt::XamlCalendarView::CalendarView>(packageBuilder, {});
}


} // namespace winrt::XamlCalendarView
