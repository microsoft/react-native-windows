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
#include <ReactCommon/TurboModule.h> // This comes from the react-native package.
#include <ReactCommon/TurboModuleUtils.h> // This must come from the react-native package, but we use a local version to fix issues.
#include <TurboModuleProvider.h> // It is RNW specific
#include <limits>
#include "TestEventService.h"
#include "TestReactNativeHostHolder.h"

using namespace facebook;
using namespace winrt;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {

// Use anonymous namespace to avoid any linking conflicts
namespace {

// In this test we put spec definition that normally must be generated.
// >>>> Start generated

// The spec from .h file
struct MySimpleTurboModuleSpec : react::TurboModule {
  virtual void logAction(jsi::Runtime &rt, const jsi::String &actionName, const jsi::Value &value) = 0;
  virtual void voidFunc(jsi::Runtime &rt) = 0;
  virtual bool getBool(jsi::Runtime &rt, bool arg) = 0;
  virtual double getNumber(jsi::Runtime &rt, double arg) = 0;
  virtual jsi::String getString(jsi::Runtime &rt, const jsi::String &arg) = 0;
  virtual jsi::Array getArray(jsi::Runtime &rt, const jsi::Array &arg) = 0;
  virtual jsi::Object getObject(jsi::Runtime &rt, const jsi::Object &arg) = 0;
  virtual jsi::Object getValue(jsi::Runtime &rt, double x, const jsi::String &y, const jsi::Object &z) = 0;
  virtual void getValueWithCallback(jsi::Runtime &rt, const jsi::Function &callback) = 0;
  virtual jsi::Value getValueWithPromise(jsi::Runtime &rt, bool error) = 0;
  virtual jsi::Object getConstants(jsi::Runtime &rt) = 0;

 protected:
  MySimpleTurboModuleSpec(std::shared_ptr<react::CallInvoker> jsInvoker);
};

// The spec from .cpp file

static jsi::Value MySimpleTurboModuleSpec_logAction(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 2);
  static_cast<MySimpleTurboModuleSpec *>(&turboModule)->logAction(rt, args[0].getString(rt), args[1]);
  return jsi::Value::undefined();
}

static jsi::Value MySimpleTurboModuleSpec_voidFunc(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 0);
  static_cast<MySimpleTurboModuleSpec *>(&turboModule)->voidFunc(rt);
  return jsi::Value::undefined();
}

static jsi::Value MySimpleTurboModuleSpec_getBool(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return jsi::Value(static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getBool(rt, args[0].getBool()));
}

static jsi::Value MySimpleTurboModuleSpec_getNumber(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return jsi::Value(static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getNumber(rt, args[0].getNumber()));
}

static jsi::Value MySimpleTurboModuleSpec_getString(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getString(rt, args[0].getString(rt));
}

static jsi::Value MySimpleTurboModuleSpec_getArray(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getArray(rt, args[0].getObject(rt).getArray(rt));
}

static jsi::Value MySimpleTurboModuleSpec_getObject(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getObject(rt, args[0].getObject(rt));
}

static jsi::Value MySimpleTurboModuleSpec_getValue(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 3);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)
      ->getValue(rt, args[0].getNumber(), args[1].getString(rt), args[2].getObject(rt));
}

static jsi::Value MySimpleTurboModuleSpec_getValueWithCallback(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  static_cast<MySimpleTurboModuleSpec *>(&turboModule)
      ->getValueWithCallback(rt, std::move(args[0].getObject(rt).getFunction(rt)));
  return jsi::Value::undefined();
}

static jsi::Value MySimpleTurboModuleSpec_getValueWithPromise(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getValueWithPromise(rt, args[0].getBool());
}

static jsi::Value MySimpleTurboModuleSpec_getConstants(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    [[maybe_unused]] const jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 0);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getConstants(rt);
}

MySimpleTurboModuleSpec::MySimpleTurboModuleSpec(std::shared_ptr<react::CallInvoker> jsInvoker)
    : react::TurboModule("MySimpleTurboModule", std::move(jsInvoker)) {
  methodMap_.try_emplace("logAction", MethodMetadata{0, MySimpleTurboModuleSpec_logAction});
  methodMap_.try_emplace("voidFunc", MethodMetadata{0, MySimpleTurboModuleSpec_voidFunc});
  methodMap_.try_emplace("getBool", MethodMetadata{1, MySimpleTurboModuleSpec_getBool});
  methodMap_.try_emplace("getNumber", MethodMetadata{1, MySimpleTurboModuleSpec_getNumber});
  methodMap_.try_emplace("getString", MethodMetadata{1, MySimpleTurboModuleSpec_getString});
  methodMap_.try_emplace("getArray", MethodMetadata{1, MySimpleTurboModuleSpec_getArray});
  methodMap_.try_emplace("getObject", MethodMetadata{1, MySimpleTurboModuleSpec_getObject});
  methodMap_.try_emplace("getValue", MethodMetadata{3, MySimpleTurboModuleSpec_getValue});
  methodMap_.try_emplace("getValueWithCallback", MethodMetadata{1, MySimpleTurboModuleSpec_getValueWithCallback});
  methodMap_.try_emplace("getValueWithPromise", MethodMetadata{1, MySimpleTurboModuleSpec_getValueWithPromise});
  methodMap_.try_emplace("getConstants", MethodMetadata{0, MySimpleTurboModuleSpec_getConstants});
}

// <<<< End generated

struct MySimpleTurboModule : MySimpleTurboModuleSpec {
  MySimpleTurboModule(std::shared_ptr<react::CallInvoker> jsInvoker);

  void logAction(jsi::Runtime &rt, const jsi::String &actionName, const jsi::Value &value) override;
  void voidFunc(jsi::Runtime &rt) override;
  bool getBool(jsi::Runtime &rt, bool arg) override;
  double getNumber(jsi::Runtime &rt, double arg) override;
  jsi::String getString(jsi::Runtime &rt, const jsi::String &arg) override;
  jsi::Array getArray(jsi::Runtime &rt, const jsi::Array &arg) override;
  jsi::Object getObject(jsi::Runtime &rt, const jsi::Object &arg) override;
  jsi::Object getValue(jsi::Runtime &rt, double x, const jsi::String &y, const jsi::Object &z) override;
  void getValueWithCallback(jsi::Runtime &rt, const jsi::Function &callback) override;
  jsi::Value getValueWithPromise(jsi::Runtime &rt, bool error) override;
  jsi::Object getConstants(jsi::Runtime &rt) override;
};

MySimpleTurboModule::MySimpleTurboModule(std::shared_ptr<react::CallInvoker> jsInvoker)
    : MySimpleTurboModuleSpec(std::move(jsInvoker)) {}

void MySimpleTurboModule::logAction(jsi::Runtime &rt, const jsi::String &actionName, const jsi::Value &value) {
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

jsi::String MySimpleTurboModule::getString(jsi::Runtime &rt, const jsi::String &arg) {
  TestEventService::LogEvent("getString called", arg.utf8(rt));
  return jsi::String::createFromUtf8(rt, arg.utf8(rt));
}

jsi::Array MySimpleTurboModule::getArray(jsi::Runtime &rt, const jsi::Array &arg) {
  TestEventService::LogEvent("getArray called", arg.length(rt));
  return react::deepCopyJSIArray(rt, arg);
}

jsi::Object MySimpleTurboModule::getObject(jsi::Runtime &rt, const jsi::Object &arg) {
  TestEventService::LogEvent("getObject called", "OK");
  return react::deepCopyJSIObject(rt, arg);
}

jsi::Object MySimpleTurboModule::getValue(jsi::Runtime &rt, double x, const jsi::String &y, const jsi::Object &z) {
  TestEventService::LogEvent("getValue called", "OK");
  // Note: return type isn't type-safe.
  jsi::Object result(rt);
  result.setProperty(rt, "x", jsi::Value(x));
  result.setProperty(rt, "y", jsi::String::createFromUtf8(rt, y.utf8(rt)));
  result.setProperty(rt, "z", react::deepCopyJSIObject(rt, z));
  return result;
}

void MySimpleTurboModule::getValueWithCallback(jsi::Runtime &rt, const jsi::Function &callback) {
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
    AddTurboModuleProvider<MySimpleTurboModule>(packageBuilder, L"MySimpleTurboModule");
  }
};

} // namespace

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

} // namespace ReactNativeIntegrationTests
