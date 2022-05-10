#pragma once

#ifdef __cplusplus
#define NORETURN [[noreturn]]

struct RNCoreApp {
  wchar_t const *jsBundleFile{L"index.windows"};
  wchar_t const *componentName{L""};
  bool useWebDebugger{false};
  bool useFastRefresh{true};
  bool useDeveloperSupport{true};

  void *resourcesAbi{nullptr};
};

#else
#define NORETURN _Noreturn

struct RNCoreApp {
  wchar_t const *jsBundleFile;
  wchar_t const *componentName;
  bool useWebDebugger;
  bool useFastRefresh;
  bool useDeveloperSupport;

  void *resourcesAbi;
};

#endif

extern "C" NORETURN void __cdecl RNStartCoreApp(void (*launched)(RNCoreApp *));
