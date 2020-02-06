// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <sstream>
#include "NativeModules.h"
#include "ReactModuleBuilderMock.h"

namespace winrt::Microsoft::ReactNative {

REACT_STRUCT(Point)
struct Point {
  REACT_FIELD(X)
  int X;

  REACT_FIELD(Y)
  int Y;
};

REACT_MODULE(SimpleNativeModule)
struct SimpleNativeModule {
  REACT_METHOD(Add)
  int Add(int x, int y) noexcept {
    return x + y;
  }

  REACT_METHOD(Negate)
  int Negate(int x) noexcept {
    return -x;
  }

  REACT_METHOD(SayHello)
  std::string SayHello() noexcept {
    return "Hello";
  }

  REACT_METHOD(StaticAdd)
  static int StaticAdd(int x, int y) noexcept {
    return x + y;
  }

  REACT_METHOD(StaticNegate)
  static int StaticNegate(int x) noexcept {
    return -x;
  }

  REACT_METHOD(StaticSayHello)
  static std::string StaticSayHello() noexcept {
    return "Hello";
  }

  REACT_METHOD(SayHello0)
  void SayHello0() noexcept {
    Message = "Hello_0";
  }

  REACT_METHOD(PrintPoint)
  void PrintPoint(Point pt) noexcept {
    std::stringstream ss;
    ss << "Point: (" << pt.X << ", " << pt.Y << ")";
    Message = ss.str();
  }

  REACT_METHOD(PrintLine)
  void PrintLine(Point start, Point end) noexcept {
    std::stringstream ss;
    ss << "Line: (" << start.X << ", " << start.Y << ")-(" << end.X << ", " << end.Y << ")";
    Message = ss.str();
  }

  REACT_METHOD(StaticSayHello1)
  static void StaticSayHello1() noexcept {
    StaticMessage = "Hello_1";
  }

  REACT_METHOD(StaticPrintPoint)
  static void StaticPrintPoint(Point pt) noexcept {
    std::stringstream ss;
    ss << "Static Point: (" << pt.X << ", " << pt.Y << ")";
    StaticMessage = ss.str();
  }

  REACT_METHOD(StaticPrintLine)
  static void StaticPrintLine(Point start, Point end) noexcept {
    std::stringstream ss;
    ss << "Static Line: (" << start.X << ", " << start.Y << ")-(" << end.X << ", " << end.Y << ")";
    StaticMessage = ss.str();
  }

  REACT_METHOD(AddCallback)
  void AddCallback(int x, int y, std::function<void(int)> const &resolve) noexcept {
    resolve(x + y);
  }

  REACT_METHOD(NegateCallback)
  void NegateCallback(int x, std::function<void(int)> const &resolve) noexcept {
    resolve(-x);
  }

  REACT_METHOD(SayHelloCallback)
  void SayHelloCallback(std::function<void(const std::string &)> const &resolve) noexcept {
    resolve("Hello_2");
  }

  REACT_METHOD(StaticAddCallback)
  static void StaticAddCallback(int x, int y, std::function<void(int)> const &resolve) noexcept {
    resolve(x + y);
  }

  REACT_METHOD(StaticNegateCallback)
  static void StaticNegateCallback(int x, std::function<void(int)> const &resolve) noexcept {
    resolve(-x);
  }

  REACT_METHOD(StaticSayHelloCallback)
  static void StaticSayHelloCallback(std::function<void(const std::string &)> const &resolve) noexcept {
    resolve("Static Hello_2");
  }

  REACT_METHOD(DivideCallbacks)
  void DivideCallbacks(
      int x,
      int y,
      std::function<void(int)> const &resolve,
      std::function<void(const std::string &)> const &reject) noexcept {
    if (y != 0) {
      resolve(x / y);
    } else {
      reject("Division by 0");
    }
  }

  REACT_METHOD(NegateCallbacks)
  void NegateCallbacks(
      int x,
      std::function<void(int)> const &resolve,
      std::function<void(const std::string &)> const &reject) noexcept {
    if (x >= 0) {
      resolve(-x);
    } else {
      reject("Already negative");
    }
  }

  REACT_METHOD(ResolveSayHelloCallbacks)
  void ResolveSayHelloCallbacks(
      std::function<void(const std::string &)> const &resolve,
      std::function<void(const std::string &)> const & /*reject*/) noexcept {
    resolve("Hello_3");
  }

  REACT_METHOD(RejectSayHelloCallbacks)
  void RejectSayHelloCallbacks(
      std::function<void(const std::string &)> const & /*resolve*/,
      std::function<void(const std::string &)> const &reject) noexcept {
    reject("Goodbye");
  }

  REACT_METHOD(StaticDivideCallbacks)
  static void StaticDivideCallbacks(
      int x,
      int y,
      std::function<void(int)> const &resolve,
      std::function<void(const std::string &)> const &reject) noexcept {
    if (y != 0) {
      resolve(x / y);
    } else {
      reject("Division by 0");
    }
  }

  REACT_METHOD(StaticNegateCallbacks)
  static void StaticNegateCallbacks(
      int x,
      std::function<void(int)> const &resolve,
      std::function<void(const std::string &)> const &reject) noexcept {
    if (x >= 0) {
      resolve(-x);
    } else {
      reject("Already negative");
    }
  }

  REACT_METHOD(StaticResolveSayHelloCallbacks)
  static void StaticResolveSayHelloCallbacks(
      std::function<void(const std::string &)> const &resolve,
      std::function<void(const std::string &)> const & /*reject*/) noexcept {
    resolve("Hello_3");
  }

  REACT_METHOD(StaticRejectSayHelloCallbacks)
  static void StaticRejectSayHelloCallbacks(
      std::function<void(const std::string &)> const & /*resolve*/,
      std::function<void(const std::string &)> const &reject) noexcept {
    reject("Goodbye");
  }

  REACT_METHOD(DividePromise)
  void DividePromise(int x, int y, ReactPromise<int> &&result) noexcept {
    if (y != 0) {
      result.Resolve(x / y);
    } else {
      ReactError error{};
      error.Message = "Division by 0";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(NegatePromise)
  void NegatePromise(int x, ReactPromise<int> &&result) noexcept {
    if (x >= 0) {
      result.Resolve(-x);
    } else {
      ReactError error{};
      error.Message = "Already negative";
      result.Reject(std::move(error));
    }
  }

  // Each macro has second optional parameter: JS name.
  REACT_METHOD(VoidPromise, L"voidPromise")
  void VoidPromise(int x, ReactPromise<void> &&result) noexcept {
    if (x % 2 == 0) {
      result.Resolve();
    } else {
      result.Reject("Odd unexpected");
    }
  }

  REACT_METHOD(ResolveSayHelloPromise)
  void ResolveSayHelloPromise(ReactPromise<std::string> &&result) noexcept {
    result.Resolve("Hello_4");
  }

  REACT_METHOD(RejectSayHelloPromise)
  void RejectSayHelloPromise(ReactPromise<std::string> &&result) noexcept {
    ReactError error{};
    error.Message = "Promise rejected";
    result.Reject(std::move(error));
  }

  REACT_METHOD(StaticDividePromise)
  static void StaticDividePromise(int x, int y, ReactPromise<int> &&result) noexcept {
    if (y != 0) {
      result.Resolve(x / y);
    } else {
      ReactError error{};
      error.Message = "Division by 0";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(StaticNegatePromise)
  static void StaticNegatePromise(int x, ReactPromise<int> &&result) noexcept {
    if (x >= 0) {
      result.Resolve(-x);
    } else {
      ReactError error{};
      error.Message = "Already negative";
      result.Reject(std::move(error));
    }
  }

  // Each macro has second optional parameter: JS name.
  REACT_METHOD(StaticVoidPromise, L"staticVoidPromise")
  void StaticVoidPromise(int x, ReactPromise<void> &&result) noexcept {
    if (x % 2 == 0) {
      result.Resolve();
    } else {
      result.Reject("Odd unexpected");
    }
  }

  REACT_METHOD(StaticResolveSayHelloPromise)
  static void StaticResolveSayHelloPromise(ReactPromise<std::string> &&result) noexcept {
    result.Resolve("Hello_4");
  }

  REACT_METHOD(StaticRejectSayHelloPromise)
  static void StaticRejectSayHelloPromise(ReactPromise<std::string> &&result) noexcept {
    ReactError error{};
    error.Message = "Promise rejected";
    result.Reject(std::move(error));
  }

  REACT_SYNC_METHOD(AddSync)
  int AddSync(int x, int y) noexcept {
    return x + y;
  }

  REACT_SYNC_METHOD(NegateSync)
  int NegateSync(int x) noexcept {
    return -x;
  }

  REACT_SYNC_METHOD(SayHelloSync)
  std::string SayHelloSync() noexcept {
    return "Hello";
  }

  REACT_SYNC_METHOD(StaticAddSync)
  static int StaticAddSync(int x, int y) noexcept {
    return x + y;
  }

  REACT_SYNC_METHOD(StaticNegateSync)
  static int StaticNegateSync(int x) noexcept {
    return -x;
  }

  REACT_SYNC_METHOD(StaticSayHelloSync)
  static std::string StaticSayHelloSync() noexcept {
    return "Hello";
  }

  REACT_CONSTANT(Constant1)
  const std::string Constant1{"MyConstant1"};

  REACT_CONSTANT(Constant2, L"const2")
  const std::string Constant2{"MyConstant2"};

  REACT_CONSTANT(Constant3, L"const3")
  static constexpr Point Constant3{/*X =*/2, /*Y =*/3};

  REACT_CONSTANT(Constant4)
  static constexpr Point Constant4{/*X =*/3, /*Y =*/4};

  REACT_CONSTANT_PROVIDER(Constant5)
  void Constant5(ReactConstantProvider &provider) noexcept {
    provider.Add(L"const51", Point{/*X =*/12, /*Y =*/14});
    provider.Add(L"const52", "MyConstant52");
  }

  REACT_CONSTANT_PROVIDER(Constant6)
  static void Constant6(ReactConstantProvider &provider) noexcept {
    provider.Add(L"const61", Point{/*X =*/15, /*Y =*/17});
    provider.Add(L"const62", "MyConstant62");
  }

  REACT_EVENT(OnIntResult1)
  std::function<void(int)> OnIntResult1;

  REACT_EVENT(OnPointResult2, L"onPointResult2")
  std::function<void(const Point &)> OnPointResult2;

  REACT_EVENT(OnObjectResult3)
  std::function<void(const JSValue &)> OnObjectResult3;

  std::string Message;
  static std::string StaticMessage;
};

/*static*/ std::string SimpleNativeModule::StaticMessage;

TEST_CLASS (NativeModuleTest) {
  ReactModuleBuilderMock m_builderMock{};
  IReactModuleBuilder m_moduleBuilder;
  Windows::Foundation::IInspectable m_moduleObject{nullptr};
  SimpleNativeModule *m_module;

  NativeModuleTest() {
    m_moduleBuilder = make<ReactModuleBuilderImpl>(m_builderMock);
    auto provider = MakeModuleProvider<SimpleNativeModule>();
    m_moduleObject = provider(m_moduleBuilder);
    auto reactModule = m_moduleObject.as<IBoxedValue>();
    m_module = &BoxedValue<SimpleNativeModule>::GetImpl(reactModule);
  }

  TEST_METHOD(TestMethodCall_Add) {
    m_builderMock.Call1(L"Add", std::function<void(int)>([](int result) noexcept { TestCheck(result == 8); }), 3, 5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_Negate) {
    m_builderMock.Call1(L"Negate", std::function<void(int)>([](int result) noexcept { TestCheck(result == -3); }), 3);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_SayHello) {
    m_builderMock.Call1(L"SayHello", std::function<void(const std::string &)>([](const std::string &result) noexcept {
                          TestCheck(result == "Hello");
                        }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticAdd) {
    m_builderMock.Call1(
        L"StaticAdd", std::function<void(int)>([](int result) noexcept { TestCheck(result == 25); }), 20, 5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegate) {
    m_builderMock.Call1(
        L"StaticNegate", std::function<void(int)>([](int result) noexcept { TestCheck(result == -7); }), 7);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticSayHello) {
    m_builderMock.Call1(L"StaticSayHello", std::function<void(const std::string &)>([
                        ](const std::string &result) noexcept { TestCheck(result == "Hello"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_SayHello0) {
    m_builderMock.Call0(L"SayHello0");
    TestCheck(m_module->Message == "Hello_0");
  }

  TEST_METHOD(TestMethodCall_PrintPoint) {
    m_builderMock.Call0(L"PrintPoint", Point{/*X =*/3, /*Y =*/5});
    TestCheck(m_module->Message == "Point: (3, 5)");
  }

  TEST_METHOD(TestMethodCall_PrintLine) {
    m_builderMock.Call0(L"PrintLine", Point{/*X =*/3, /*Y =*/5}, Point{/*X =*/6, /*Y =*/8});
    TestCheck(m_module->Message == "Line: (3, 5)-(6, 8)");
  }

  TEST_METHOD(TestMethodCall_StaticSayHello1) {
    m_builderMock.Call0(L"StaticSayHello1");
    TestCheck(SimpleNativeModule::StaticMessage == "Hello_1");
  }

  TEST_METHOD(TestMethodCall_StaticPrintPoint) {
    m_builderMock.Call0(L"StaticPrintPoint", Point{/*X =*/13, /*Y =*/15});
    TestCheck(SimpleNativeModule::StaticMessage == "Static Point: (13, 15)");
  }

  TEST_METHOD(TestMethodCall_StaticPrintLine) {
    m_builderMock.Call0(L"StaticPrintLine", Point{/*X =*/13, /*Y =*/15}, Point{/*X =*/16, /*Y =*/18});
    TestCheck(SimpleNativeModule::StaticMessage == "Static Line: (13, 15)-(16, 18)");
  }

  TEST_METHOD(TestMethodCall_AddCallback) {
    m_builderMock.Call1(
        L"AddCallback", std::function<void(int)>([](int result) noexcept { TestCheck(result == -1); }), 7, -8);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateCallback) {
    m_builderMock.Call1(
        L"NegateCallback", std::function<void(int)>([](int result) noexcept { TestCheck(result == -4); }), 4);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_SayHelloCallback) {
    m_builderMock.Call1(L"SayHelloCallback", std::function<void(const std::string &)>([
                        ](const std::string &result) noexcept { TestCheck(result == "Hello_2"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticAddCallback) {
    m_builderMock.Call1(
        L"StaticAddCallback", std::function<void(int)>([](int result) noexcept { TestCheck(result == 60); }), 4, 56);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateCallback) {
    m_builderMock.Call1(
        L"StaticNegateCallback", std::function<void(int)>([](int result) noexcept { TestCheck(result == -33); }), 33);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticSayHelloCallback) {
    m_builderMock.Call1(L"StaticSayHelloCallback", std::function<void(const std::string &)>([
                        ](const std::string &result) noexcept { TestCheck(result == "Static Hello_2"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_DivideCallbacks) {
    m_builderMock.Call2(
        L"DivideCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_DivideCallbacksError) {
    m_builderMock.Call2(
        L"DivideCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        0);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateCallbacks) {
    m_builderMock.Call2(
        L"NegateCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateCallbacksError) {
    m_builderMock.Call2(
        L"NegateCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_ResolveSayHelloCallbacks) {
    m_builderMock.Call2(
        L"ResolveSayHelloCallbacks",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_3"); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Goodbye"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_RejectSayHelloCallbacks) {
    m_builderMock.Call2(
        L"RejectSayHelloCallbacks",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_3"); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Goodbye"); }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticDivideCallbacks) {
    m_builderMock.Call2(
        L"StaticDivideCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticDivideCallbacksError) {
    m_builderMock.Call2(
        L"StaticDivideCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        0);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateCallbacks) {
    m_builderMock.Call2(
        L"StaticNegateCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateCallbacksError) {
    m_builderMock.Call2(
        L"StaticNegateCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticResolveSayHelloCallbacks) {
    m_builderMock.Call2(
        L"StaticResolveSayHelloCallbacks",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_3"); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Goodbye"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticRejectSayHelloCallbacks) {
    m_builderMock.Call2(
        L"StaticRejectSayHelloCallbacks",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_3"); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Goodbye"); }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_DividePromise) {
    m_builderMock.Call2(
        L"DividePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_DividePromiseError) {
    m_builderMock.Call2(
        L"DividePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        0);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegatePromise) {
    m_builderMock.Call2(
        L"NegatePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegatePromiseError) {
    m_builderMock.Call2(
        L"NegatePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_VoidPromise) {
    m_builderMock.Call2(
        L"voidPromise",
        std::function<void()>([]() noexcept {}),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Odd unexpected"); }),
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_VoidError) {
    m_builderMock.Call2(
        L"voidPromise",
        std::function<void()>([]() noexcept {}),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Odd unexpected"); }),
        3);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_ResolveSayHelloPromise) {
    m_builderMock.Call2(
        L"ResolveSayHelloPromise",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_4"); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Promise rejected"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_RejectSayHelloPromise) {
    m_builderMock.Call2(
        L"RejectSayHelloPromise",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_4"); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Promise rejected"); }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticDividePromise) {
    m_builderMock.Call2(
        L"StaticDividePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticDividePromiseError) {
    m_builderMock.Call2(
        L"StaticDividePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        0);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegatePromise) {
    m_builderMock.Call2(
        L"StaticNegatePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegatePromiseError) {
    m_builderMock.Call2(
        L"StaticNegatePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticVoidPromise) {
    m_builderMock.Call2(
        L"staticVoidPromise",
        std::function<void()>([]() noexcept {}),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Odd unexpected"); }),
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticVoidPromiseError) {
    m_builderMock.Call2(
        L"staticVoidPromise",
        std::function<void()>([]() noexcept {}),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Odd unexpected"); }),
        3);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticResolveSayHelloPromise) {
    m_builderMock.Call2(
        L"StaticResolveSayHelloPromise",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_4"); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Promise rejected"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticRejectSayHelloPromise) {
    m_builderMock.Call2(
        L"StaticRejectSayHelloPromise",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_4"); }),
        std::function<void(JSValue const &)>(
            [](JSValue const &error) noexcept { TestCheck(error["message"] == "Promise rejected"); }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodSyncCall_AddSync) {
    int result;
    m_builderMock.CallSync(L"AddSync", /*out*/ result, 3, 5);
    TestCheck(result == 8);
  }

  TEST_METHOD(TestMethodSyncCall_NegateSync) {
    int result;
    m_builderMock.CallSync(L"NegateSync", /*out*/ result, 7);
    TestCheck(result == -7);
  }

  TEST_METHOD(TestMethodSyncCall_SayHelloSync) {
    std::string result;
    m_builderMock.CallSync(L"SayHelloSync", /*out*/ result);
    TestCheck(result == "Hello");
  }

  TEST_METHOD(TestMethodSyncCall_StaticAddSync) {
    int result;
    m_builderMock.CallSync(L"StaticAddSync", /*out*/ result, 3, 5);
    TestCheck(result == 8);
  }

  TEST_METHOD(TestMethodSyncCall_StaticNegateSync) {
    int result;
    m_builderMock.CallSync(L"StaticNegateSync", /*out*/ result, 7);
    TestCheck(result == -7);
  }

  TEST_METHOD(TestMethodSyncCall_StaticSayHelloSync) {
    std::string result;
    m_builderMock.CallSync(L"StaticSayHelloSync", /*out*/ result);
    TestCheck(result == "Hello");
  }

  TEST_METHOD(TestConstants) {
    auto constants = m_builderMock.GetConstants();
    TestCheck(constants["Constant1"] == "MyConstant1");
    TestCheck(constants["const2"] == "MyConstant2");
    TestCheck(constants["const3"]["X"] == 2);
    TestCheck(constants["const3"]["Y"] == 3);
    TestCheck(constants["Constant4"]["X"] == 3);
    TestCheck(constants["Constant4"]["Y"] == 4);
    TestCheck(constants["const51"]["X"] == 12);
    TestCheck(constants["const51"]["Y"] == 14);
    TestCheck(constants["const52"] == "MyConstant52");
    TestCheck(constants["const61"]["X"] == 15);
    TestCheck(constants["const61"]["Y"] == 17);
    TestCheck(constants["const62"] == "MyConstant62");
  }

  TEST_METHOD(TestEvent_EventField1) {
    bool eventRaised = false;
    m_builderMock.SetEventHandler(L"OnIntResult1", std::function<void(int)>([&eventRaised](int eventArg) noexcept {
                                    TestCheck(eventArg == 42);
                                    eventRaised = true;
                                  }));

    m_module->OnIntResult1(42);
    TestCheck(eventRaised == true);
  }

  TEST_METHOD(TestEvent_EventField2) {
    bool eventRaised = false;
    m_builderMock.SetEventHandler(
        L"onPointResult2", std::function<void(const Point &)>([&eventRaised](const Point &eventArg) noexcept {
          TestCheck(eventArg.X == 4);
          TestCheck(eventArg.Y == 2);
          eventRaised = true;
        }));

    m_module->OnPointResult2(Point{/*X =*/4, /*Y =*/2});
    TestCheck(eventRaised == true);
  }

  TEST_METHOD(TestEvent_EventField3) {
    bool eventRaised = false;
    m_builderMock.SetEventHandler(
        L"OnObjectResult3", std::function<void(const JSValue &)>([&eventRaised](const JSValue &eventArg) noexcept {
          TestCheck(eventArg.Object().at("X").Int64() == 4);
          TestCheck(eventArg.Object().at("Y").Int64() == 2);
          eventRaised = true;
        }));

    JSValue data = JSValue();
    auto writer = MakeJSValueTreeWriter(data);
    writer.WriteObjectBegin();
    WriteProperty(writer, "X", 4);
    WriteProperty(writer, "Y", 2);
    writer.WriteObjectEnd();

    m_module->OnObjectResult3(data);
    TestCheck(eventRaised == true);
  }
};

} // namespace winrt::Microsoft::ReactNative
