// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactModuleBuilderMock.h"

#include <sstream>
#include "Point.h"
#include "future/futureWait.h"

namespace ReactNativeTests {

REACT_MODULE(SimpleNativeModule)
struct SimpleNativeModule {
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &context) noexcept {
    IsInitialized = true;
    TestCheck(context);

    // Event and Function fields are initialized before REACT_INIT method call.
    TestCheck(this->OnIntEvent != nullptr);
    TestCheck(this->JSIntFunction != nullptr);
  }

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

  REACT_METHOD(NegateAsyncCallback)
  fire_and_forget NegateAsyncCallback(int x, std::function<void(int)> resolve) noexcept {
    co_await winrt::resume_background();
    resolve(-x);
  }

  REACT_METHOD(NegateDispatchQueueCallback)
  void NegateDispatchQueueCallback(int x, std::function<void(int)> const &resolve) noexcept {
    Mso::DispatchQueue::ConcurrentQueue().Post([x, resolve]() noexcept { resolve(-x); });
  }

  REACT_METHOD(NegateFutureCallback)
  void NegateFutureCallback(int x, std::function<void(int)> const &resolve) noexcept {
    Mso::PostFuture([x, resolve]() noexcept { resolve(-x); });
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

  REACT_METHOD(StaticNegateAsyncCallback)
  static fire_and_forget StaticNegateAsyncCallback(int x, std::function<void(int)> resolve) noexcept {
    co_await winrt::resume_background();
    resolve(-x);
  }

  REACT_METHOD(StaticNegateDispatchQueueCallback)
  static void StaticNegateDispatchQueueCallback(int x, std::function<void(int)> const &resolve) noexcept {
    Mso::DispatchQueue::ConcurrentQueue().Post([x, resolve]() noexcept { resolve(-x); });
  }

  REACT_METHOD(StaticNegateFutureCallback)
  static void StaticNegateFutureCallback(int x, std::function<void(int)> const &resolve) noexcept {
    Mso::PostFuture([x, resolve]() noexcept { resolve(-x); });
  }

  REACT_METHOD(StaticSayHelloCallback)
  static void StaticSayHelloCallback(std::function<void(const std::string &)> const &resolve) noexcept {
    resolve("Static Hello_2");
  }

  REACT_METHOD(CallbackZeroArgs)
  void CallbackZeroArgs(std::function<void()> const &resolve) noexcept {
    resolve();
  }

  REACT_METHOD(CallbackTwoArgs)
  void CallbackTwoArgs(std::function<void(int, int)> const &resolve) noexcept {
    resolve(1, 2);
  }

  REACT_METHOD(CallbackThreeArgs)
  void CallbackThreeArgs(std::function<void(int, int, std::string const &)> const &resolve) noexcept {
    resolve(1, 2, "Hello");
  }

  REACT_METHOD(DivideCallbacks)
  void DivideCallbacks(
      int x,
      int y,
      std::function<void(int)> const &resolve,
      std::function<void(std::string const &)> const &reject) noexcept {
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
      std::function<void(std::string const &)> const &reject) noexcept {
    if (x >= 0) {
      resolve(-x);
    } else {
      reject("Already negative");
    }
  }

  REACT_METHOD(NegateAsyncCallbacks)
  fire_and_forget NegateAsyncCallbacks(
      int x,
      std::function<void(int)> resolve,
      std::function<void(std::string const &)> reject) noexcept {
    co_await winrt::resume_background();
    if (x >= 0) {
      resolve(-x);
    } else {
      reject("Already negative");
    }
  }

  REACT_METHOD(NegateDispatchQueueCallbacks)
  void NegateDispatchQueueCallbacks(
      int x,
      std::function<void(int)> const &resolve,
      std::function<void(std::string const &)> const &reject) noexcept {
    Mso::DispatchQueue::ConcurrentQueue().Post([x, resolve, reject]() noexcept {
      if (x >= 0) {
        resolve(-x);
      } else {
        reject("Already negative");
      }
    });
  }

  REACT_METHOD(NegateFutureCallbacks)
  void NegateFutureCallbacks(
      int x,
      std::function<void(int)> const &resolve,
      std::function<void(std::string const &)> const &reject) noexcept {
    Mso::PostFuture([x, resolve, reject]() noexcept {
      if (x >= 0) {
        resolve(-x);
      } else {
        reject("Already negative");
      }
    });
  }

  REACT_METHOD(ResolveSayHelloCallbacks)
  void ResolveSayHelloCallbacks(
      std::function<void(std::string const &)> const &resolve,
      std::function<void(std::string const &)> const & /*reject*/) noexcept {
    resolve("Hello_3");
  }

  REACT_METHOD(RejectSayHelloCallbacks)
  void RejectSayHelloCallbacks(
      std::function<void(std::string const &)> const & /*resolve*/,
      std::function<void(std::string const &)> const &reject) noexcept {
    reject("Goodbye");
  }

  REACT_METHOD(StaticDivideCallbacks)
  static void StaticDivideCallbacks(
      int x,
      int y,
      std::function<void(int)> const &resolve,
      std::function<void(std::string const &)> const &reject) noexcept {
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
      std::function<void(std::string const &)> const &reject) noexcept {
    if (x >= 0) {
      resolve(-x);
    } else {
      reject("Already negative");
    }
  }

  REACT_METHOD(StaticNegateAsyncCallbacks)
  static fire_and_forget StaticNegateAsyncCallbacks(
      int x,
      std::function<void(int)> resolve,
      std::function<void(std::string const &)> reject) noexcept {
    co_await winrt::resume_background();
    if (x >= 0) {
      resolve(-x);
    } else {
      reject("Already negative");
    }
  }

  REACT_METHOD(StaticNegateDispatchQueueCallbacks)
  static void StaticNegateDispatchQueueCallbacks(
      int x,
      std::function<void(int)> const &resolve,
      std::function<void(std::string const &)> const &reject) noexcept {
    Mso::DispatchQueue::ConcurrentQueue().Post([x, resolve, reject]() noexcept {
      if (x >= 0) {
        resolve(-x);
      } else {
        reject("Already negative");
      }
    });
  }

  REACT_METHOD(StaticNegateFutureCallbacks)
  static void StaticNegateFutureCallbacks(
      int x,
      std::function<void(int)> const &resolve,
      std::function<void(std::string const &)> const &reject) noexcept {
    Mso::PostFuture([x, resolve, reject]() noexcept {
      if (x >= 0) {
        resolve(-x);
      } else {
        reject("Already negative");
      }
    });
  }

  REACT_METHOD(StaticResolveSayHelloCallbacks)
  static void StaticResolveSayHelloCallbacks(
      std::function<void(std::string const &)> const &resolve,
      std::function<void(std::string const &)> const & /*reject*/) noexcept {
    resolve("Hello_3");
  }

  REACT_METHOD(StaticRejectSayHelloCallbacks)
  static void StaticRejectSayHelloCallbacks(
      std::function<void(std::string const &)> const & /*resolve*/,
      std::function<void(std::string const &)> const &reject) noexcept {
    reject("Goodbye");
  }

  REACT_METHOD(TwoCallbacksZeroArgs1)
  void TwoCallbacksZeroArgs1(std::function<void()> const &resolve, std::function<void()> const & /*reject*/) noexcept {
    resolve();
  }

  REACT_METHOD(TwoCallbacksZeroArgs2)
  void TwoCallbacksZeroArgs2(std::function<void()> const & /*resolve*/, std::function<void()> const &reject) noexcept {
    reject();
  }

  REACT_METHOD(TwoCallbacksTwoArgs1)
  void TwoCallbacksTwoArgs1(
      std::function<void(int, int)> const &resolve,
      std::function<void(int, int)> const & /*reject*/) noexcept {
    resolve(1, 2);
  }

  REACT_METHOD(TwoCallbacksTwoArgs2)
  void TwoCallbacksTwoArgs2(
      std::function<void(int, int)> const & /*resolve*/,
      std::function<void(int, int)> const &reject) noexcept {
    reject(1, 2);
  }

  REACT_METHOD(TwoCallbacksThreeArgs1)
  void TwoCallbacksThreeArgs1(
      std::function<void(int, int, std::string const &)> const &resolve,
      std::function<void(int, int, std::string const &)> const & /*reject*/) noexcept {
    resolve(1, 2, "Hello");
  }

  REACT_METHOD(TwoCallbacksThreeArgs2)
  void TwoCallbacksThreeArgs2(
      std::function<void(int, int, std::string const &)> const & /*resolve*/,
      std::function<void(int, int, std::string const &)> const &reject) noexcept {
    reject(1, 2, "Hello");
  }

  REACT_METHOD(DividePromise)
  void DividePromise(int x, int y, React::ReactPromise<int> const &result) noexcept {
    if (y != 0) {
      result.Resolve(x / y);
    } else {
      React::ReactError error{};
      error.Message = "Division by 0";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(NegatePromise)
  void NegatePromise(int x, React::ReactPromise<int> const &result) noexcept {
    if (x >= 0) {
      result.Resolve(-x);
    } else {
      React::ReactError error{};
      error.Message = "Already negative";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(NegateAsyncPromise)
  fire_and_forget NegateAsyncPromise(int x, React::ReactPromise<int> result) noexcept {
    co_await winrt::resume_background();
    if (x >= 0) {
      result.Resolve(-x);
    } else {
      React::ReactError error{};
      error.Message = "Already negative";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(NegateDispatchQueuePromise)
  void NegateDispatchQueuePromise(int x, React::ReactPromise<int> const &result) noexcept {
    Mso::DispatchQueue::ConcurrentQueue().Post([x, result]() noexcept {
      if (x >= 0) {
        result.Resolve(-x);
      } else {
        React::ReactError error{};
        error.Message = "Already negative";
        result.Reject(std::move(error));
      }
    });
  }

  REACT_METHOD(NegateFuturePromise)
  void NegateFuturePromise(int x, React::ReactPromise<int> const &result) noexcept {
    Mso::PostFuture([x, result]() noexcept {
      if (x >= 0) {
        result.Resolve(-x);
      } else {
        React::ReactError error{};
        error.Message = "Already negative";
        result.Reject(std::move(error));
      }
    });
  }

  // Each macro has second optional parameter: JS name.
  REACT_METHOD(VoidPromise, L"voidPromise")
  void VoidPromise(int x, React::ReactPromise<void> const &result) noexcept {
    if (x % 2 == 0) {
      result.Resolve();
    } else {
      result.Reject("Odd unexpected");
    }
  }

  REACT_METHOD(ResolveSayHelloPromise)
  void ResolveSayHelloPromise(React::ReactPromise<std::string> const &result) noexcept {
    result.Resolve("Hello_4");
  }

  REACT_METHOD(RejectSayHelloPromise)
  void RejectSayHelloPromise(React::ReactPromise<std::string> const &result) noexcept {
    React::ReactError error{};
    error.Message = "Promise rejected";
    result.Reject(std::move(error));
  }

  REACT_METHOD(StaticDividePromise)
  static void StaticDividePromise(int x, int y, React::ReactPromise<int> const &result) noexcept {
    if (y != 0) {
      result.Resolve(x / y);
    } else {
      React::ReactError error{};
      error.Message = "Division by 0";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(StaticNegatePromise)
  static void StaticNegatePromise(int x, React::ReactPromise<int> const &result) noexcept {
    if (x >= 0) {
      result.Resolve(-x);
    } else {
      React::ReactError error{};
      error.Message = "Already negative";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(StaticNegateAsyncPromise)
  static fire_and_forget StaticNegateAsyncPromise(int x, React::ReactPromise<int> result) noexcept {
    co_await winrt::resume_background();
    if (x >= 0) {
      result.Resolve(-x);
    } else {
      React::ReactError error{};
      error.Message = "Already negative";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(StaticNegateDispatchQueuePromise)
  static void StaticNegateDispatchQueuePromise(int x, React::ReactPromise<int> const &result) noexcept {
    Mso::DispatchQueue::ConcurrentQueue().Post([x, result]() noexcept {
      if (x >= 0) {
        result.Resolve(-x);
      } else {
        React::ReactError error{};
        error.Message = "Already negative";
        result.Reject(std::move(error));
      }
    });
  }

  REACT_METHOD(StaticNegateFuturePromise)
  static void StaticNegateFuturePromise(int x, React::ReactPromise<int> const &result) noexcept {
    Mso::PostFuture([x, result]() noexcept {
      if (x >= 0) {
        result.Resolve(-x);
      } else {
        React::ReactError error{};
        error.Message = "Already negative";
        result.Reject(std::move(error));
      }
    });
  }

  // Each macro has second optional parameter: JS name.
  REACT_METHOD(StaticVoidPromise, L"staticVoidPromise")
  void StaticVoidPromise(int x, React::ReactPromise<void> const &result) noexcept {
    if (x % 2 == 0) {
      result.Resolve();
    } else {
      result.Reject("Odd unexpected");
    }
  }

  REACT_METHOD(StaticResolveSayHelloPromise)
  static void StaticResolveSayHelloPromise(React::ReactPromise<std::string> const &result) noexcept {
    result.Resolve("Hello_4");
  }

  REACT_METHOD(StaticRejectSayHelloPromise)
  static void StaticRejectSayHelloPromise(React::ReactPromise<std::string> const &result) noexcept {
    React::ReactError error{};
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
  void Constant5(React::ReactConstantProvider &provider) noexcept {
    provider.Add(L"const51", Point{/*X =*/12, /*Y =*/14});
    provider.Add(L"const52", "MyConstant52");
  }

  REACT_CONSTANT_PROVIDER(Constant6)
  static void Constant6(React::ReactConstantProvider &provider) noexcept {
    provider.Add(L"const61", Point{/*X =*/15, /*Y =*/17});
    provider.Add(L"const62", "MyConstant62");
  }

  // Allows to emit native module events
  REACT_EVENT(OnIntEvent)
  std::function<void(int)> OnIntEvent;

  // An event without arguments
  REACT_EVENT(OnNoArgEvent)
  std::function<void()> OnNoArgEvent;

  // An event with two arguments
  REACT_EVENT(OnTwoArgsEvent)
  std::function<void(Point const &, Point const &)> OnTwoArgsEvent;

  // Specify event name different from the field name.
  REACT_EVENT(OnPointEvent, L"onPointEvent")
  std::function<void(Point const &)> OnPointEvent;

  // By default we use the event emitter name from REACT_MODULE which is by default 'RCTDeviceEventEmitter'.
  // Here we specify event emitter name local for this event.
  REACT_EVENT(OnStringEvent, L"onStringEvent", L"MyEventEmitter")
  std::function<void(char const *)> OnStringEvent;

  // Use React::JSValue which is an immutable JSON-like data representation.
  REACT_EVENT(OnJSValueEvent)
  std::function<void(const React::JSValue &)> OnJSValueEvent;

  // Allows to call JS functions.
  REACT_FUNCTION(JSIntFunction)
  std::function<void(int)> JSIntFunction;

  // Specify JS function name different from the field name.
  REACT_FUNCTION(JSPointFunction, L"pointFunc")
  std::function<void(Point const &)> JSPointFunction;

  // Use two arguments. Specify JS function name different from the field name.
  REACT_FUNCTION(JSLineFunction, L"lineFunc")
  std::function<void(Point const &, Point const &)> JSLineFunction;

  // Use no arguments.
  REACT_FUNCTION(JSNoArgFunction)
  std::function<void()> JSNoArgFunction;

  // By default we use the module name from REACT_MODULE which is by default the struct name.
  // Here we specify module name local for this function.
  REACT_FUNCTION(JSStringFunction, L"stringFunc", L"MyModule")
  std::function<void(char const *)> JSStringFunction;

  // Use React::JSValue which is an immutable JSON-like data representation.
  REACT_FUNCTION(JSValueFunction)
  std::function<void(const React::JSValue &)> JSValueFunction;

 public: // Used to report some test messages
  bool IsInitialized{false};
  std::string Message;
  static std::string StaticMessage;
};

/*static*/ std::string SimpleNativeModule::StaticMessage;

TEST_CLASS (NativeModuleTest) {
  React::ReactModuleBuilderMock m_builderMock{};
  React::IReactModuleBuilder m_moduleBuilder;
  Windows::Foundation::IInspectable m_moduleObject{nullptr};
  SimpleNativeModule *m_module;

  NativeModuleTest() {
    m_moduleBuilder = winrt::make<React::ReactModuleBuilderImpl>(m_builderMock);
    auto provider = React::MakeModuleProvider<SimpleNativeModule>();
    m_moduleObject = m_builderMock.CreateModule(provider, m_moduleBuilder);
    m_module = React::ReactNonAbiValue<SimpleNativeModule>::GetPtrUnsafe(m_moduleObject);
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
    m_builderMock.Call1(
        L"StaticSayHello", std::function<void(const std::string &)>([](const std::string &result) noexcept {
          TestCheck(result == "Hello");
        }));
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

  TEST_METHOD(TestMethodCall_NegateAsyncCallback) {
    Mso::FutureWait(m_builderMock.Call1(
        L"NegateAsyncCallback", std::function<void(int)>([](int result) noexcept { TestCheck(result == -4); }), 4));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateDispatchQueueCallback) {
    Mso::FutureWait(m_builderMock.Call1(
        L"NegateDispatchQueueCallback",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -4); }),
        4));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateFutureCallback) {
    Mso::FutureWait(m_builderMock.Call1(
        L"NegateFutureCallback", std::function<void(int)>([](int result) noexcept { TestCheck(result == -4); }), 4));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_SayHelloCallback) {
    m_builderMock.Call1(
        L"SayHelloCallback", std::function<void(const std::string &)>([](const std::string &result) noexcept {
          TestCheck(result == "Hello_2");
        }));
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

  TEST_METHOD(TestMethodCall_StaticNegateAsyncCallback) {
    Mso::FutureWait(m_builderMock.Call1(
        L"StaticNegateAsyncCallback",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -4); }),
        4));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateDispatchQueueCallback) {
    Mso::FutureWait(m_builderMock.Call1(
        L"StaticNegateDispatchQueueCallback",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -4); }),
        4));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateFutureCallback) {
    Mso::FutureWait(m_builderMock.Call1(
        L"StaticNegateFutureCallback",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -4); }),
        4));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticSayHelloCallback) {
    m_builderMock.Call1(
        L"StaticSayHelloCallback", std::function<void(const std::string &)>([](const std::string &result) noexcept {
          TestCheck(result == "Static Hello_2");
        }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_CallbackZeroArgs) {
    m_builderMock.Call1(L"CallbackZeroArgs", std::function<void()>([]() noexcept {}));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_CallbackTwoArgs) {
    m_builderMock.Call1(L"CallbackTwoArgs", std::function<void(int, int)>([](int p1, int p2) noexcept {
                          TestCheckEqual(1, p1);
                          TestCheckEqual(2, p2);
                        }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_CallbackThreeArgs) {
    m_builderMock.Call1(
        L"CallbackThreeArgs",
        std::function<void(int, int, std::string const &)>([](int p1, int p2, std::string const &p3) noexcept {
          TestCheckEqual(1, p1);
          TestCheckEqual(2, p2);
          TestCheckEqual("Hello", p3);
        }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_DivideCallbacks) {
    m_builderMock.Call2(
        L"DivideCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Division by 0"); }),
        6,
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_DivideCallbacksError) {
    m_builderMock.Call2(
        L"DivideCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Division by 0"); }),
        6,
        0);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateCallbacks) {
    m_builderMock.Call2(
        L"NegateCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateCallbacksError) {
    m_builderMock.Call2(
        L"NegateCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        -5);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateAsyncCallbacks) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateAsyncCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateAsyncCallbacksError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateAsyncCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateDispatchQueueCallbacks) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateDispatchQueueCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateDispatchQueueCallbacksError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateDispatchQueueCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateFutureCallbacks) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateFutureCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateFutureCallbacksError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateFutureCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_ResolveSayHelloCallbacks) {
    m_builderMock.Call2(
        L"ResolveSayHelloCallbacks",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_3"); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Goodbye"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_RejectSayHelloCallbacks) {
    m_builderMock.Call2(
        L"RejectSayHelloCallbacks",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_3"); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Goodbye"); }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticDivideCallbacks) {
    m_builderMock.Call2(
        L"StaticDivideCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Division by 0"); }),
        6,
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticDivideCallbacksError) {
    m_builderMock.Call2(
        L"StaticDivideCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Division by 0"); }),
        6,
        0);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateCallbacks) {
    m_builderMock.Call2(
        L"StaticNegateCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateCallbacksError) {
    m_builderMock.Call2(
        L"StaticNegateCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        -5);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateAsyncCallbacks) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateAsyncCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateAsyncCallbacksError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateAsyncCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateDispatchQueueCallbacks) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateDispatchQueueCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateDispatchQueueCallbacksError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateDispatchQueueCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateFutureCallbacks) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateFutureCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateFutureCallbacksError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateFutureCallbacks",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticResolveSayHelloCallbacks) {
    m_builderMock.Call2(
        L"StaticResolveSayHelloCallbacks",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_3"); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Goodbye"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticRejectSayHelloCallbacks) {
    m_builderMock.Call2(
        L"StaticRejectSayHelloCallbacks",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_3"); }),
        std::function<void(std::string const &)>(
            [](std::string const &error) noexcept { TestCheck(error == "Goodbye"); }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_TwoCallbacksZeroArgs1) {
    m_builderMock.Call2(
        L"TwoCallbacksZeroArgs1", std::function<void()>([]() noexcept {}), std::function<void()>([]() noexcept {
          TestCheckFail();
        }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_TwoCallbacksZeroArgs2) {
    m_builderMock.Call2(
        L"TwoCallbacksZeroArgs2",
        std::function<void()>([]() noexcept { TestCheckFail(); }),
        std::function<void()>([]() noexcept {}));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_TwoCallbacksTwoArgs1) {
    m_builderMock.Call2(
        L"TwoCallbacksTwoArgs1",
        std::function<void(int, int)>([](int p1, int p2) noexcept {
          TestCheckEqual(1, p1);
          TestCheckEqual(2, p2);
        }),
        std::function<void(int, int)>([](int /*p1*/, int /*p2*/) noexcept { TestCheckFail(); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_TwoCallbacksTwoArgs2) {
    m_builderMock.Call2(
        L"TwoCallbacksTwoArgs2",
        std::function<void(int, int)>([](int /*p1*/, int /*p2*/) noexcept { TestCheckFail(); }),
        std::function<void(int, int)>([](int p1, int p2) noexcept {
          TestCheckEqual(1, p1);
          TestCheckEqual(2, p2);
        }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_TwoCallbacksThreeArgs1) {
    m_builderMock.Call2(
        L"TwoCallbacksThreeArgs1",
        std::function<void(int, int, const std::string &)>([](int p1, int p2, std::string const &p3) noexcept {
          TestCheckEqual(1, p1);
          TestCheckEqual(2, p2);
          TestCheckEqual("Hello", p3);
        }),
        std::function<void(int, int, const std::string &)>(
            [](int /*p1*/, int /*p2*/, std::string const & /*p3*/) noexcept { TestCheckFail(); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_TwoCallbacksThreeArgs2) {
    m_builderMock.Call2(
        L"TwoCallbacksThreeArgs2",
        std::function<void(int, int, const std::string &)>(
            [](int /*p1*/, int /*p2*/, std::string const & /*p3*/) noexcept { TestCheckFail(); }),
        std::function<void(int, int, const std::string &)>([](int p1, int p2, std::string const &p3) noexcept {
          TestCheckEqual(1, p1);
          TestCheckEqual(2, p2);
          TestCheckEqual("Hello", p3);
        }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_DividePromise) {
    m_builderMock.Call2(
        L"DividePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_DividePromiseError) {
    m_builderMock.Call2(
        L"DividePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        0);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegatePromise) {
    m_builderMock.Call2(
        L"NegatePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegatePromiseError) {
    m_builderMock.Call2(
        L"NegatePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateAsyncPromise) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateAsyncPromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateAsyncPromiseError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateAsyncPromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateDispatchQueuePromise) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateDispatchQueuePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateDispatchQueuePromiseError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateDispatchQueuePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateFuturePromise) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateFuturePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_NegateFuturePromiseError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"NegateFuturePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_VoidPromise) {
    m_builderMock.Call2(
        L"voidPromise",
        std::function<void()>([]() noexcept {}),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Odd unexpected"); }),
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_VoidError) {
    m_builderMock.Call2(
        L"voidPromise",
        std::function<void()>([]() noexcept {}),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Odd unexpected"); }),
        3);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_ResolveSayHelloPromise) {
    m_builderMock.Call2(
        L"ResolveSayHelloPromise",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_4"); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Promise rejected"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_RejectSayHelloPromise) {
    m_builderMock.Call2(
        L"RejectSayHelloPromise",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_4"); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Promise rejected"); }));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticDividePromise) {
    m_builderMock.Call2(
        L"StaticDividePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticDividePromiseError) {
    m_builderMock.Call2(
        L"StaticDividePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == 3); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Division by 0"); }),
        6,
        0);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegatePromise) {
    m_builderMock.Call2(
        L"StaticNegatePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegatePromiseError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateAsyncPromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateAsyncPromise) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateAsyncPromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateAsyncPromiseError) {
    m_builderMock.Call2(
        L"StaticNegatePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateDispatchQueuePromise) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateDispatchQueuePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateDispatchQueuePromiseError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateDispatchQueuePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateFuturePromise) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateFuturePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        5));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticNegateFuturePromiseError) {
    Mso::FutureWait(m_builderMock.Call2(
        L"StaticNegateFuturePromise",
        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        -5));
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticVoidPromise) {
    m_builderMock.Call2(
        L"staticVoidPromise",
        std::function<void()>([]() noexcept {}),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Odd unexpected"); }),
        2);
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticVoidPromiseError) {
    m_builderMock.Call2(
        L"staticVoidPromise",
        std::function<void()>([]() noexcept {}),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Odd unexpected"); }),
        3);
    TestCheck(m_builderMock.IsRejectCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticResolveSayHelloPromise) {
    m_builderMock.Call2(
        L"StaticResolveSayHelloPromise",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_4"); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Promise rejected"); }));
    TestCheck(m_builderMock.IsResolveCallbackCalled());
  }

  TEST_METHOD(TestMethodCall_StaticRejectSayHelloPromise) {
    m_builderMock.Call2(
        L"StaticRejectSayHelloPromise",
        std::function<void(const std::string &)>(
            [](const std::string &result) noexcept { TestCheck(result == "Hello_4"); }),
        std::function<void(React::JSValue const &)>(
            [](React::JSValue const &error) noexcept { TestCheck(error["message"] == "Promise rejected"); }));
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

  TEST_METHOD(TestEvent_IntEventField) {
    bool eventRaised = false;
    m_builderMock.ExpectEvent(
        L"RCTDeviceEventEmitter", L"OnIntEvent", [&eventRaised](React::JSValueArray const &args) noexcept {
          TestCheck(args[0] == 42);
          eventRaised = true;
        });

    m_module->OnIntEvent(42);
    TestCheck(eventRaised);
  }

  TEST_METHOD(TestEvent_OnNoArgEventField) {
    bool eventRaised = false;
    m_builderMock.ExpectEvent(
        L"RCTDeviceEventEmitter", L"OnNoArgEvent", [&eventRaised](React::JSValueArray const &args) noexcept {
          TestCheckEqual(0u, args.size());
          eventRaised = true;
        });

    m_module->OnNoArgEvent();
    TestCheck(eventRaised);
  }

  TEST_METHOD(TestEvent_TwoArgsEventField) {
    bool eventRaised = false;
    m_builderMock.ExpectEvent(
        L"RCTDeviceEventEmitter", L"OnTwoArgsEvent", [&eventRaised](React::JSValueArray const &args) noexcept {
          TestCheckEqual(4, args[0]["X"]);
          TestCheckEqual(2, args[0]["Y"]);
          TestCheckEqual(12, args[1]["X"]);
          TestCheckEqual(18, args[1]["Y"]);
          eventRaised = true;
        });

    m_module->OnTwoArgsEvent(Point{/*X =*/4, /*Y =*/2}, Point{/*X =*/12, /*Y =*/18});
    TestCheck(eventRaised);
  }

  TEST_METHOD(TestEvent_JSNameEventField) {
    bool eventRaised = false;
    m_builderMock.ExpectEvent(
        L"RCTDeviceEventEmitter", L"onPointEvent", [&eventRaised](React::JSValueArray const &args) noexcept {
          TestCheck(args[0]["X"] == 4);
          TestCheck(args[0]["Y"] == 2);
          eventRaised = true;
        });

    m_module->OnPointEvent(Point{/*X =*/4, /*Y =*/2});
    TestCheck(eventRaised == true);
  }

  TEST_METHOD(TestEvent_JSEventEmitterEventField) {
    bool eventRaised = false;
    m_builderMock.ExpectEvent(
        L"MyEventEmitter", L"onStringEvent", [&eventRaised](React::JSValueArray const &args) noexcept {
          TestCheckEqual("Hello World!", args[0]);
          eventRaised = true;
        });

    m_module->OnStringEvent("Hello World!");
    TestCheck(eventRaised == true);
  }

  TEST_METHOD(TestEvent_JSValueObjectEventField) {
    bool eventRaised = false;
    m_builderMock.ExpectEvent(
        L"RCTDeviceEventEmitter", L"OnJSValueEvent", ([&eventRaised](React::JSValueArray const &args) noexcept {
          TestCheck(args[0]["X"] == 4);
          TestCheck(args[0]["Y"] == 2);
          eventRaised = true;
        }));

    m_module->OnJSValueEvent(React::JSValueObject{{"X", 4}, {"Y", 2}});
    TestCheck(eventRaised == true);
  }

  TEST_METHOD(TestEvent_JSValueArrayEventField) {
    bool eventRaised = false;
    m_builderMock.ExpectEvent(
        L"RCTDeviceEventEmitter", L"OnJSValueEvent", ([&eventRaised](React::JSValueArray const &args) noexcept {
          TestCheck(args[0][0] == "X");
          TestCheck(args[0][1] == 4);
          TestCheck(args[0][2] == true);
          TestCheck(args[0][3]["Id"] == 42);
          eventRaised = true;
        }));

    m_module->OnJSValueEvent(React::JSValueArray{"X", 4, true, React::JSValueObject{{"Id", 42}}});
    TestCheck(eventRaised == true);
  }

  TEST_METHOD(TestEvent_JSValueArray1EventField) {
    bool eventRaised = false;
    m_builderMock.ExpectEvent(
        L"RCTDeviceEventEmitter", L"OnJSValueEvent", ([&eventRaised](React::JSValueArray const &args) noexcept {
          TestCheck(args[0][0] == 4);
          eventRaised = true;
        }));

    m_module->OnJSValueEvent(React::JSValueArray{4});
    TestCheck(eventRaised == true);
  }

  TEST_METHOD(TestFunction_JSIntFunctionField) {
    bool functionCalled = false;
    m_builderMock.ExpectFunction(
        L"SimpleNativeModule", L"JSIntFunction", [&functionCalled](React::JSValueArray const &args) noexcept {
          TestCheck(args[0] == 42);
          functionCalled = true;
        });

    m_module->JSIntFunction(42);
    TestCheck(functionCalled);
  }

  TEST_METHOD(TestFunction_JSNameFunctionField) {
    bool functionCalled = false;
    m_builderMock.ExpectFunction(
        L"SimpleNativeModule", L"pointFunc", [&functionCalled](React::JSValueArray const &args) noexcept {
          TestCheck(args[0]["X"] == 4);
          TestCheck(args[0]["Y"] == 2);
          functionCalled = true;
        });

    m_module->JSPointFunction(Point{/*X =*/4, /*Y =*/2});
    TestCheck(functionCalled == true);
  }

  TEST_METHOD(TestFunction_TwoArgFunctionField) {
    bool functionCalled = false;
    m_builderMock.ExpectFunction(
        L"SimpleNativeModule", L"lineFunc", [&functionCalled](React::JSValueArray const &args) noexcept {
          TestCheck(args[0]["X"] == 4);
          TestCheck(args[0]["Y"] == 2);
          TestCheck(args[1]["X"] == 12);
          TestCheck(args[1]["Y"] == 18);
          functionCalled = true;
        });

    m_module->JSLineFunction(Point{/*X =*/4, /*Y =*/2}, Point{/*X =*/12, /*Y =*/18});
    TestCheck(functionCalled == true);
  }

  TEST_METHOD(TestFunction_NoArgFunctionField) {
    bool functionCalled = false;
    m_builderMock.ExpectFunction(
        L"SimpleNativeModule", L"JSNoArgFunction", [&functionCalled](React::JSValueArray const &args) noexcept {
          TestCheckEqual(0u, args.size());
          functionCalled = true;
        });

    m_module->JSNoArgFunction();
    TestCheck(functionCalled);
  }

  TEST_METHOD(TestFunction_JSModuleNameFunctionField) {
    bool functionCalled = false;
    m_builderMock.ExpectFunction(
        L"MyModule", L"stringFunc", [&functionCalled](React::JSValueArray const &args) noexcept {
          TestCheck(args[0] == "Hello World!");
          functionCalled = true;
        });

    m_module->JSStringFunction("Hello World!");
    TestCheck(functionCalled == true);
  }

  TEST_METHOD(TestFunction_JSValueObjectFunctionField) {
    bool functionCalled = false;
    m_builderMock.ExpectFunction(
        L"SimpleNativeModule", L"JSValueFunction", ([&functionCalled](React::JSValueArray const &args) noexcept {
          TestCheck(args[0]["X"] == 4);
          TestCheck(args[0]["Y"] == 2);
          functionCalled = true;
        }));

    m_module->JSValueFunction(React::JSValueObject{{"X", 4}, {"Y", 2}});
    TestCheck(functionCalled == true);
  }

  TEST_METHOD(TestFunction_JSValueArrayFunctionField) {
    bool functionCalled = false;
    m_builderMock.ExpectFunction(
        L"SimpleNativeModule", L"JSValueFunction", ([&functionCalled](React::JSValueArray const &args) noexcept {
          TestCheck(args[0][0] == "X");
          TestCheck(args[0][1] == 4);
          TestCheck(args[0][2] == true);
          TestCheck(args[0][3]["Id"] == 42);
          functionCalled = true;
        }));

    m_module->JSValueFunction(React::JSValueArray{"X", 4, true, React::JSValueObject{{"Id", 42}}});
    TestCheck(functionCalled == true);
  }

  TEST_METHOD(TestInitialized) {
    TestCheck(m_module->IsInitialized);
  }
};

} // namespace ReactNativeTests
