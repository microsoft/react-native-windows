#include "pch.h"
#include <combaseapi.h>
#include <winrt/Windows.Data.Json.h>
#include <fstream>
#include "CoreApp.h"

int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR /*server*/, _In_ int /*showCommand*/) {
  winrt::init_apartment();
  RNStartCoreApp([](RNCoreApp *app) {
    std::wifstream appConfigJson(L"app.config.json");
    winrt::check_bool(appConfigJson.good());
    std::wostringstream out;
    out << appConfigJson.rdbuf();
    auto json = winrt::Windows::Data::Json::JsonObject::Parse(out.str());

    app->componentName = _wcsdup(json.GetNamedString(L"componentName").c_str());
    app->jsBundleFile = _wcsdup(json.GetNamedString(L"jsBundleFile").c_str());
    app->useDeveloperSupport = json.GetNamedBoolean(L"useDeveloperSupport");
    app->useWebDebugger = json.GetNamedBoolean(L"useWebDebugger");
  });
}
