// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "CalendarView.h"

#if defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)

#include "codegen/react/components/SampleCustomComponent/CalendarView.g.h"
#include "codegen/react/components/SampleCustomComponent/FabricXamlControl.g.h"

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
  }

 private:
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
  winrt::Microsoft::UI::Xaml::Controls::CalendarView m_calendarView{nullptr};
};

struct XamlControlComponentView : public winrt::implements<XamlControlComponentView, winrt::IInspectable>,
                                   Codegen::BaseFabricXamlControl<XamlControlComponentView> {


 private:
};

REACT_STRUCT(CustomXamlControlProps)
struct CustomXamlControlProps : winrt::implements<CustomXamlControlProps, winrt::Microsoft::ReactNative::IComponentProps> {
  CustomXamlControlProps(winrt::Microsoft::ReactNative::ViewProps props, const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom)
    : ViewProps(props)
  {
     if (cloneFrom) {
       auto cloneFromProps = cloneFrom.as<CustomXamlControlProps>();
       xamlTypeName = cloneFromProps->xamlTypeName;  
     }
  }

  void SetProp(uint32_t hash, winrt::hstring propName, winrt::Microsoft::ReactNative::IJSValueReader value) noexcept {
    winrt::Microsoft::ReactNative::ReadProp(hash, propName, value, *this);
  }

  REACT_FIELD(xamlTypeName)
  std::string xamlTypeName;

  const winrt::Microsoft::ReactNative::ViewProps ViewProps;
};

template <typename TUserData>
void CustomRegisterXamlControlNativeComponent(
    const wchar_t *name,
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder,
    std::function<void(const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder&)> builderCallback) noexcept {
  packageBuilder.as<winrt::Microsoft::ReactNative::IReactPackageBuilderFabric>().AddViewComponent(
      name, [builderCallback](winrt::Microsoft::ReactNative::IReactViewComponentBuilder const &builder) noexcept {
        auto compBuilder = builder.as<winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder>();

        builder.SetCreateProps([](winrt::Microsoft::ReactNative::ViewProps props,
                              const winrt::Microsoft::ReactNative::IComponentProps& cloneFrom) noexcept {
            return winrt::make<Codegen::FabricXamlControlProps>(props, cloneFrom); 
        });

        builder.SetUpdatePropsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentProps &newProps,
                                     const winrt::Microsoft::ReactNative::IComponentProps &oldProps) noexcept {
            auto userData = view.UserData().as<TUserData>();
          userData->UpdateProps(
              view,
              newProps ? newProps.as<Codegen::FabricXamlControlProps>() : nullptr,
              oldProps ? oldProps.as<Codegen::FabricXamlControlProps>() : nullptr);
        });

        compBuilder.SetUpdateLayoutMetricsHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::LayoutMetrics &newLayoutMetrics,
                                      const winrt::Microsoft::ReactNative::LayoutMetrics &oldLayoutMetrics) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateLayoutMetrics(view, newLayoutMetrics, oldLayoutMetrics);
        });

        builder.SetUpdateEventEmitterHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::EventEmitter &eventEmitter) noexcept {
          auto userData = view.UserData().as<TUserData>();
          userData->UpdateEventEmitter(std::make_shared<Codegen::FabricXamlControlEventEmitter>(eventEmitter));
        });

        if constexpr (&TUserData::FinalizeUpdate != &winrt::SampleCustomComponent::Codegen::BaseFabricXamlControl<TUserData>::FinalizeUpdate) {
            builder.SetFinalizeUpdateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     winrt::Microsoft::ReactNative::ComponentViewUpdateMask mask) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->FinalizeUpdate(view, mask);
          });
        } 

        if constexpr (
            &TUserData::UpdateState !=
            &winrt::SampleCustomComponent::Codegen::BaseFabricXamlControl<TUserData>::UpdateState) {
          builder.SetUpdateStateHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                     const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept {
            auto userData = view.UserData().as<TUserData>();
            userData->UpdateState(view, newState);
          });
        }

        if constexpr (
            &TUserData::MountChildComponentView !=
            &winrt::SampleCustomComponent::Codegen::BaseFabricXamlControl<TUserData>::MountChildComponentView) {
          builder.SetMountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->MountChildComponentView(view, args);
          });
        }

        if constexpr (
            &TUserData::UnmountChildComponentView !=
            &winrt::SampleCustomComponent::Codegen::BaseFabricXamlControl<TUserData>::UnmountChildComponentView) {
          builder.SetUnmountChildComponentViewHandler([](const winrt::Microsoft::ReactNative::ComponentView &view,
                                      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept {
            auto userData = view.UserData().as<TUserData>();
            return userData->UnmountChildComponentView(view, args);
          });
        }

        compBuilder.SetViewComponentViewInitializer([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
          auto userData = winrt::make_self<TUserData>();
              if constexpr (
                  &TUserData::Initialize !=
                  &winrt::SampleCustomComponent::Codegen::BaseFabricXamlControl<TUserData>::Initialize) {
            userData->Initialize(view);
          }
          view.UserData(*userData);
        });

        if constexpr (
            &TUserData::CreateVisual !=
            &winrt::SampleCustomComponent::Codegen::BaseFabricXamlControl<TUserData>::CreateVisual) {
          compBuilder.SetCreateVisualHandler([](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
            // I suppose we should return null here since we're not backed by a visual?
            // No, it looks like in CompositionViewComponentView.cpp ViewComponentView::ensureVisual that this will fail.
            auto userData = view.UserData().as<TUserData>();
            return userData->CreateVisual(view);
          });
        }

        // Allow app to further customize the builder
        if (builderCallback) {
          builderCallback(compBuilder);
        }
      });
}



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




void RegisterXamlControl(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder)
{  
  winrt::SampleCustomComponent::CustomRegisterXamlControlNativeComponent<
      winrt::SampleCustomComponent::XamlControlComponentView>(
        L"XamlControl",
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        /*
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::SampleCustomComponent::XamlControlComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
            */
        
      });
}

#endif // defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
