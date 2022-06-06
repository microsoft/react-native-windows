// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CoreApp.h"
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <fstream>
#include <string_view>
#include "ReactApplication.h"
#include "UI.Xaml.Controls.h"

namespace react = winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Data::Json;
using namespace std::literals;

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
  rnca->requestInlineSourceMap = true;
  rnca->jsEngine = L"chakra";
  rnca->viewName = nullptr;

  rnca->resourcesAbi = nullptr;
  rnca->args = nullptr;
  rnca->packageProvidersAbi = nullptr;
  rnca->packageProvidersAbiCount = 0;
  rnca->propertiesAbi = nullptr;
}

extern "C" NORETURN void __cdecl RNStartCoreAppWithCoreApp(RNCoreApp *rnca, coreAppCallback launched) {
  xaml::Application::Start([rnca, launched](auto &&) {
    winrt::Windows::Foundation::IInspectable outer{nullptr};
    const auto &app = winrt::make<react::implementation::ReactApplication>(outer);

    const auto impl_app = winrt::get_self<react::implementation::ReactApplication>(app);

    impl_app->LaunchedInternal([rnca, launched](
                                   react::ReactApplication const &app,
                                   const winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs &args) {
      rnca->args = args.Arguments().c_str();
      if (launched) {
        launched(rnca);
      }

      for (auto i = 0; i < rnca->packageProvidersAbiCount; i++) {
        auto provider = winrt::Microsoft::ReactNative::IReactPackageProvider(
            rnca->packageProvidersAbi[i], winrt::take_ownership_from_abi);
        app.PackageProviders().Append(provider);
      }

      if (rnca->viewName) {
        auto view = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();
        view.Title(rnca->viewName);
      }

      app.JavaScriptBundleFile(rnca->jsBundleFile);

      auto settings = app.InstanceSettings();
      constexpr std::pair<std::wstring_view, react::JSIEngine> engines[] = {
          {L"chakra"sv, react::JSIEngine::Chakra},
          {L"hermes"sv, react::JSIEngine::Hermes},
          {L"v8"sv, react::JSIEngine::V8}};

      if (auto it = std::find_if(
              std::begin(engines), std::end(engines), [rnca](const auto &c) { return c.first == rnca->jsEngine; })) {
        settings.JSIEngineOverride(it->second);
      }

      settings.BundleRootPath(rnca->bundleRootPath);

      settings.UseWebDebugger(rnca->useWebDebugger);
      settings.UseFastRefresh(rnca->useFastRefresh);
      settings.UseDeveloperSupport(rnca->useDeveloperSupport);
      settings.UseDirectDebugger(rnca->useDirectDebugger);
      settings.RequestInlineSourceMap(rnca->requestInlineSourceMap);

      settings.EnableDefaultCrashHandler(rnca->enableDefaultCrashHandler);
      settings.DebuggerPort(rnca->debuggerPort);
      settings.SourceBundlePort(rnca->sourceBundlePort);
      settings.SourceBundleHost(rnca->sourceBundleHost);

      if (rnca->propertiesAbi) {
        if (auto props = react::IReactPropertyBag(rnca->propertiesAbi, winrt::take_ownership_from_abi)) {
          for (const auto &propName : props.Names()) {
            settings.Properties().Set(propName, props.Get(propName));
          }
        }
      }

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

JsonObject LoadJsonFromFile(wchar_t const *jsoncPath) {
  std::wifstream appConfigJson(jsoncPath);
  winrt::check_bool(appConfigJson.good());
  std::wostringstream out;
  out << appConfigJson.rdbuf();
  auto input = out.str();

  // strip comments from jsonc since WinRT's JsonObject doesn't support them
  // see
  // https://github.com/microsoft/vscode/blob/432349e1eb4a90eccd1039c61f719e6f54c77607/src/vs/base/common/stripComments.js
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

static void SetPackageProviders(const JsonObject &json, RNCoreApp *app) {
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
}

static void SetProperties(const JsonObject &json, RNCoreApp *app) {
  react::IReactPropertyBag bag{nullptr};
  if (auto v = json.TryLookup(L"properties"); v && v.ValueType() == JsonValueType::Object) {
    auto props = v.GetObject();
    bag = react::ReactPropertyBagHelper::CreatePropertyBag();
    for (const auto &prop : props) {
      auto name = prop.Key();
      auto lastDot = std::find(name.rbegin(), name.rend(), L'.');
      std::wstring nsName;
      std::wstring localName;
      if (lastDot != name.rend()) {
        nsName = std::wstring{name.begin(), static_cast<uint64_t>(name.rend() - lastDot - 1)};
        localName = std::wstring{lastDot.base()};
      } else {
        localName = name;
      }
      winrt::hstring nsNameH(nsName);
      winrt::hstring localNameH(localName);
      auto propName = react::ReactPropertyName{nsNameH, localNameH};
      auto value = prop.Value();
      switch (value.ValueType()) {
        case JsonValueType::String:
          bag.Set(propName.Handle(), winrt::box_value(value.GetString()));
          break;
        case JsonValueType::Boolean:
          bag.Set(propName.Handle(), winrt::box_value(value.GetBoolean()));
          break;
        case JsonValueType::Number:
          bag.Set(propName.Handle(), winrt::box_value(value.GetNumber()));
          break;
        case JsonValueType::Null:
          bag.Set(propName.Handle(), nullptr);
          break;
        default:
          assert(false && "not yet implemented");
          break;
      }
    }
  }

  if (bag) {
    winrt::copy_to_abi(bag, app->propertiesAbi);
  }
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
  SET_FROM_JSON(requestInlineSourceMap);

  SET_FROM_JSON(enableDefaultCrashHandler);
  SET_FROM_JSON(debuggerPort);
  SET_FROM_JSON(sourceBundlePort);
  SET_FROM_JSON(sourceBundleHost);
  SET_FROM_JSON(jsEngine);

  SET_FROM_JSON(viewName);

  SetPackageProviders(json, app);
  SetProperties(json, app);
  RNStartCoreAppWithCoreApp(app, launched);
}

extern "C" NORETURN void __cdecl RNStartCoreApp(coreAppCallback launched) {
  auto rnca = std::make_shared<RNCoreApp>();

  RNCoreApp_SetDefaults(rnca.get());
  RNStartCoreAppWithCoreApp(rnca.get(), launched);
}
