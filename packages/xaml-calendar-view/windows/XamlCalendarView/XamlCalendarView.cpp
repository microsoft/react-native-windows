#include "pch.h"

#include "XamlCalendarView.h"

#include <winrt/Microsoft.ReactNative.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.Web.WebView2.Core.h>

namespace winrt::XamlCalendarView
{

struct CalendarView : public winrt::implements<CalendarView, winrt::Microsoft::ReactNative::Xaml::IXamlControl, winrt::IInspectable>, 
                    public Codegen::BaseCalendarView<CalendarView> {

  winrt::Microsoft::UI::Xaml::UIElement GetXamlElement() {
    if (m_datePicker == nullptr) {
      CreateXamlCalendarView();
    }
    return m_webView2;
  }

  void UpdateProps(
    const winrt::Microsoft::ReactNative::ComponentView &view,
    const winrt::com_ptr<Codegen::CalendarViewProps> &newProps,
    const winrt::com_ptr<Codegen::CalendarViewProps> &oldProps) noexcept override {
    Codegen::BaseCalendarView<CalendarView>::UpdateProps(view, newProps, oldProps);
    
    /*
    if (m_calendarView && newProps) {
      m_calendarView.DisplayMode(static_cast<winrt::Microsoft::UI::Xaml::Controls::CalendarViewDisplayMode>(
          newProps->displayMode));
    }
          */
  }

  void CreateXamlCalendarView() {
    m_datePicker = winrt::Microsoft::UI::Xaml::Controls::DatePicker();

    m_webView2 = winrt::Microsoft::UI::Xaml::Controls::WebView2();


            m_webView2.CoreWebView2Initialized([](winrt::Microsoft::UI::Xaml::Controls::WebView2 const& sender, 
                winrt::Microsoft::UI::Xaml::Controls::CoreWebView2InitializedEventArgs const& args)
                {
                        sender.CoreWebView2().Navigate(L"https://github.com");
                });
            
            // Initialize WebView2 environment
            m_webView2.EnsureCoreWebView2Async(nullptr);

    /*
    m_calendarView = winrt::Microsoft::UI::Xaml::Controls::CalendarView();
    if (Props()) {
      m_calendarView.DisplayMode(
        static_cast<winrt::Microsoft::UI::Xaml::Controls::CalendarViewDisplayMode>(
        Props()->displayMode));
    }
    m_calendarView.SelectedDatesChanged([this](auto &&, auto &&) {
      if (auto emitter = EventEmitter()) {
        Codegen::CalendarView_OnSelectedDatesChanged args;
        auto selectedDates = m_calendarView.SelectedDates();
        if (selectedDates.Size() == 0) {
          args.startDate = "(none)";
        } else {
          auto firstSelectedDate = selectedDates.GetAt(0);

          auto tt = winrt::clock::to_time_t(firstSelectedDate);
          tm local{};
          localtime_s(&local, &tt);
          auto timeStr = std::put_time(&local, "%F");

          std::stringstream ss;
          ss << (timeStr._Tptr->tm_year + 1900) << "-" << (timeStr._Tptr->tm_mon + 1) << "-" << timeStr._Tptr->tm_mday;

          args.startDate = ss.str();
        }
        emitter->onSelectedDatesChanged(args);
      }
    });
    */
  }


  private:
    //winrt::Microsoft::UI::Xaml::Controls::CalendarView m_calendarView{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::DatePicker m_datePicker{ nullptr };
    winrt::Microsoft::UI::Xaml::Controls::WebView2 m_webView2{ nullptr };
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
