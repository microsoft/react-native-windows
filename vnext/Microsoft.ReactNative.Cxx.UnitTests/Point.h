#pragma once
#include "NativeModules.h"

namespace ReactNativeTests {

REACT_STRUCT(Point)
struct Point {
  REACT_FIELD(X)
  int X;

  REACT_FIELD(Y)
  int Y;
};

} // namespace ReactNativeTests
