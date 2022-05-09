#pragma once
#include <roapi.h>

#ifdef __cplusplus
#define NORETURN [[noreturn]]

struct RNCoreApp {
  wchar_t const *jsBundleFile{L"index.windows"};
  wchar_t const *componentName{L""};
  bool useWebDebugger{false};
  bool useFastRefresh{true};
  bool useDeveloperSupport{true};

  void *_abi{nullptr};
  void *_abiResources{nullptr};
};

#else
#define NORETURN _Noreturn

struct RNCoreApp {
  wchar_t const *jsBundleFile;
  wchar_t const *componentName;
  bool useWebDebugger;
  bool useFastRefresh;
  bool useDeveloperSupport;

  void *_abi;
  void *_abiResources;
};

#endif

extern "C" NORETURN void RNStartCoreApp(void (*launched)(RNCoreApp *));
