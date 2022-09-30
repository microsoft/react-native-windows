// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "..\..\codegen\NativeMyModuleSpec.g.h"
#include "DebugHelpers.h"

#define DEBUG_MYMODULE_OUTPUT(...) DebugWriteLine("MyModule", ##__VA_ARGS__);

namespace SampleLibraryCpp {

REACT_MODULE(MyModule)
struct MyModule {
  using ModuleSpec = SampleLibraryCodegen::MyModuleSpec;

  REACT_GET_CONSTANTS(GetConstants)
  SampleLibraryCodegen::MyModuleSpec_Constants GetConstants() noexcept {
    SampleLibraryCodegen::MyModuleSpec_Constants constants;
    constants.const1 = true;
    constants.const2 = 1.234;
    constants.const3 = "const3";
    return constants;
  }

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
  React::JSValue getObject(React::JSValue && /*arg*/) noexcept {
    return React::JSValueObject{{"X", 4}, {"Y", 2}};
  }

  REACT_SYNC_METHOD(getValue)
  React::JSValue getValue(double x, std::string y, JSValue &&z) noexcept {
    return React::JSValueObject{{"X", x}, {"Y", y}, {"Z", std::move(z)}};
  }

  REACT_METHOD(getValueWithCallback)
  void getValueWithCallback(std::function<void(std::string)> const &callback) noexcept {
    callback("some string");
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
