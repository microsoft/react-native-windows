#pragma once

struct RNCoreApp {
  /// default value: L"index.windows"
  wchar_t const *jsBundleFile;
  /// default value: L"ms-appx:///Bundle/"
  wchar_t const *bundleRootPath;
  /// Provide the name of your React component here
  wchar_t const *componentName;

  /// default value: true
  bool useWebDebugger;
  /// default value: true
  bool useFastRefresh;
  /// default value: true
  bool useDeveloperSupport;
  /// default value: false
  bool useDirectDebugger;

  /// default value: false
  bool enableDefaultCrashHandler;
  /// default value: 9229
  unsigned short debuggerPort;
  /// default value: 8081
  unsigned short sourceBundlePort;
  /// default value: L"localhost"
  wchar_t const *sourceBundleHost;

  void *resourcesAbi;
  /// Provides the launch arguments back to the CoreApp callback
  wchar_t const *args;
};

#ifdef __cplusplus
#define NORETURN [[noreturn]]
#else
#define NORETURN _Noreturn
#endif
extern "C" NORETURN void __cdecl RNStartCoreApp(void (*launched)(RNCoreApp *));
extern "C" NORETURN void __cdecl RNStartCoreAppFromConfigJson(wchar_t const *configJson);
