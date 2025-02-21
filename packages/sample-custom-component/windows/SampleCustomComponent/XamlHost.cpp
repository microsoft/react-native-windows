// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "XamlHost.h"
#include "FabricXamlControl.h"

#if defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)

#include "codegen/react/components/SampleCustomComponent/XamlHost.g.h"

namespace winrt::SampleCustomComponent {

// This is a "toy" implementation of Xaml's XamlHost, just for bringup and testing.
// When UseExperimentalWinUI3 and UseNewArchitecture are both true, it will host a
// XamlHost control in a XamlIsland.  But we only expose a few APIs right now.
// Later, we'll expose this through react-native-xaml.

struct XamlHostComponentView : public winrt::implements<XamlHostComponentView, winrt::IInspectable>,
                                   Codegen::BaseXamlHost<XamlHostComponentView> {
  void InitializeContentIsland(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};
    //m_xamlIsland.Content(m_XamlHost);
    islandView.Connect(m_xamlIsland.ContentIsland());
   
  }

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs & args) noexcept override {
        // Add the xaml child to the m_xamlIsland here.
        auto xamlControl = args.Child().UserData().as<XamlControlComponentView>();
        if (xamlControl) {
          auto xamlElement = xamlControl->GetXamlElement();
          m_xamlIsland.Content(xamlElement);
        }
      }

 private:
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
};

} // namespace winrt::SampleCustomComponent

void RegisterXamlHostComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  winrt::SampleCustomComponent::Codegen::RegisterXamlHostNativeComponent<
      winrt::SampleCustomComponent::XamlHostComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::SampleCustomComponent::XamlHostComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
      });
}


#endif // defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
