#pragma once

#include <functional>
#include "NativeModules.h"

namespace TestApp {

namespace {

struct MyPoint {
  int X;
  int Y;
};

bool WriteValue(
    winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const &writer,
    const MyPoint &point) noexcept {
  writer.WriteObjectBegin();
  Microsoft::ReactNative::WriteProperty(writer, "X", point.X);
  Microsoft::ReactNative::WriteProperty(writer, "Y", point.Y);
  return writer.WriteObjectEnd();
}

} // namespace

struct MyCtorModule {
  MyCtorModule() noexcept {
    REACT_REGISTER_METHOD(AddWithBias2);

    REACT_REGISTER_EVENT(OnClick);

    // Three different ways to register constants

    REACT_REGISTER_CONST_METHOD(ComplexConstants);

    REACT_REGISTER_CONSTANT(m_fieldConst2);
    REACT_REGISTER_CONSTANT_JSNAME(m_fldConst2, "fldConst2");

    REACT_CONSTANT_VALUE("myConst1", "Hello");
    REACT_CONSTANT_VALUE("myConst2", 42);
  }

  int AddWithBias2(int x, int y) noexcept {
    OnClick({x, y});
    return x + y + 2;
  }

  std::function<void(MyPoint)> OnClick;

  void ComplexConstants(
      winrt::Microsoft::ReactNative::Bridge::IJSValueWriter const
          &writer) noexcept {
    ::Microsoft::ReactNative::WriteProperty(writer, "complexConst1", 15);
    ::Microsoft::ReactNative::WriteProperty(writer, "complexConst2", "Me");
  }

  const std::string m_fieldConst2 = "const 1";
  const std::string m_fldConst2 = "const 2";
};

} // namespace TestApp
