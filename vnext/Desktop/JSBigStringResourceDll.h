// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/JSBigString.h>
#include <windows.h>

namespace facebook {
namespace react {

class JSBigStringResourceDll final : public JSBigString {
 public:
  static std::unique_ptr<const JSBigStringResourceDll> Make(
      HMODULE dll,
      HRSRC resource);

  JSBigStringResourceDll(HMODULE dll, HRSRC resource);

  virtual bool isAscii() const override {
    return false;
  }

  virtual const char *c_str() const override {
    return resource;
  }

  virtual size_t size() const override {
    return resourceSize - 1;
  }

 private:
  char *resource;
  uint32_t resourceSize;
};

} // namespace react
} // namespace facebook
