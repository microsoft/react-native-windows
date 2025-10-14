// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// The TurboModule sample is based on the code from the react-native NPM package.
// Below is the original copyright notice for that code.
// The referred LICENSE file is in the root of the react-native NPM package.
/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// This test shows how we can use the react-native style of C++ TurboModules in
// ReactNative Windows. They work on top of ABI safe JSI implementation.
// The only RNW specific part is the registration of TurboModule class using IReactPackageBuilder.
// See the details for the MySimpleTurboModulePackageProvider below.

#include "pch.h"
#include <ReactCommon/TurboModule.h>
#include <ReactCommon/TurboModuleUtils.h>
#include <TurboModuleProvider.h> // It is RNW specific
#include <limits>
#include "TestEventService.h"
#include "TestReactNativeHostHolder.h"

#pragma pack(push)
#pragma warning(disable : 4100 4127 4324)
#include "codegen/msrnIntegrationTestsJSI.h"
#pragma pack(pop)

using namespace facebook;
using namespace winrt;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

// Use anonymous namespace to avoid any linking conflicts
namespace {

jsi::Value deepCopyJSIValue(jsi::Runtime &rt, const jsi::Value &value);
jsi::Object deepCopyJSIObject(jsi::Runtime &rt, const jsi::Object &obj);
jsi::Array deepCopyJSIArray(jsi::Runtime &rt, const jsi::Array &arr);

struct MySimpleTurboModule : react::NativeMySimpleTurboModuleCxxCxxSpecJSI {
  MySimpleTurboModule(std::shared_ptr<react::CallInvoker> jsInvoker);

  void logAction(jsi::Runtime &rt, const jsi::String actionName, jsi::Value value) override;
  void voidFunc(jsi::Runtime &rt) override;
  bool getBool(jsi::Runtime &rt, bool arg) override;
  double getNumber(jsi::Runtime &rt, double arg) override;
  jsi::String getString(jsi::Runtime &rt, jsi::String arg) override;
  jsi::Array getArray(jsi::Runtime &rt, jsi::Array arg) override;
  jsi::Object getObject(jsi::Runtime &rt, jsi::Object arg) override;
  jsi::Object getValue(jsi::Runtime &rt, double x, jsi::String y, jsi::Object z) override;
  void getValueWithCallback(jsi::Runtime &rt, jsi::Function callback) override;
  jsi::Value getValueWithPromise(jsi::Runtime &rt, bool error) override;
  jsi::Object getConstants(jsi::Runtime &rt) override;
};

MySimpleTurboModule::MySimpleTurboModule(std::shared_ptr<react::CallInvoker> jsInvoker)
    : NativeMySimpleTurboModuleCxxCxxSpecJSI(std::move(jsInvoker)) {}

void MySimpleTurboModule::logAction(jsi::Runtime &rt, jsi::String actionName, jsi::Value value) {
  JSValue jsValue{};
  if (value.isBool()) {
    jsValue = JSValue(value.getBool());
  } else if (value.isNumber()) {
    jsValue = JSValue(value.getNumber());
  } else if (value.isString()) {
    jsValue = JSValue(value.getString(rt).utf8(rt));
  }
  TestEventService::LogEvent(actionName.utf8(rt), std::move(jsValue));
}

void MySimpleTurboModule::voidFunc(jsi::Runtime & /*rt*/) {
  TestEventService::LogEvent("voidFunc called", nullptr);
}

bool MySimpleTurboModule::getBool(jsi::Runtime & /*rt*/, bool arg) {
  TestEventService::LogEvent("getBool called", arg);
  return arg;
}

double MySimpleTurboModule::getNumber(jsi::Runtime & /*rt*/, double arg) {
  TestEventService::LogEvent("getNumber called", arg);
  return arg;
}

jsi::String MySimpleTurboModule::getString(jsi::Runtime &rt, jsi::String arg) {
  TestEventService::LogEvent("getString called", arg.utf8(rt));
  return jsi::String::createFromUtf8(rt, arg.utf8(rt));
}

jsi::Array MySimpleTurboModule::getArray(jsi::Runtime &rt, jsi::Array arg) {
  TestEventService::LogEvent("getArray called", arg.length(rt));
  return deepCopyJSIArray(rt, arg);
}

jsi::Object MySimpleTurboModule::getObject(jsi::Runtime &rt, jsi::Object arg) {
  TestEventService::LogEvent("getObject called", "OK");
  return deepCopyJSIObject(rt, arg);
}

jsi::Object MySimpleTurboModule::getValue(jsi::Runtime &rt, double x, jsi::String y, jsi::Object z) {
  TestEventService::LogEvent("getValue called", "OK");
  // Note: return type isn't type-safe.
  jsi::Object result(rt);
  result.setProperty(rt, "x", jsi::Value(x));
  result.setProperty(rt, "y", jsi::String::createFromUtf8(rt, y.utf8(rt)));
  result.setProperty(rt, "z", deepCopyJSIObject(rt, z));
  return result;
}

void MySimpleTurboModule::getValueWithCallback(jsi::Runtime &rt, jsi::Function callback) {
  TestEventService::LogEvent("getValueWithCallback called", "OK");
  callback.call(rt, jsi::String::createFromUtf8(rt, "value from callback!"));
}

jsi::Value MySimpleTurboModule::getValueWithPromise(jsi::Runtime &rt, bool error) {
  TestEventService::LogEvent("getValueWithPromise called", error);
  return react::createPromiseAsJSIValue(rt, [error](jsi::Runtime &rt2, std::shared_ptr<react::Promise> promise) {
    if (error) {
      promise->reject("intentional promise rejection");
    } else {
      promise->resolve(jsi::String::createFromUtf8(rt2, "result!"));
    }
  });
}

jsi::Object MySimpleTurboModule::getConstants(jsi::Runtime &rt) {
  TestEventService::LogEvent("getConstants called", "OK");
  // Note: return type isn't type-safe.
  jsi::Object result(rt);
  result.setProperty(rt, "const1", jsi::Value(true));
  result.setProperty(rt, "const2", jsi::Value(375));
  result.setProperty(rt, "const3", jsi::String::createFromUtf8(rt, "something"));
  return result;
}

struct MySimpleTurboModulePackageProvider
    : winrt::implements<MySimpleTurboModulePackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    AddTurboModuleProvider<MySimpleTurboModule>(packageBuilder, L"MySimpleTurboModuleCxx");
  }
};

jsi::Value deepCopyJSIValue(jsi::Runtime &rt, const jsi::Value &value) {
  if (value.isNull()) {
    return jsi::Value::null();
  }

  if (value.isBool()) {
    return jsi::Value(value.getBool());
  }

  if (value.isNumber()) {
    return jsi::Value(value.getNumber());
  }

  if (value.isString()) {
    return value.getString(rt);
  }

  if (value.isObject()) {
    jsi::Object o = value.getObject(rt);
    if (o.isArray(rt)) {
      return deepCopyJSIArray(rt, o.getArray(rt));
    }
    if (o.isFunction(rt)) {
      return o.getFunction(rt);
    }
    return deepCopyJSIObject(rt, o);
  }

  return jsi::Value::undefined();
}

jsi::Object deepCopyJSIObject(jsi::Runtime &rt, const jsi::Object &obj) {
  jsi::Object copy(rt);
  jsi::Array propertyNames = obj.getPropertyNames(rt);
  size_t size = propertyNames.size(rt);
  for (size_t i = 0; i < size; i++) {
    jsi::String name = propertyNames.getValueAtIndex(rt, i).getString(rt);
    jsi::Value value = obj.getProperty(rt, name);
    copy.setProperty(rt, name, deepCopyJSIValue(rt, value));
  }
  return copy;
}

jsi::Array deepCopyJSIArray(jsi::Runtime &rt, const jsi::Array &arr) {
  size_t size = arr.size(rt);
  jsi::Array copy(rt, size);
  for (size_t i = 0; i < size; i++) {
    copy.setValueAtIndex(rt, i, deepCopyJSIValue(rt, arr.getValueAtIndex(rt, i)));
  }
  return copy;
}

} // namespace

// The test fails to run when compiled on x86. It is most probably of the mix of sdtcall vs cdecl calling conventions
// that we use in the test and RNW code. Ideally, we must switch to the default cdecl as soon as we remove the non-ABI
// safe APIs.
#ifndef _M_IX86
TEST_CLASS (JsiTurboModuleTests) {
  TEST_METHOD(ExecuteSampleTurboModule) {
    TestEventService::Initialize();

    auto reactNativeHost = TestReactNativeHostHolder(L"JsiTurboModuleTests", [](ReactNativeHost const &host) noexcept {
      host.PackageProviders().Append(winrt::make<MySimpleTurboModulePackageProvider>());
    });

    TestEventService::ObserveEvents(
        {TestEvent{"voidFunc called", nullptr},
         TestEvent{"getBool called", true},
         TestEvent{"getBool result", true},
         TestEvent{"getBool called", false},
         TestEvent{"getBool result", false},
         TestEvent{"getNumber called", 5.0},
         TestEvent{"getNumber result", 5.0},
         TestEvent{"getNumber called", std::numeric_limits<double>::quiet_NaN()},
         TestEvent{"getNumber result", std::numeric_limits<double>::quiet_NaN()},
         TestEvent{"getNumber called", std::numeric_limits<double>::infinity()},
         TestEvent{"getNumber result", std::numeric_limits<double>::infinity()},
         TestEvent{"getString called", "Hello"},
         TestEvent{"getString result", "Hello"},
         TestEvent{"getString called", ""},
         TestEvent{"getString result", ""},
         TestEvent{"getArray called", 3},
         TestEvent{"getArray result", "OK"},
         TestEvent{"getObject called", "OK"},
         TestEvent{"getObject result", "OK"},
         TestEvent{"getValue called", "OK"},
         TestEvent{"getValue result", "OK"},
         TestEvent{"getConstants called", "OK"},
         TestEvent{"getConstants result", "OK"},
         TestEvent{"getValueWithCallback called", "OK"},
         TestEvent{"getValueWithCallback result", "value from callback!"},
         TestEvent{"getValueWithPromise called", false},
         TestEvent{"getValueWithPromise called", true},
         TestEvent{"getValueWithPromise result resolve", "result!"},
         TestEvent{"getValueWithPromise result reject", "intentional promise rejection"}});
  }
};
#endif

} // namespace ReactNativeIntegrationTests
