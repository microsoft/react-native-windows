#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "ReactApplication.h"
#include "CoreApp.g.h"

namespace winrt::Microsoft::ReactNative::implementation
{
  struct CoreApp : CoreAppT<CoreApp> {
  CoreApp(winrt::Microsoft::ReactNative::CoreAppStartingHandler const &handler) {
    handler(*this);
    InitializeComponent();
  }
  void OnLaunched(activation::LaunchActivatedEventArgs const &);
  void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const &e);
  void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(IInspectable const &, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const &);

 private:
    using super = CoreAppT<CoreApp>;

    };
}

namespace winrt::Microsoft::ReactNative::factory_implementation
{
    struct CoreApp : CoreAppT<CoreApp, implementation::CoreApp>
    {
    };
}
