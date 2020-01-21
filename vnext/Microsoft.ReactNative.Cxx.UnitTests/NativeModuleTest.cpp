// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <sstream>
#include "NativeModules.h"
#include "ReactModuleBuilderMock.h"
#include "catch.hpp"

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

// Common base class used by all unit tests below.
struct NativeModuleTestFixture {
  NativeModuleTestFixture() {
    m_moduleBuilder = make<ReactModuleBuilderImpl>(m_builderMock);
    auto provider = MakeModuleProvider<SimpleNativeModule>();
    m_moduleObject = provider(m_moduleBuilder);
    auto reactModule = m_moduleObject.as<IBoxedValue>();
    m_module = &BoxedValue<SimpleNativeModule>::GetImpl(reactModule);
  }

 protected:
  ReactModuleBuilderMock m_builderMock{};
  IReactModuleBuilder m_moduleBuilder;
  Windows::Foundation::IInspectable m_moduleObject{nullptr};
  SimpleNativeModule *m_module;
};

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_Add", "NativeModuleTest") {
  m_builderMock.Call1(L"Add", std::function<void(int)>([](int result) noexcept { REQUIRE(result == 8); }), 3, 5);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_Negate", "NativeModuleTest") {
  m_builderMock.Call1(L"Negate", std::function<void(int)>([](int result) noexcept { REQUIRE(result == -3); }), 3);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_SayHello", "NativeModuleTest") {
  m_builderMock.Call1(L"SayHello", std::function<void(const std::string &)>([](const std::string &result) noexcept {
                        REQUIRE(result == "Hello");
                      }));
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticAdd", "NativeModuleTest") {
  m_builderMock.Call1(
      L"StaticAdd", std::function<void(int)>([](int result) noexcept { REQUIRE(result == 25); }), 20, 5);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticNegate", "NativeModuleTest") {
  m_builderMock.Call1(L"StaticNegate", std::function<void(int)>([](int result) noexcept { REQUIRE(result == -7); }), 7);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticSayHello", "NativeModuleTest") {
  m_builderMock.Call1(
      L"StaticSayHello",
      std::function<void(const std::string &)>([](const std::string &result) noexcept { REQUIRE(result == "Hello"); }));
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_SayHello0", "NativeModuleTest") {
  m_builderMock.Call0(L"SayHello0");
  REQUIRE(m_module->Message == "Hello_0");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_PrintPoint", "NativeModuleTest") {
  m_builderMock.Call0(L"PrintPoint", Point{/*X =*/3, /*Y =*/5});
  REQUIRE(m_module->Message == "Point: (3, 5)");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_PrintLine", "NativeModuleTest") {
  m_builderMock.Call0(L"PrintLine", Point{/*X =*/3, /*Y =*/5}, Point{/*X =*/6, /*Y =*/8});
  REQUIRE(m_module->Message == "Line: (3, 5)-(6, 8)");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticSayHello1", "NativeModuleTest") {
  m_builderMock.Call0(L"StaticSayHello1");
  REQUIRE(SimpleNativeModule::StaticMessage == "Hello_1");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticPrintPoint", "NativeModuleTest") {
  m_builderMock.Call0(L"StaticPrintPoint", Point{/*X =*/13, /*Y =*/15});
  REQUIRE(SimpleNativeModule::StaticMessage == "Static Point: (13, 15)");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticPrintLine", "NativeModuleTest") {
  m_builderMock.Call0(L"StaticPrintLine", Point{/*X =*/13, /*Y =*/15}, Point{/*X =*/16, /*Y =*/18});
  REQUIRE(SimpleNativeModule::StaticMessage == "Static Line: (13, 15)-(16, 18)");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_AddCallback", "NativeModuleTest") {
  m_builderMock.Call1(
      L"AddCallback", std::function<void(int)>([](int result) noexcept { REQUIRE(result == -1); }), 7, -8);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_NegateCallback", "NativeModuleTest") {
  m_builderMock.Call1(
      L"NegateCallback", std::function<void(int)>([](int result) noexcept { REQUIRE(result == -4); }), 4);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_SayHelloCallback", "NativeModuleTest") {
  m_builderMock.Call1(L"SayHelloCallback", std::function<void(const std::string &)>([
                      ](const std::string &result) noexcept { REQUIRE(result == "Hello_2"); }));
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticAddCallback", "NativeModuleTest") {
  m_builderMock.Call1(
      L"StaticAddCallback", std::function<void(int)>([](int result) noexcept { REQUIRE(result == 60); }), 4, 56);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticNegateCallback", "NativeModuleTest") {
  m_builderMock.Call1(
      L"StaticNegateCallback", std::function<void(int)>([](int result) noexcept { REQUIRE(result == -33); }), 33);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticSayHelloCallback", "NativeModuleTest") {
  m_builderMock.Call1(L"StaticSayHelloCallback", std::function<void(const std::string &)>([
                      ](const std::string &result) noexcept { REQUIRE(result == "Static Hello_2"); }));
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_DivideCallbacks", "NativeModuleTest") {
  m_builderMock.Call2(
      L"DivideCallbacks",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == 3); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Division by 0"); }),
      6,
      2);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_DivideCallbacksError", "NativeModuleTest") {
  m_builderMock.Call2(
      L"DivideCallbacks",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == 3); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Division by 0"); }),
      6,
      0);
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_NegateCallbacks", "NativeModuleTest") {
  m_builderMock.Call2(
      L"NegateCallbacks",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == -5); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Already negative"); }),
      5);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_NegateCallbacksError", "NativeModuleTest") {
  m_builderMock.Call2(
      L"NegateCallbacks",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == -5); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Already negative"); }),
      -5);
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_ResolveSayHelloCallbacks", "NativeModuleTest") {
  m_builderMock.Call2(
      L"ResolveSayHelloCallbacks",
      std::function<void(const std::string &)>(
          [](const std::string &result) noexcept { REQUIRE(result == "Hello_3"); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Goodbye"); }));
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_RejectSayHelloCallbacks", "NativeModuleTest") {
  m_builderMock.Call2(
      L"RejectSayHelloCallbacks",
      std::function<void(const std::string &)>(
          [](const std::string &result) noexcept { REQUIRE(result == "Hello_3"); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Goodbye"); }));
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticDivideCallbacks", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticDivideCallbacks",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == 3); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Division by 0"); }),
      6,
      2);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticDivideCallbacksError", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticDivideCallbacks",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == 3); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Division by 0"); }),
      6,
      0);
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticNegateCallbacks", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticNegateCallbacks",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == -5); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Already negative"); }),
      5);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticNegateCallbacksError", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticNegateCallbacks",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == -5); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Already negative"); }),
      -5);
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticResolveSayHelloCallbacks", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticResolveSayHelloCallbacks",
      std::function<void(const std::string &)>(
          [](const std::string &result) noexcept { REQUIRE(result == "Hello_3"); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Goodbye"); }));
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticRejectSayHelloCallbacks", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticRejectSayHelloCallbacks",
      std::function<void(const std::string &)>(
          [](const std::string &result) noexcept { REQUIRE(result == "Hello_3"); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Goodbye"); }));
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_DividePromise", "NativeModuleTest") {
  m_builderMock.Call2(
      L"DividePromise",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == 3); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Division by 0"); }),
      6,
      2);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_DividePromiseError", "NativeModuleTest") {
  m_builderMock.Call2(
      L"DividePromise",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == 3); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Division by 0"); }),
      6,
      0);
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_NegatePromise", "NativeModuleTest") {
  m_builderMock.Call2(
      L"NegatePromise",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == -5); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Already negative"); }),
      5);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_NegatePromiseError", "NativeModuleTest") {
  m_builderMock.Call2(
      L"NegatePromise",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == -5); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Already negative"); }),
      -5);
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_ResolveSayHelloPromise", "NativeModuleTest") {
  m_builderMock.Call2(
      L"ResolveSayHelloPromise",
      std::function<void(const std::string &)>(
          [](const std::string &result) noexcept { REQUIRE(result == "Hello_4"); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Promise rejected"); }));
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_RejectSayHelloPromise", "NativeModuleTest") {
  m_builderMock.Call2(
      L"RejectSayHelloPromise",
      std::function<void(const std::string &)>(
          [](const std::string &result) noexcept { REQUIRE(result == "Hello_4"); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Promise rejected"); }));
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticDividePromise", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticDividePromise",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == 3); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Division by 0"); }),
      6,
      2);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticDividePromiseError", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticDividePromise",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == 3); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Division by 0"); }),
      6,
      0);
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticNegatePromise", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticNegatePromise",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == -5); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Already negative"); }),
      5);
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticNegatePromiseError", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticNegatePromise",
      std::function<void(int)>([](int result) noexcept { REQUIRE(result == -5); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Already negative"); }),
      -5);
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticResolveSayHelloPromise", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticResolveSayHelloPromise",
      std::function<void(const std::string &)>(
          [](const std::string &result) noexcept { REQUIRE(result == "Hello_4"); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Promise rejected"); }));
  REQUIRE(m_builderMock.IsResolveCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodCall_StaticRejectSayHelloPromise", "NativeModuleTest") {
  m_builderMock.Call2(
      L"StaticRejectSayHelloPromise",
      std::function<void(const std::string &)>(
          [](const std::string &result) noexcept { REQUIRE(result == "Hello_4"); }),
      std::function<void(JSValue const &)>(
          [](JSValue const &error) noexcept { REQUIRE(error["message"] == "Promise rejected"); }));
  REQUIRE(m_builderMock.IsRejectCallbackCalled());
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodSyncCall_AddSync", "NativeModuleTest") {
  int result;
  m_builderMock.CallSync(L"AddSync", /*out*/ result, 3, 5);
  REQUIRE(result == 8);
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodSyncCall_NegateSync", "NativeModuleTest") {
  int result;
  m_builderMock.CallSync(L"NegateSync", /*out*/ result, 7);
  REQUIRE(result == -7);
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodSyncCall_SayHelloSync", "NativeModuleTest") {
  std::string result;
  m_builderMock.CallSync(L"SayHelloSync", /*out*/ result);
  REQUIRE(result == "Hello");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodSyncCall_StaticAddSync", "NativeModuleTest") {
  int result;
  m_builderMock.CallSync(L"StaticAddSync", /*out*/ result, 3, 5);
  REQUIRE(result == 8);
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodSyncCall_StaticNegateSync", "NativeModuleTest") {
  int result;
  m_builderMock.CallSync(L"StaticNegateSync", /*out*/ result, 7);
  REQUIRE(result == -7);
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestMethodSyncCall_StaticSayHelloSync", "NativeModuleTest") {
  std::string result;
  m_builderMock.CallSync(L"StaticSayHelloSync", /*out*/ result);
  REQUIRE(result == "Hello");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestConstants", "NativeModuleTest") {
  auto constants = m_builderMock.GetConstants();
  REQUIRE(constants["Constant1"] == "MyConstant1");
  REQUIRE(constants["const2"] == "MyConstant2");
  REQUIRE(constants["const3"]["X"] == 2);
  REQUIRE(constants["const3"]["Y"] == 3);
  REQUIRE(constants["Constant4"]["X"] == 3);
  REQUIRE(constants["Constant4"]["Y"] == 4);
  REQUIRE(constants["const51"]["X"] == 12);
  REQUIRE(constants["const51"]["Y"] == 14);
  REQUIRE(constants["const52"] == "MyConstant52");
  REQUIRE(constants["const61"]["X"] == 15);
  REQUIRE(constants["const61"]["Y"] == 17);
  REQUIRE(constants["const62"] == "MyConstant62");
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestEvent_EventField1", "NativeModuleTest") {
  bool eventRaised = false;
  m_builderMock.SetEventHandler(L"OnIntResult1", std::function<void(int)>([&eventRaised](int eventArg) noexcept {
                                  REQUIRE(eventArg == 42);
                                  eventRaised = true;
                                }));

  m_module->OnIntResult1(42);
  REQUIRE(eventRaised == true);
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestEvent_EventField2", "NativeModuleTest") {
  bool eventRaised = false;
  m_builderMock.SetEventHandler(
      L"onPointResult2", std::function<void(const Point &)>([&eventRaised](const Point &eventArg) noexcept {
        REQUIRE(eventArg.X == 4);
        REQUIRE(eventArg.Y == 2);
        eventRaised = true;
      }));

  m_module->OnPointResult2(Point{/*X =*/4, /*Y =*/2});
  REQUIRE(eventRaised == true);
}

TEST_CASE_METHOD(NativeModuleTestFixture, "TestEvent_EventField3", "NativeModuleTest") {
  bool eventRaised = false;
  m_builderMock.SetEventHandler(
      L"OnObjectResult3", std::function<void(const JSValue &)>([&eventRaised](const JSValue &eventArg) noexcept {
        REQUIRE(eventArg.Object().at("X").Int64() == 4);
        REQUIRE(eventArg.Object().at("Y").Int64() == 2);
        eventRaised = true;
      }));

  JSValue data = JSValue();
  auto writer = MakeJSValueTreeWriter(data);
  writer.WriteObjectBegin();
  WriteProperty(writer, "X", 4);
  WriteProperty(writer, "Y", 2);
  writer.WriteObjectEnd();

  m_module->OnObjectResult3(data);
  REQUIRE(eventRaised == true);
}

} // namespace winrt::Microsoft::ReactNative
