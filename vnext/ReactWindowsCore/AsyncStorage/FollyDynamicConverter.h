// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <cxxreact/JsArgumentHelpers.h>
#include <folly/dynamic.h>

using namespace std;
using namespace folly;
using namespace facebook::xplat;

namespace facebook {
namespace react {
class FollyDynamicConverter {
 public:
  static std::vector<string> jsArgAsStringVector(const dynamic &args) noexcept;
  static std::vector<tuple<string, string>> jsArgAsTupleStringVector(const dynamic &args) noexcept;
  static folly::dynamic stringVectorAsRetVal(const std::vector<string> &vec) noexcept;
  static folly::dynamic tupleStringVectorAsRetVal(const std::vector<tuple<string, string>> &vec) noexcept;
};
} // namespace react
} // namespace facebook