// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DebugHelpers.h"

#define DEBUG_OUTPUT(...) DebugWriteLine("SampleModuleCppImpl", ##__VA_ARGS__);

namespace SampleLibraryCpp {

struct MyNativeModule {
    void Initialize(IReactContext const & /*reactContext*/) noexcept {
    }

    const bool const1 = false;
    const double const2 = 1.234;
    const std::string const3 = "abc";

    void voidFunc() noexcept {
      DEBUG_OUTPUT("voidFunc");
    }

    bool getBool(bool arg) noexcept {
      DEBUG_OUTPUT("getBool", arg);
      return arg;
    }

    double getNumber(double arg) noexcept {
      DEBUG_OUTPUT("getNumber", arg);
      return arg;
    }

    std::string getString(std::string arg) noexcept {
      DEBUG_OUTPUT("getString", arg);
      return arg;
    }
};

} // namespace SampleLibraryCpp
