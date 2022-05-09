#include "pch.h"
#include "CoreApp.h"
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include "ReactApplication.h"

namespace react = winrt::Microsoft::ReactNative;

extern "C" NORETURN void RNStartCoreApp(void (*launched)(RNCoreApp *)) {
  xaml::Application::Start([launched](auto &&) {
    // CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    winrt::Windows::Foundation::IInspectable outer{nullptr};
    const auto &app = winrt::make<react::implementation::ReactApplication>(outer);

    auto rnca = std::make_shared<RNCoreApp>();
    rnca->_abi = winrt::get_abi(app);

    app.LaunchedInternal([rnca, launched](react::ReactApplication const &app, auto &&args) {
      if (launched) {
        launched(rnca.get());
      }
      app.JavaScriptBundleFile(rnca->jsBundleFile);
      auto settings = app.InstanceSettings();
      settings.UseWebDebugger(rnca->useWebDebugger);
      settings.UseFastRefresh(rnca->useFastRefresh);
      settings.UseDeveloperSupport(rnca->useDeveloperSupport);

      if (auto res =
              xaml::ResourceDictionary(rnca->_abiResources, winrt::take_ownership_from_abi)) {
        app.Resources(res);
      } else {
        try {
          auto xcr = winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources();
          app.Resources(xcr);
        } catch (...) {
        }
      }
    });
    app.ViewCreatedInternal([](react::ReactApplication const &app, winrt::hstring const &args) {
      auto rootFrame = xaml::Window::Current().Content().as<xaml::Controls::Frame>();
      rootFrame.Navigate(winrt::xaml_typename<react::CoreAppPage>(), winrt::box_value(args));
    });
    app.PageNavigatedInternal([rnca](react::ReactApplication const & /*app*/, react::ReactRootView const &view) {
      view.ComponentName(rnca->componentName);
    });
  });
}
