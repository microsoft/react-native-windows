// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JsModulesUnbundle.h"
#include "ChakraHost.h"

const unsigned int MagicFileHeader = 0xFB0BD1E5;

wchar_t *GetJavaScriptModuleDirectory(const wchar_t *sourcePath) {
  auto buffer = new wchar_t[_MAX_DIR];
  buffer[0] = '\0';
  auto filename = wcsrchr(sourcePath, L'\\');
  wcsncat_s(
      buffer, _MAX_DIR, sourcePath, wcslen(sourcePath) - wcslen(filename) + 1);
  wcscat_s(buffer, _MAX_DIR, L"js-modules\\");
  return buffer;
}

JsModulesUnbundle::JsModulesUnbundle() {}

JsModulesUnbundle::JsModulesUnbundle(const wchar_t *szSourcePath) {
  // Clone the source path in case it is garbage collected
  sourcePath = new wchar_t[_MAX_DIR];
  sourcePath[0] = L'\0';
  wcscat_s(sourcePath, _MAX_DIR, szSourcePath);
  modulesPath = GetJavaScriptModuleDirectory(sourcePath);
}

JsModulesUnbundle::~JsModulesUnbundle() {
  delete[] sourcePath;
  delete[] modulesPath;
}

JsModulesUnbundleModule *JsModulesUnbundle::GetModule(uint32_t index) {
  auto module = new JsModulesUnbundleModule();
  auto filename = new wchar_t[_MAX_FNAME];
  wchar_t path[_MAX_DIR] = L"";

  IfErrnoCleanup(_itow_s(index, filename, _MAX_FNAME, 10));
  IfErrnoCleanup(wcscat_s(filename, _MAX_FNAME, L".js"));
  IfErrnoCleanup(wcscat_s(path, modulesPath));
  IfErrnoCleanup(wcscat_s(path, filename));

  module->sourceUrl = filename;
  if (ChakraHost::LoadFileContents(path, &module->source) != JsNoError) {
    goto cleanup;
  }

  return module;

cleanup:
  delete filename;
  delete module;
  return nullptr;
}

JsErrorCode JsModulesUnbundle::GetStartupCode(wchar_t **pszScript) {
  return ChakraHost::LoadFileContents(sourcePath, pszScript);
}