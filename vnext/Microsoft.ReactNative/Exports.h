#pragma once

struct RNCoreApp {
  void *_abi;
  wchar_t const *jsBundleFile{L"index.windows"};
  bool useWebDebugger{false};
  bool useFastRefresh{true};
  bool useDeveloperSupport{true};
};

extern "C" void *RNStartCoreApp(void (*callback)(RNCoreApp *));
