#include "pch.h"
#include "../Microsoft.ReactNative/Exports.h"
#include <combaseapi.h>

void check(HRESULT hr) {
  if (FAILED(hr))
    throw hr;
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR /*server*/, int /*showCommand*/)
{
  check(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

  RNStartCoreApp(
      [](RNCoreApp *app) {
        app->jsBundleFile = L"index"; 
      });

  CoUninitialize();
}
