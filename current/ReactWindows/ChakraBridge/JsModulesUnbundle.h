// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <jsrt.h>
#include <stdint.h>

struct JsModulesUnbundleModule sealed {
 public:
  wchar_t *source;
  wchar_t *sourceUrl;
  ~JsModulesUnbundleModule() {
    delete[] source;
    delete[] sourceUrl;
  }
};

class JsModulesUnbundle {
 public:
  JsModulesUnbundle(const wchar_t *szSourcePath);
  ~JsModulesUnbundle();
  virtual JsModulesUnbundleModule *GetModule(uint32_t index);
  virtual JsErrorCode GetStartupCode(wchar_t **pszScript);

 protected:
  JsModulesUnbundle();

 private:
  wchar_t *sourcePath;
  wchar_t *modulesPath;
};
