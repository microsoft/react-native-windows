// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "DebugHelpers.h"

#define DEBUG_MYMODULE_OUTPUT(...) DebugWriteLine("MyModule", ##__VA_ARGS__);

namespace SampleLibraryCpp {

REACT_MODULE(MyModule)
struct MyModule {
  // The spec file does not currently validate constants
  REACT_CONSTANT(const1)
  const bool const1 = true;
  REACT_CONSTANT(const2)
  const double const2 = 1.234;
  REACT_CONSTANT(const3)
  const std::string const3{"const3"};

  REACT_METHOD(voidFunc)
  void voidFunc() noexcept {
    DEBUG_MYMODULE_OUTPUT("voidFunc");
  }

  REACT_SYNC_METHOD(getBool)
  bool getBool(bool arg) noexcept {
    DEBUG_MYMODULE_OUTPUT("getBool", arg);
    return arg;
  }

  REACT_SYNC_METHOD(getNumber)
  double getNumber(double arg) noexcept {
    DEBUG_MYMODULE_OUTPUT("getNumber", arg);
    return arg;
  }

  REACT_SYNC_METHOD(getString)
  std::string getString(std::string arg) noexcept {
    DEBUG_MYMODULE_OUTPUT("getString", arg);
    return arg;
  }

  REACT_SYNC_METHOD(getArray)
  React::JSValueArray getArray(React::JSValueArray && /*arg*/) noexcept {
    return React::JSValueArray{"X", 4, true};
  }

  REACT_SYNC_METHOD(getObject)
  React::JSValueObject getObject(React::JSValueObject && /*arg*/) noexcept {
    return React::JSValueObject{{"X", 4}, {"Y", 2}};
  }

  REACT_SYNC_METHOD(getValue)
  React::JSValueObject getValue(double x, std::string y, JSValueObject &&z) noexcept {
    return React::JSValueObject{{"X", x}, {"Y", y}, {"Z", std::move(z)}};
  }

  REACT_METHOD(getValueWithCallback)
  void getValueWithCallback(std::function<void(React::JSValue const &)> const &callback) noexcept {
    callback(React::JSValue{"some string"});
  }

  REACT_METHOD(getValueWithPromise)
  void getValueWithPromise(bool error, React::ReactPromise<React::JSValue> &&result) noexcept {
    if (error) {
      result.Reject("Failure");
    } else {
      result.Resolve(React::JSValue{"Succeeded"});
    }
  }
};

} // namespace SampleLibraryCpp
