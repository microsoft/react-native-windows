// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "XamlApplication.h"
#include "XamlHost.h"
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>

#if defined(RNW_NEW_ARCH)

#include "..\codegen\react\components\rnwcore\XamlHost.g.h"

namespace winrt::Microsoft::ReactNative {

struct XamlHostComponentView : public winrt::implements<XamlHostComponentView, winrt::IInspectable>,
                               ::Microsoft::ReactNativeSpecs::BaseXamlHost<XamlHostComponentView> {
  void InitializeContentIsland(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
    winrt::Microsoft::ReactNative::implementation::XamlApplication::EnsureCreated();

    m_xamlIsland = winrt::Microsoft::UI::Xaml::XamlIsland{};

   /* auto grid = winrt::Microsoft::UI::Xaml::Controls::Grid{};
    auto button = winrt::Microsoft::UI::Xaml::Controls::Button{};
    button.Content(winrt::box_value(L"(This is placeholder content for the XamlHost.)"));
    grid.Children().Append(button);
    m_xamlIsland.Content(grid);
    m_xamlIsland.Content(m_XamlHost);*/
    // Create a Button control

   /* winrt::Microsoft::UI::Xaml::Controls::TextBlock textBlock;
    textBlock.Text(L"Hello from XAML!");
    textBlock.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Center);
    textBlock.VerticalAlignment(winrt::Microsoft::UI::Xaml::VerticalAlignment::Center);
    */
    winrt::Microsoft::UI::Xaml::Controls::Button button;
    button.Content(winrt::box_value(L"Click Me"));
    button.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Stretch);
    button.VerticalAlignment(winrt::Microsoft::UI::Xaml::VerticalAlignment::Stretch);
    // Attach Click event handler
    button.Click([](winrt::IInspectable const &, winrt::Microsoft::UI::Xaml::RoutedEventArgs const &) {
      OutputDebugString(L"Welcome to XAML world\n");
    });

    
    button.GotFocus([](winrt::IInspectable const &,
        winrt::Microsoft::UI::Xaml::RoutedEventArgs const &) {
      OutputDebugString(L"[XamlHost] Button received focus\n");
        });


    
   // Accessibility-friendly settings
    winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::SetName(button, L"XAML Button");
    winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::SetHelpText(
     button, L"This is a button hosted in XAML Island.");
    winrt::Microsoft::UI::Xaml::Automation::AutomationProperties::SetAutomationId(button, L"XamlButton1");

    auto peer = winrt::Microsoft::UI::Xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(button);

    auto buttonPeer = peer.try_as<winrt::Microsoft::UI::Xaml::Automation::Peers::ButtonAutomationPeer>();

    auto string = buttonPeer.GetHelpText();
    
    button.IsTabStop(true);
    button.TabIndex(0);
  
    m_xamlIsland.Content(button);
    OutputDebugString(L"[XamlHost] Setting Content\n");

    islandView.Connect(m_xamlIsland.ContentIsland());
  }

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept override {
    // Add the xaml child to the m_xamlIsland here.
    /*
    auto childXamlControl = args.Child().UserData().as<winrt::Microsoft::ReactNative::IXamlControl>();
    if (childXamlControl) {
      auto xamlElement = childXamlControl.GetXamlElement();
      m_xamlIsland.Content(xamlElement);
    }
    */

    // Create a Button control
    winrt::Microsoft::UI::Xaml::Controls::Button button;
    button.Content(winrt::box_value(L"Click Me"));
    button.HorizontalAlignment(winrt::Microsoft::UI::Xaml::HorizontalAlignment::Center);
    button.VerticalAlignment(winrt::Microsoft::UI::Xaml::VerticalAlignment::Center);

    // Attach Click event handler
    button.Click([](winrt::IInspectable const &, winrt::Microsoft::UI::Xaml::RoutedEventArgs const &) {
      OutputDebugString(L"Welcome to XAML world\n");
    });

    m_xamlIsland.Content(button);

  }

  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &) noexcept override {
    m_xamlIsland.Content(nullptr);
  }

 private:
  winrt::Microsoft::UI::Xaml::XamlIsland m_xamlIsland{nullptr};
};

} // namespace winrt::Microsoft::ReactNative

void RegisterXamlHostComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  ::Microsoft::ReactNativeSpecs::RegisterXamlHostNativeComponent<winrt::Microsoft::ReactNative::XamlHostComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData = winrt::make_self<winrt::Microsoft::ReactNative::XamlHostComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
      });
}

#endif // defined(RNW_NEW_ARCH) && defined(USE_EXPERIMENTAL_WINUI3)
