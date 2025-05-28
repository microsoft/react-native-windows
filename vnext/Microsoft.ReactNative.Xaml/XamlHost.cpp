// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "FabricXamlControl.h"
#include "XamlApplication.h"
#include "XamlHost.h"

#if defined(RNW_NEW_ARCH)

#include "..\codegen\react\components\rnwcore\XamlHost.g.h"
// #include "codegen/react/components/Microsoft::ReactNative::Xaml/XamlHost.g.h"

namespace winrt::Microsoft::ReactNative::Xaml {

// This is a "toy" implementation of Xaml's XamlHost, just for bringup and testing.
// When UseExperimentalWinUI3 and UseNewArchitecture are both true, it will host a
// XamlHost control in a XamlIsland.  But we only expose a few APIs right now.
// Later, we'll expose this through react-native-xaml.

struct XamlHostComponentView : public winrt::implements<XamlHostComponentView, winrt::IInspectable>,
                               ::Microsoft::ReactNativeSpecs::BaseXamlHost<XamlHostComponentView> {
  void InitializeContentIsland(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
    winrt::Microsoft::ReactNative::Xaml::implementation::XamlApplication::EnsureCreated();

    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};

    auto grid = winrt::Microsoft::UI::Xaml::Controls::Grid{};
    auto button = winrt::Microsoft::UI::Xaml::Controls::Button{};
    button.Content(winrt::box_value(L"(This is placeholder content for the XamlHost.)"));
    grid.Children().Append(button);
    m_xamlIsland.Content(grid);

    // m_xamlIsland.Content(m_XamlHost);
    islandView.Connect(m_xamlIsland.ContentIsland());
  }

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept override {
    // Add the xaml child to the m_xamlIsland here.
    //auto xamlControl = args.Child().UserData().as<XamlControlComponentView>();
    auto childXamlControl = args.Child().UserData().as<winrt::Microsoft::ReactNative::Xaml::IXamlControl>();
    if (childXamlControl) {
      auto xamlElement = childXamlControl.GetXamlElement();
      m_xamlIsland.Content(xamlElement);
    }
  }

  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &) noexcept override {
    m_xamlIsland.Content(nullptr);
  }

 private:
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::Xaml

void RegisterXamlHostComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  ::Microsoft::ReactNativeSpecs::RegisterXamlHostNativeComponent<
      winrt::Microsoft::ReactNative::Xaml::XamlHostComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::Microsoft::ReactNative::Xaml::XamlHostComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
      });
}

#endif // defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
