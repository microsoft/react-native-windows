// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DebugHelpers.h"

#define DEBUG_MYMODULE_OUTPUT(...) DebugWriteLine("MyModule", ##__VA_ARGS__);

namespace SampleLibraryCpp {

struct MyModule {
  void Initialize(IReactContext const & /*reactContext*/) noexcept {}

  const bool const1 = false;
  const double const2 = 1.234;
  const std::string const3 = "abc";

  void voidFunc() noexcept {
    DEBUG_MYMODULE_OUTPUT("voidFunc");
  }

  bool getBool(bool arg) noexcept {
    DEBUG_MYMODULE_OUTPUT("getBool", arg);
    return arg;
  }

  double getNumber(double arg) noexcept {
    DEBUG_MYMODULE_OUTPUT("getNumber", arg);
    return arg;
  }

  std::string getString(std::string arg) noexcept {
    DEBUG_MYMODULE_OUTPUT("getString", arg);
    return arg;
  }
};

} // namespace SampleLibraryCpp
