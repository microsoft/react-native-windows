#include "pch.h"
#include "Exports.h"
#include "ReactApplication.h"
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace react = winrt::Microsoft::ReactNative;

extern "C" void *RNStartCoreApp(void (*launched)(RNCoreApp *)) {
  void *abi{nullptr};
  xaml::Application::Start([&abi, launched](auto &&) {
    // CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    winrt::Windows::Foundation::IInspectable outer{nullptr};
    const auto &app = winrt::make<react::implementation::ReactApplication>(outer);
    app.LaunchedInternal([launched](react::ReactApplication const &app, auto &&args) {
      RNCoreApp rnca{winrt::get_abi(app)};
      if (launched) {
        launched(&rnca);
      }
      app.JavaScriptBundleFile(rnca.jsBundleFile);
      auto settings = app.InstanceSettings();
      settings.UseWebDebugger(rnca.useWebDebugger);
      settings.UseFastRefresh(rnca.useFastRefresh);
      settings.UseDeveloperSupport(rnca.useDeveloperSupport);

      if (auto res = winrt::Windows::UI::Xaml::ResourceDictionary(rnca._abiResources, winrt::take_ownership_from_abi)) {
        app.Resources(res);
      } else {
        try {
          auto xcr = winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources();
          app.Resources(xcr);
        } catch (...) {
        }
      }
    });
    app.ViewCreatedInternal([](react::ReactApplication const &app, winrt::hstring const& args) {
      auto rootFrame = xaml::Window::Current().Content().as<xaml::Controls::Frame>();
      rootFrame.Navigate(winrt::xaml_typename<react::CoreAppPage>(), winrt::box_value(args));
      });
  });

  return abi;
}

