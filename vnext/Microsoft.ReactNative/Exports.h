#pragma once
#include <roapi.h>

struct RNCoreApp {
  wchar_t const *jsBundleFile{L"index.windows"};
  wchar_t const *componentName{L""};
  bool useWebDebugger{false};
  bool useFastRefresh{true};
  bool useDeveloperSupport{true};

  void *_abi{nullptr};
  void *_abiResources{nullptr};
};

#ifdef __cplusplus
#define NORETURN [[noreturn]]
#else
#define NORETURN _Noreturn
#endif


extern "C" NORETURN void RNStartCoreApp(void (*launched)(RNCoreApp *));
