// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <ReactCommon/TurboModuleUtils.h>
#include "../../codegen/SampleAppJSI-generated.cpp"
#include "MyJsiModule.h"

namespace SampleLibraryCpp {

MyJsiModuleCxx::MyJsiModuleCxx(std::shared_ptr<facebook::react::CallInvoker> jsInvoker)
    : NativeMyJsiModuleCxxSpecJSI(jsInvoker) {}

facebook::jsi::Object MyJsiModuleCxx::getConstants(facebook::jsi::Runtime &rt) {
  facebook::jsi::Object result(rt);
  result.setProperty(rt, "const1", facebook::jsi::Value(true));
  result.setProperty(rt, "const2", facebook::jsi::Value(375));
  result.setProperty(rt, "const3", facebook::jsi::String::createFromUtf8(rt, "something"));
  return result;
}

void MyJsiModuleCxx::voidFunc(facebook::jsi::Runtime &rt) {
  // Nothing to do
}

bool MyJsiModuleCxx::getBool(facebook::jsi::Runtime &rt, bool arg) {
  return arg;
}

double MyJsiModuleCxx::getNumber(facebook::jsi::Runtime &rt, double arg) {
  return arg;
}

facebook::jsi::String MyJsiModuleCxx::getString(facebook::jsi::Runtime &rt, facebook::jsi::String arg) {
  return facebook::jsi::String::createFromUtf8(rt, arg.utf8(rt));
}

facebook::jsi::Array MyJsiModuleCxx::getArray(facebook::jsi::Runtime &rt, facebook::jsi::Array arg) {
  return facebook::react::deepCopyJSIArray(rt, arg);
}

facebook::jsi::Object MyJsiModuleCxx::getObject(facebook::jsi::Runtime &rt, facebook::jsi::Object arg) {
  return facebook::react::deepCopyJSIObject(rt, arg);
}

facebook::jsi::Object
MyJsiModuleCxx::getValue(facebook::jsi::Runtime &rt, double x, facebook::jsi::String y, facebook::jsi::Object z) {
  // Note: return type isn't type-safe.
  facebook::jsi::Object result(rt);
  result.setProperty(rt, "x", facebook::jsi::Value(x));
  result.setProperty(rt, "y", facebook::jsi::String::createFromUtf8(rt, y.utf8(rt)));
  result.setProperty(rt, "z", facebook::react::deepCopyJSIObject(rt, z));
  return result;
}

void MyJsiModuleCxx::getValueWithCallback(facebook::jsi::Runtime &rt, facebook::jsi::Function callback) {
  callback.call(rt, facebook::jsi::String::createFromUtf8(rt, "value from callback!"));
}

facebook::jsi::Value MyJsiModuleCxx::getValueWithPromise(facebook::jsi::Runtime &rt, bool error) {
  return facebook::react::createPromiseAsJSIValue(
      rt, [error](facebook::jsi::Runtime &rt2, std::shared_ptr<facebook::react::Promise> promise) {
        if (error) {
          promise->reject("intentional promise rejection");
        } else {
          promise->resolve(facebook::jsi::String::createFromUtf8(rt2, "result!"));
        }
      });
}

} // namespace SampleLibraryCpp
