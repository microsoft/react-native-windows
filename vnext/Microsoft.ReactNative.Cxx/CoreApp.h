// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
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
  /// default value: true
  bool requestInlineSourceMap;

  /// default value: false
  bool enableDefaultCrashHandler;
  /// default value: 9229
  unsigned short debuggerPort;
  /// default value: 8081
  unsigned short sourceBundlePort;
  /// default value: L"localhost"
  wchar_t const *sourceBundleHost;

  /// default value: "chakra", possible values: "chakra", "hermes"
  wchar_t const *jsEngine;

  /// default value: nullptr
  wchar_t const *viewName;

  void *resourcesAbi;
  /// Provides the launch arguments back to the CoreApp callback
  wchar_t const *args;
  /// Allocate with CoTaskMemAlloc, will be freed by the framework
  void **packageProvidersAbi;
  unsigned char packageProvidersAbiCount;
  void *propertiesAbi;
};

#ifdef __cplusplus
#define NORETURN [[noreturn]]
#else
#define NORETURN _Noreturn
#endif

typedef void(__cdecl *RNCoreAppCallback)(RNCoreApp *, void *);

extern "C" NORETURN void __cdecl RNCoreAppStart(RNCoreAppCallback launched, void *data);
extern "C" NORETURN void __cdecl RNCoreAppStartFromConfigJson(
    wchar_t const *configJson,
    RNCoreAppCallback launched,
    void *data);
