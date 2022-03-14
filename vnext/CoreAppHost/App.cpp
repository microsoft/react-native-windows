#include "pch.h"
#include "../Microsoft.ReactNative/Exports.h"
#include <combaseapi.h>

void check(HRESULT hr) {
  if (FAILED(hr))
    throw hr;
}

int __stdcall wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR /*server*/, _In_ int /*showCommand*/) {
  check(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

  RNStartCoreApp([](RNCoreApp *app) {
    app->componentName = L"testRN";

    app->jsBundleFile = L"index";
    app->useDeveloperSupport = true;
    app->useWebDebugger = true;
  });

  CoUninitialize();
}
