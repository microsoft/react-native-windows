// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include "NativeModules.h"

namespace TestApp {

struct MyModule {
  RN_CONSTANT(m_fieldConst);
  const int m_fieldConst = 42;

  RN_CONSTANT_JSNAME(m_fldConst, "fldConst");
  const int m_fldConst = 43;

  RN_CONST_METHOD(SimpleConstants);
  void SimpleConstants(
      const winrt::Microsoft::ReactNative::Bridge::IJSValueWriter
          &writer) noexcept {
    ::Microsoft::ReactNative::WriteProperty(writer, "simpleConst1", 5);
    ::Microsoft::ReactNative::WriteProperty(writer, "simpleConst2", "World");
  }

  RN_EVENT(OnChanged);
  std::function<void(int)> OnChanged;

  RN_METHOD(Add);
  int Add(int x, int y) noexcept {
    OnChanged(x + y);
    return x + y;
  }

  RN_METHOD(PrintAdd);
  void PrintAdd(int x, int y) noexcept {
    printf("PrintAdd: %d\n", x + y);
  }

  RN_METHOD(AddCallback);
  void AddCallback(int x, int y, std::function<void(int)> &&result) noexcept {
    // printf("AddCallback: %d\n", x + y);
    result(x + y);
  }

  RN_METHOD(SubtractCallback);
  void SubtractCallback(
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

  RN_METHOD(NoArg0);
  void NoArg0() noexcept {
    printf("no arg \n");
  }

  RN_METHOD(NoArg1);
  int NoArg1() noexcept {
    return 10;
  }

  RN_SYNC_METHOD_JSNAME(GetRegValue, "regValue");
  std::string GetRegValue(std::string key) noexcept {
    if (key == "Key1") {
      return "Simple1";
    } else if (key == "Key2") {
      return "Simple2";
    }

    return "Default";
  }
};

} // namespace TestApp
