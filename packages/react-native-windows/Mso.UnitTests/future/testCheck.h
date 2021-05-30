// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "motifCpp/testCheck.h"

#define ValidateEmplacedType(obj, v1, v2, v3)           \
  TestCheckEqualAt(__FILE__, __LINE__, v1, obj.Value1); \
  TestCheckEqualAt(__FILE__, __LINE__, v2, obj.Value2); \
  TestCheckEqualAt(__FILE__, __LINE__, v3, obj.Value3);

struct EmplacedType {
  EmplacedType() noexcept {}

  EmplacedType(int v1) noexcept : Value1(v1) {}

  EmplacedType(int v1, int v2) noexcept : Value1(v1), Value2(v2) {}

  EmplacedType(int v1, int v2, int v3) noexcept : Value1(v1), Value2(v2), Value3(v3) {}

  int Value1 = 1;
  int Value2 = 2;
  int Value3 = 3;
};
