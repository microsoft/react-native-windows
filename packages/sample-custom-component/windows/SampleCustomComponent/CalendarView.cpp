// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "CalendarView.h"

#if defined(RNW_NEW_ARCH)

#include "codegen/react/components/SampleCustomComponent/CalendarView.g.h"

#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt::SampleCustomComponent {

// This is a "toy" implementation of Xaml's CalendarView, just for bringup and testing.
// When UseExperimentalWinUI3 and UseNewArchitecture are both true, it will host a
// CalendarView control in a XamlIsland.  But we only expose a few APIs right now.
// Later, we'll expose this through react-native-xaml.

struct CalendarViewComponentView : public winrt::implements<CalendarViewComponentView, winrt::IInspectable>,
                                   Codegen::BaseCalendarView<CalendarViewComponentView> {
  void InitializeContentIsland(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};
    m_calendarView = winrt::Microsoft::UI::Xaml::Controls::CalendarView{};
    m_xamlIsland.Content(m_calendarView);
    islandView.Connect(m_xamlIsland.ContentIsland());

    m_calendarView.SelectedDatesChanged([this](auto &&, auto &&) {
      if (auto emitter = EventEmitter()) {
        Codegen::CalendarViewEventEmitter::OnSelectedDatesChanged args;
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
        emitter->onSelectedDatesChanged(std::move(args));
      }
    });
  }

 private:
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
  winrt::Microsoft::UI::Xaml::Controls::CalendarView m_calendarView{nullptr};
};

} // namespace winrt::SampleCustomComponent

void RegisterCalendarViewComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  winrt::SampleCustomComponent::Codegen::RegisterCalendarViewNativeComponent<
      winrt::SampleCustomComponent::CalendarViewComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::SampleCustomComponent::CalendarViewComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
      });
}

#endif // defined(RNW_NEW_ARCH)
