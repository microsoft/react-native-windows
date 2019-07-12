// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <jsi/jsi.h>

#include <gtest/gtest.h>

#include <functional>
#include <memory>

namespace facebook::jsi {

using RuntimeFactory = std::function<std::unique_ptr<Runtime>()>;

class JsiRuntimeUnitTests : public ::testing::TestWithParam<RuntimeFactory> {
 public:
  JsiRuntimeUnitTests()
      : factory(GetParam()), runtime(factory()), rt(*runtime) {}

  Value eval(const char *code) {
    return rt.global().getPropertyAsFunction(rt, "eval").call(rt, code);
  }

  Function function(const std::string &code) {
    return eval(("(" + code + ")").c_str()).getObject(rt).getFunction(rt);
  }

  bool checkValue(const Value &value, const std::string &jsValue) {
    // TODO (yicyao): Should we use === instead of == here?
    return function("function(value) { return value == " + jsValue + "; }")
        .call(rt, std::move(value))
        .getBool();
  }

  // The order of these member variable declarations is important because they
  // need to be initialized in this order.
  RuntimeFactory factory;
  std::unique_ptr<Runtime> runtime;
  Runtime &rt;
};

} // namespace facebook::jsi
