// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <sstream>
#include "NativeModules.h"

namespace TestApp {

struct MyModule {
  REACT_CONSTANT(m_fieldConst);
  const int m_fieldConst = 42;

  REACT_CONSTANT_JSNAME(m_fldConst, "fldConst");
  const int m_fldConst = 43;

  REACT_CONST_METHOD(SimpleConstants);
  void SimpleConstants(
      const winrt::Microsoft::ReactNative::Bridge::IJSValueWriter
          &writer) noexcept {
    ::Microsoft::ReactNative::WriteProperty(writer, "simpleConst1", 5);
    ::Microsoft::ReactNative::WriteProperty(writer, "simpleConst2", "World");
  }

  REACT_EVENT(OnChanged);
  std::function<void(int)> OnChanged;

  REACT_METHOD(Add);
  int Add(int x, int y) noexcept {
    OnChanged(x + y);
    return x + y;
  }

  REACT_METHOD(PrintAdd);
  void PrintAdd(int x, int y) noexcept {
    std::stringstream ss;
    ss << "MyModule.PrintAdd: " << x + y << "\n";
    OutputDebugStringA(ss.str().c_str());
  }

  REACT_METHOD(AddCallback);
  void AddCallback(int x, int y, std::function<void(int)> &&result) noexcept {
    result(x + y);
  }

  REACT_METHOD(SubtractCallback);
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

  REACT_METHOD(NoArg0);
  void NoArg0() noexcept {
    std::stringstream ss;
    ss << "MyModule.NoArg0 \n";
    OutputDebugStringA(ss.str().c_str());
  }

  REACT_METHOD(NoArg1);
  int NoArg1() noexcept {
    return 10;
  }

  REACT_SYNC_METHOD_JSNAME(GetRegValue, "regValue");
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
