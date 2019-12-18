// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "NativeModules.h"

namespace SampleAppCpp {

REACT_MODULE(DebugConsole);
struct DebugConsole {
  REACT_METHOD(Log);
  void Log(std::string message) noexcept {
    std::string output = message + "\n";
    OutputDebugStringA(output.c_str());
  }
};

} // namespace SampleAppCpp
