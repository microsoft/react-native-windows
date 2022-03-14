#pragma once
#include <roapi.h>

struct RNCoreApp {
  void *_abi{nullptr};
  wchar_t const *jsBundleFile{L"index.windows"};
  bool useWebDebugger{false};
  bool useFastRefresh{true};
  bool useDeveloperSupport{true};
  void *_abiResources{nullptr};
};

extern "C" void *RNStartCoreApp(void (*launched)(RNCoreApp *));
