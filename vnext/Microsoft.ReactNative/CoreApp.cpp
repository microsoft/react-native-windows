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

void RNCoreApp_SetDefaults(RNCoreApp *rnca) {
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
}

extern "C" NORETURN void __cdecl RNStartCoreAppWithCoreApp(RNCoreApp *rnca, coreAppCallback launched) {
  xaml::Application::Start([rnca, launched](auto &&) {
    winrt::Windows::Foundation::IInspectable outer{nullptr};
    const auto &app = winrt::make<react::implementation::ReactApplication>(outer);

    const auto impl_app = winrt::get_self<react::implementation::ReactApplication>(app);

    for (auto i = 0; i < rnca->packageProvidersAbiCount; i++) {
      auto provider = winrt::Microsoft::ReactNative::IReactPackageProvider(
          rnca->packageProvidersAbi[i], winrt::take_ownership_from_abi);
      app.PackageProviders().Append(provider);
    }

    impl_app->LaunchedInternal([rnca, launched](
                                   react::ReactApplication const &app,
                                   const winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs &args) {
      rnca->args = args.Arguments().c_str();
      if (launched) {
        launched(rnca);
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

void SetFromJson(JsonObject json, wchar_t const *key, wchar_t const *&appValue) {
  if (auto v = json.TryLookup(key); v && v.ValueType() == JsonValueType::String) {
    appValue = _wcsdup(v.GetString().c_str());
  }
}

void SetFromJson(JsonObject json, wchar_t const *key, bool &appValue) {
  if (auto v = json.TryLookup(key); v && v.ValueType() == JsonValueType::Boolean) {
    appValue = v.GetBoolean();
  }
}

template <typename T>
std::enable_if_t<std::is_arithmetic_v<T>> SetFromJson(JsonObject json, wchar_t const *key, T &appValue) {
  if (auto v = json.TryLookup(key); v && v.ValueType() == JsonValueType::Number) {
    appValue = static_cast<T>(v.GetNumber());
  }
}

#define SET_FROM_JSON(key) SetFromJson(json, L#key, app->key)

JsonObject LoadJsonFromFile(wchar_t const* jsoncPath) {
  std::wifstream appConfigJson(jsoncPath);
  winrt::check_bool(appConfigJson.good());
  std::wostringstream out;
  out << appConfigJson.rdbuf();
  auto input = out.str();

  // strip comments from jsonc since WinRT's JsonObject doesn't support them
  // see https://github.com/microsoft/vscode/blob/432349e1eb4a90eccd1039c61f719e6f54c77607/src/vs/base/common/stripComments.js
  auto comments = std::wregex(
      LR"(("[^"\\]*(?:\\.[^"\\]*)*")|('[^'\\]*(?:\\.[^'\\]*)*')|(\/\*[^\/\*]*(?:(?:\*|\/)[^\/\*]*)*?\*\/)|(\/{2,}.*?(?:(?:\r?\n)|$)))");
  std::wstring stripped;
  std::size_t previous = 0;
  for (auto it = std::wsregex_iterator(input.begin(), input.end(), comments); it != std::wsregex_iterator(); ++it) {
    auto match = *it;
    stripped += input.substr(previous, match.position() - previous);
    if (match[3].matched) {
      // do nothing
    } else if (match[4].matched) {
      if (*(match[4].second - 1) == L'\n') {
        stripped += *(match[4].second - 2) == L'\r' ? L"\r\n" : L"\n";
      }
    } else {
      stripped += match.str();
    }

    previous = match.position() + match.length();
  }

  stripped += input.substr(previous);

  auto json = JsonObject::Parse(stripped);
  return json;
}

extern "C" NORETURN void __cdecl RNStartCoreAppFromConfigJson(wchar_t const *configJson, coreAppCallback launched) {
  auto app = new RNCoreApp();
  RNCoreApp_SetDefaults(app);

  auto json = LoadJsonFromFile(configJson);
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

  if (auto v = json.TryLookup(L"nativeModules"); v && v.ValueType() == JsonValueType::Array) {
    auto nativeModules = v.GetArray();
    decltype(app->packageProvidersAbiCount) countAdded = 0;
    const auto count = static_cast<decltype(countAdded)>(nativeModules.Size());

    app->packageProvidersAbi = new void *[count];
    for (std::remove_const_t<decltype(count)> index = 0; index < count; index++) {
      if (auto nm_value = nativeModules.GetAt(index); nm_value.ValueType() == JsonValueType::Object) {
        auto nm = nm_value.GetObject();

        const wchar_t *container{nullptr};
        SetFromJson(nm, L"moduleContainer", container);

        const wchar_t *factoryName{nullptr};
        SetFromJson(nm, L"factory", factoryName);

        HMODULE mod{};
        if (container != nullptr) {
          mod = reinterpret_cast<HMODULE>(WINRT_IMPL_LoadLibraryW(container));
        } else {
          mod = GetModuleHandle(nullptr);
        }

        if (mod) {
          if (auto factory = reinterpret_cast<void *(__cdecl *)()>(
                  WINRT_IMPL_GetProcAddress(mod, winrt::to_string(factoryName).c_str()))) {
            auto packageProviderAbi = factory();
            app->packageProvidersAbi[countAdded++] = packageProviderAbi;
          } else {
            assert(false && "Could not find native module factory function");
          }
        } else {
          assert(false && "Could not find native module library");
        }
      }
    }
    app->packageProvidersAbiCount = countAdded;
  }

  RNStartCoreAppWithCoreApp(app, launched);
}

NORETURN void __cdecl RNStartCoreAppWithModules(coreAppCallback launched, coreAppCallback addModules) {
  auto rnca = std::make_shared<RNCoreApp>();

  RNCoreApp_SetDefaults(rnca.get());
  if (addModules) {
    addModules(rnca.get());
  }
  RNStartCoreAppWithCoreApp(rnca.get(), launched);
}

extern "C" NORETURN void __cdecl RNStartCoreApp(coreAppCallback launched) {
  RNStartCoreAppWithModules(launched, nullptr);
}
