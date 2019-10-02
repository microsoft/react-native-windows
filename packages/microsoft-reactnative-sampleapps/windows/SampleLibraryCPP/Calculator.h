// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <functional>
#include "NativeModules.h"

namespace SampleLibraryCPP {

struct Calculator {
  RN_METHOD(Add);
  int Add(int x, int y) noexcept {
    return x + y;
  }

  RN_METHOD(Subtract);
  void Subtract(
      int x,
      int y,
      std::function<void(int)> &&result,
      std::function<void(std::string)> &&onError) noexcept {
    if (x > y) {
      result(x - y);
    } else {
      onError("x must be greater than y");
    }
  }
};

} // namespace SampleLibraryCPP
