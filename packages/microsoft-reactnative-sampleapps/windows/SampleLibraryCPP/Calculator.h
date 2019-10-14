// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <functional>
#include "NativeModules.h"

namespace SampleLibraryCPP {

REACT_MODULE(Calculator);
struct Calculator {
  REACT_METHOD(Add);
  int Add(int x, int y) noexcept {
    return x + y;
  }

  REACT_METHOD(Subtract);
  void Subtract(
      int x,
      int y,
      std::function<void(int)> &&resolve,
      std::function<void(std::string)> &&reject) noexcept {
    if (x > y) {
      resolve(x - y);
    } else {
      reject("x must be greater than y");
    }
  }
};

} // namespace SampleLibraryCPP
