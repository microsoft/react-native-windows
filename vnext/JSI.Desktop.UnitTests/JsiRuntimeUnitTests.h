// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <jsi/jsi.h>

#include <gtest/gtest.h>

#include <functional>
#include <memory>

namespace Microsoft::React::Test {

using RuntimeFactory = std::function<std::unique_ptr<facebook::jsi::Runtime>()>;

class JsiRuntimeUnitTests : public ::testing::TestWithParam<RuntimeFactory> {
 public:
  JsiRuntimeUnitTests() : factory(GetParam()), runtime(factory()), rt(*runtime) {}

  facebook::jsi::Value eval(const char *code) {
    return rt.global().getPropertyAsFunction(rt, "eval").call(rt, code);
  }

  facebook::jsi::Function function(const std::string &code) {
    return eval(("(" + code + ")").c_str()).getObject(rt).getFunction(rt);
  }

  bool checkValue(const facebook::jsi::Value &value, const std::string &jsValue) {
    // TODO (yicyao): Should we use === instead of == here?
    return function("function(value) { return value == " + jsValue + "; }").call(rt, value).getBool();
  }

  // The order of these member variable declarations is important because they
  // need to be initialized in this order.
  RuntimeFactory factory;
  std::unique_ptr<facebook::jsi::Runtime> runtime;
  facebook::jsi::Runtime &rt;
};

// V8 doesn't currently pass all the tests (particularly around hostobjects), move those out into _Chakra for now
class JsiRuntimeUnitTests_Chakra : public JsiRuntimeUnitTests {};

} // namespace Microsoft::React::Test
