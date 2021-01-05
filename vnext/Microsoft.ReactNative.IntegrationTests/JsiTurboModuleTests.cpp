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
#include <JSValue.h>
#include <ReactCommon/TurboModule.h> // This comes from the react-native package.
#include <ReactCommon/TurboModuleUtils.h> // This must come from the react-native package, but we use a local version to fix issues.
#include <TurboModuleProvider.h> // It is RNW specific

#include <condition_variable>
#include <limits>
#include <mutex>
#include <sstream>
#include <string>

using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;

namespace ReactNativeIntegrationTests {

// Use anonymous namespace to avoid any linking conflicts
namespace {

// In this test we put spec definition that normally must be generated.
// >>>> Start generated

// The spec from .h file
struct MySimpleTurboModuleSpec : ::facebook::react::TurboModule {
  virtual void
  logAction(facebook::jsi::Runtime &rt, const facebook::jsi::String &actionName, const facebook::jsi::Value &value) = 0;
  virtual void voidFunc(::facebook::jsi::Runtime &rt) = 0;
  virtual bool getBool(::facebook::jsi::Runtime &rt, bool arg) = 0;
  virtual double getNumber(::facebook::jsi::Runtime &rt, double arg) = 0;
  virtual ::facebook::jsi::String getString(::facebook::jsi::Runtime &rt, const ::facebook::jsi::String &arg) = 0;
  virtual ::facebook::jsi::Array getArray(::facebook::jsi::Runtime &rt, const ::facebook::jsi::Array &arg) = 0;
  virtual ::facebook::jsi::Object getObject(::facebook::jsi::Runtime &rt, const ::facebook::jsi::Object &arg) = 0;
  virtual ::facebook::jsi::Object getValue(
      ::facebook::jsi::Runtime &rt,
      double x,
      const ::facebook::jsi::String &y,
      const ::facebook::jsi::Object &z) = 0;
  virtual void getValueWithCallback(::facebook::jsi::Runtime &rt, const ::facebook::jsi::Function &callback) = 0;
  virtual ::facebook::jsi::Value getValueWithPromise(::facebook::jsi::Runtime &rt, bool error) = 0;
  virtual ::facebook::jsi::Object getConstants(::facebook::jsi::Runtime &rt) = 0;

 protected:
  MySimpleTurboModuleSpec(std::shared_ptr<facebook::react::CallInvoker> jsInvoker);
};

// The spec from .cpp file

static ::facebook::jsi::Value MySimpleTurboModuleSpec_logAction(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 2);
  static_cast<MySimpleTurboModuleSpec *>(&turboModule)->logAction(rt, args[0].getString(rt), args[1]);
  return ::facebook::jsi::Value::undefined();
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_voidFunc(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 0);
  static_cast<MySimpleTurboModuleSpec *>(&turboModule)->voidFunc(rt);
  return ::facebook::jsi::Value::undefined();
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getBool(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return ::facebook::jsi::Value(static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getBool(rt, args[0].getBool()));
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getNumber(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return ::facebook::jsi::Value(
      static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getNumber(rt, args[0].getNumber()));
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getString(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getString(rt, args[0].getString(rt));
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getArray(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getArray(rt, args[0].getObject(rt).getArray(rt));
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getObject(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getObject(rt, args[0].getObject(rt));
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getValue(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 3);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)
      ->getValue(rt, args[0].getNumber(), args[1].getString(rt), args[2].getObject(rt));
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getValueWithCallback(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  static_cast<MySimpleTurboModuleSpec *>(&turboModule)
      ->getValueWithCallback(rt, std::move(args[0].getObject(rt).getFunction(rt)));
  return ::facebook::jsi::Value::undefined();
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getValueWithPromise(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 1);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getValueWithPromise(rt, args[0].getBool());
}

static ::facebook::jsi::Value MySimpleTurboModuleSpec_getConstants(
    ::facebook::jsi::Runtime &rt,
    ::facebook::react::TurboModule &turboModule,
    [[maybe_unused]] const ::facebook::jsi::Value *args,
    [[maybe_unused]] size_t count) {
  assert(count >= 0);
  return static_cast<MySimpleTurboModuleSpec *>(&turboModule)->getConstants(rt);
}

MySimpleTurboModuleSpec::MySimpleTurboModuleSpec(std::shared_ptr<::facebook::react::CallInvoker> jsInvoker)
    : ::facebook::react::TurboModule("MySimpleTurboModule", std::move(jsInvoker)) {
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

struct TestAction {
  std::string ActionName;
  JSValue Value;
};

struct MySimpleTurboModule : MySimpleTurboModuleSpec {
  MySimpleTurboModule(std::shared_ptr<facebook::react::CallInvoker> jsInvoker);

  void logAction(facebook::jsi::Runtime &rt, const facebook::jsi::String &actionName, const facebook::jsi::Value &value)
      override;
  void voidFunc(facebook::jsi::Runtime &rt) override;
  bool getBool(facebook::jsi::Runtime &rt, bool arg) override;
  double getNumber(facebook::jsi::Runtime &rt, double arg) override;
  facebook::jsi::String getString(facebook::jsi::Runtime &rt, const facebook::jsi::String &arg) override;
  facebook::jsi::Array getArray(facebook::jsi::Runtime &rt, const facebook::jsi::Array &arg) override;
  facebook::jsi::Object getObject(facebook::jsi::Runtime &rt, const facebook::jsi::Object &arg) override;
  facebook::jsi::Object getValue(
      facebook::jsi::Runtime &rt,
      double x,
      const facebook::jsi::String &y,
      const facebook::jsi::Object &z) override;
  void getValueWithCallback(facebook::jsi::Runtime &rt, const facebook::jsi::Function &callback) override;
  facebook::jsi::Value getValueWithPromise(facebook::jsi::Runtime &rt, bool error) override;
  facebook::jsi::Object getConstants(facebook::jsi::Runtime &rt) override;

 public: // Code to report test results
  static void LogAction(std::string actionName, JSValue value) {
    auto lock = std::scoped_lock{s_mutex};
    s_currentAction.ActionName = std::move(actionName);
    s_currentAction.Value = std::move(value);
    ++s_actionIndex;
    s_cv.notify_all();
  }

  static std::mutex s_mutex;
  static std::condition_variable s_cv;
  static TestAction s_currentAction;
  static int s_actionIndex;
};

/*static*/ std::mutex MySimpleTurboModule::s_mutex;
/*static*/ std::condition_variable MySimpleTurboModule::s_cv;
/*static*/ TestAction MySimpleTurboModule::s_currentAction;
/*static*/ int MySimpleTurboModule::s_actionIndex{-1};

MySimpleTurboModule::MySimpleTurboModule(std::shared_ptr<facebook::react::CallInvoker> jsInvoker)
    : MySimpleTurboModuleSpec(std::move(jsInvoker)) {}

void MySimpleTurboModule::logAction(
    facebook::jsi::Runtime &rt,
    const facebook::jsi::String &actionName,
    const facebook::jsi::Value &value) {
  JSValue jsValue{};
  if (value.isBool()) {
    jsValue = JSValue(value.getBool());
  } else if (value.isNumber()) {
    jsValue = JSValue(value.getNumber());
  } else if (value.isString()) {
    jsValue = JSValue(value.getString(rt).utf8(rt));
  }
  LogAction(actionName.utf8(rt), std::move(jsValue));
}

void MySimpleTurboModule::voidFunc(facebook::jsi::Runtime & /*rt*/) {
  LogAction("voidFunc called", nullptr);
}

bool MySimpleTurboModule::getBool(facebook::jsi::Runtime & /*rt*/, bool arg) {
  LogAction("getBool called", arg);
  return arg;
}

double MySimpleTurboModule::getNumber(facebook::jsi::Runtime & /*rt*/, double arg) {
  LogAction("getNumber called", arg);
  return arg;
}

facebook::jsi::String MySimpleTurboModule::getString(facebook::jsi::Runtime &rt, const facebook::jsi::String &arg) {
  LogAction("getString called", arg.utf8(rt));
  return facebook::jsi::String::createFromUtf8(rt, arg.utf8(rt));
}

facebook::jsi::Array MySimpleTurboModule::getArray(facebook::jsi::Runtime &rt, const facebook::jsi::Array &arg) {
  LogAction("getArray called", arg.length(rt));
  return facebook::react::deepCopyJSIArray(rt, arg);
}

facebook::jsi::Object MySimpleTurboModule::getObject(facebook::jsi::Runtime &rt, const facebook::jsi::Object &arg) {
  LogAction("getObject called", "OK");
  return facebook::react::deepCopyJSIObject(rt, arg);
}

facebook::jsi::Object MySimpleTurboModule::getValue(
    facebook::jsi::Runtime &rt,
    double x,
    const facebook::jsi::String &y,
    const facebook::jsi::Object &z) {
  LogAction("getValue called", "OK");
  // Note: return type isn't type-safe.
  facebook::jsi::Object result(rt);
  result.setProperty(rt, "x", facebook::jsi::Value(x));
  result.setProperty(rt, "y", facebook::jsi::String::createFromUtf8(rt, y.utf8(rt)));
  result.setProperty(rt, "z", facebook::react::deepCopyJSIObject(rt, z));
  return result;
}

void MySimpleTurboModule::getValueWithCallback(facebook::jsi::Runtime &rt, const facebook::jsi::Function &callback) {
  LogAction("getValueWithCallback called", "OK");
  callback.call(rt, facebook::jsi::String::createFromUtf8(rt, "value from callback!"));
}

facebook::jsi::Value MySimpleTurboModule::getValueWithPromise(facebook::jsi::Runtime &rt, bool error) {
  LogAction("getValueWithPromise called", error);
  return facebook::react::createPromiseAsJSIValue(
      rt, [error](facebook::jsi::Runtime &rt2, std::shared_ptr<facebook::react::Promise> promise) {
        if (error) {
          promise->reject("intentional promise rejection");
        } else {
          promise->resolve(facebook::jsi::String::createFromUtf8(rt2, "result!"));
        }
      });
}

facebook::jsi::Object MySimpleTurboModule::getConstants(facebook::jsi::Runtime &rt) {
  LogAction("getConstants called", "OK");
  // Note: return type isn't type-safe.
  facebook::jsi::Object result(rt);
  result.setProperty(rt, "const1", facebook::jsi::Value(true));
  result.setProperty(rt, "const2", facebook::jsi::Value(375));
  result.setProperty(rt, "const3", facebook::jsi::String::createFromUtf8(rt, "something"));
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
    MySimpleTurboModule::s_actionIndex = -1;
    TestAction expectedActions[] = {
        TestAction{"voidFunc called", nullptr},
        TestAction{"getBool called", true},
        TestAction{"getBool result", true},
        TestAction{"getBool called", false},
        TestAction{"getBool result", false},
        TestAction{"getNumber called", 5.0},
        TestAction{"getNumber result", 5.0},
        TestAction{"getNumber called", std::numeric_limits<double>::quiet_NaN()},
        TestAction{"getNumber result", std::numeric_limits<double>::quiet_NaN()},
        TestAction{"getNumber called", std::numeric_limits<double>::infinity()},
        TestAction{"getNumber result", std::numeric_limits<double>::infinity()},
        TestAction{"getString called", "Hello"},
        TestAction{"getString result", "Hello"},
        TestAction{"getString called", ""},
        TestAction{"getString result", ""},
        TestAction{"getArray called", 3},
        TestAction{"getArray result", "OK"},
        TestAction{"getObject called", "OK"},
        TestAction{"getObject result", "OK"},
        TestAction{"getValue called", "OK"},
        TestAction{"getValue result", "OK"},
        TestAction{"getConstants called", "OK"},
        TestAction{"getConstants result", "OK"},
        TestAction{"getValueWithCallback called", "OK"},
        TestAction{"getValueWithCallback result", "value from callback!"},
        TestAction{"getValueWithPromise called", false},
        TestAction{"getValueWithPromise called", true},
        TestAction{"getValueWithPromise result resolve", "result!"},
        TestAction{"getValueWithPromise result reject", "intentional promise rejection"},
    };

    ReactNativeHost host{};

    auto queueController = winrt::Windows::System::DispatcherQueueController::CreateOnDedicatedThread();
    queueController.DispatcherQueue().TryEnqueue([&]() noexcept {
      host.PackageProviders().Append(winrt::make<MySimpleTurboModulePackageProvider>());

      // bundle is assumed to be co-located with the test binary
      wchar_t testBinaryPath[MAX_PATH];
      TestCheck(GetModuleFileNameW(NULL, testBinaryPath, MAX_PATH) < MAX_PATH);
      testBinaryPath[std::wstring_view{testBinaryPath}.rfind(L"\\")] = 0;

      host.InstanceSettings().BundleRootPath(testBinaryPath);
      host.InstanceSettings().JavaScriptBundleFile(L"JsiTurboModuleTests");
      host.InstanceSettings().UseDeveloperSupport(false);
      host.InstanceSettings().UseWebDebugger(false);
      host.InstanceSettings().UseFastRefresh(false);
      host.InstanceSettings().UseLiveReload(false);
      host.InstanceSettings().EnableDeveloperMenu(false);

      host.LoadInstance().Completed(
          [](IAsyncAction asyncInfo, AsyncStatus asyncStatus) { TestCheckEqual(AsyncStatus::Completed, asyncStatus); });
    });

    auto lock = std::unique_lock{MySimpleTurboModule::s_mutex};
    MySimpleTurboModule::s_cv.wait(lock, [&]() {
      if (MySimpleTurboModule::s_actionIndex >= 0) {
        auto const &expectedAction = expectedActions[MySimpleTurboModule::s_actionIndex];
        TestCheckEqual(expectedAction.ActionName, MySimpleTurboModule::s_currentAction.ActionName);
        if (auto d1 = expectedAction.Value.TryGetDouble(),
            d2 = MySimpleTurboModule::s_currentAction.Value.TryGetDouble();
            d1 && d2) {
          if (!isnan(*d1) && !isnan(*d2)) {
            TestCheckEqual(*d1, *d2);
          }
        } else if (expectedAction.Value != MySimpleTurboModule::s_currentAction.Value) {
          std::stringstream os;
          os << "Action index: " << MySimpleTurboModule::s_actionIndex << '\n'
             << "Expected: " << expectedAction.Value.ToString() << '\n'
             << "Actual: " << MySimpleTurboModule::s_currentAction.Value.ToString();
          TestCheckFail("%s", os.str().c_str());
        }
      }
      return MySimpleTurboModule::s_actionIndex >= static_cast<ptrdiff_t>(std::size(expectedActions)) - 1;
    });

    // Make sure that we did all actions
    TestCheckEqual(MySimpleTurboModule::s_actionIndex, static_cast<ptrdiff_t>(std::size(expectedActions)) - 1);
    lock.unlock();

    host.UnloadInstance().get();
    queueController.ShutdownQueueAsync().get();
  }
};

} // namespace ReactNativeIntegrationTests
