#include "pch.h"
#include "CoreApp.h"
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <fstream>
#include "ReactApplication.h"
#include "UI.Xaml.Controls.h"

namespace react = winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Data::Json;

void SetFromJson(JsonObject json, wchar_t const *key, wchar_t const *&appValue) {
  if (json.HasKey(key)) {
    appValue = _wcsdup(json.GetNamedString(key).c_str());
  }
}

void SetFromJson(JsonObject json, wchar_t const *key, bool &appValue) {
  if (json.HasKey(key)) {
    appValue = json.GetNamedBoolean(key);
  }
}

template <typename T>
std::enable_if_t<std::is_arithmetic_v<T>> SetFromJson(JsonObject json, wchar_t const *key, T &appValue) {
  if (json.HasKey(key)) {
    appValue = static_cast<T>(json.GetNamedNumber(key));
  }
}

#define SET_FROM_JSON(key) SetFromJson(json, L#key, app->key)

extern "C" NORETURN void __cdecl RNStartCoreAppFromConfigJson(wchar_t const *configJson) {
  RNStartCoreApp([](RNCoreApp *app) {
    std::wifstream appConfigJson(L"app.config.json");
    winrt::check_bool(appConfigJson.good());
    std::wostringstream out;
    out << appConfigJson.rdbuf();
    auto json = JsonObject::Parse(out.str());

    SET_FROM_JSON(jsBundleFile);
    SET_FROM_JSON(bundleRootPath);
    SET_FROM_JSON(componentName);
    SET_FROM_JSON(useWebDebugger);
    SET_FROM_JSON(useFastRefresh);
    SET_FROM_JSON(useDeveloperSupport);
    SET_FROM_JSON(useDirectDebugger);

    SET_FROM_JSON(enableDefaultCrashHandler);
    SET_FROM_JSON(debuggerPort);
    SET_FROM_JSON(sourceBundlePort);
    SET_FROM_JSON(sourceBundleHost);
  });
}

extern "C" NORETURN void __cdecl RNStartCoreAppWithModules(void (*launched)(RNCoreApp *), void(*addModules)(RNCoreApp*)) {
  xaml::Application::Start([launched, addModules](auto &&) {
    winrt::Windows::Foundation::IInspectable outer{nullptr};
    const auto &app = winrt::make<react::implementation::ReactApplication>(outer);

    const auto impl_app = winrt::get_self<react::implementation::ReactApplication>(app);

    auto rnca = std::make_shared<RNCoreApp>();

    if (addModules) {
      addModules(rnca.get());
    }
    for (auto i = 0; i < rnca->packageProvidersAbiCount; i++) {
      auto provider = winrt::Microsoft::ReactNative::IReactPackageProvider(
          rnca->packageProvidersAbi[i], winrt::take_ownership_from_abi);
      app.PackageProviders().Append(provider);
    }

    /// Set default values
    rnca->jsBundleFile = L"index.windows";
    rnca->bundleRootPath = L"ms-appx:///Bundle/";
    rnca->componentName = L"";
    rnca->useWebDebugger = true;
    rnca->useFastRefresh = true;
    rnca->useDeveloperSupport = true;
    rnca->useDirectDebugger = false;
    rnca->enableDefaultCrashHandler = false;
    rnca->debuggerPort = 9229;
    rnca->sourceBundlePort = 8081;
    rnca->sourceBundleHost = L"localhost";
    rnca->resourcesAbi = nullptr;
    rnca->args = nullptr;
    rnca->packageProvidersAbi = nullptr;
    rnca->packageProvidersAbiCount = 0;

    impl_app->LaunchedInternal([rnca, launched](
                             react::ReactApplication const &app,
                             const winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs &args) {
      rnca->args = args.Arguments().c_str();
      if (launched) {
        launched(rnca.get());
      }
      app.JavaScriptBundleFile(rnca->jsBundleFile);

      auto settings = app.InstanceSettings();
      settings.BundleRootPath(rnca->bundleRootPath);

      settings.UseWebDebugger(rnca->useWebDebugger);
      settings.UseFastRefresh(rnca->useFastRefresh);
      settings.UseDeveloperSupport(rnca->useDeveloperSupport);
      settings.UseDirectDebugger(rnca->useDirectDebugger);

      settings.EnableDefaultCrashHandler(rnca->enableDefaultCrashHandler);
      settings.DebuggerPort(rnca->debuggerPort);
      settings.SourceBundlePort(rnca->sourceBundlePort);
      settings.SourceBundleHost(rnca->sourceBundleHost);

      if (auto res = xaml::ResourceDictionary(rnca->resourcesAbi, winrt::take_ownership_from_abi)) {
        app.Resources(res);
      } else {
        try {
          auto xcr = winrt::Microsoft::UI::Xaml::Controls::XamlControlsResources();
          app.Resources(xcr);
        } catch (...) {
        }
      }

    });
    impl_app->ViewCreatedInternal([](react::ReactApplication const &app, winrt::hstring const &args) {
      auto rootFrame = xaml::Window::Current().Content().as<xaml::Controls::Frame>();
      rootFrame.Navigate(winrt::xaml_typename<react::CoreAppPage>(), winrt::box_value(args));
    });
    impl_app->PageNavigatedInternal([rnca](react::ReactApplication const & /*app*/, react::ReactRootView const &view) {
      view.ComponentName(rnca->componentName);
    });
  });
}

extern "C" NORETURN void __cdecl RNStartCoreApp(void (*launched)(RNCoreApp*))
{
  RNStartCoreAppWithModules(launched, nullptr);
}
