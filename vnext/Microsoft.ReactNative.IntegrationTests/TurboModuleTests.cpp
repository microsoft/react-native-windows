// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <NativeModules.h>
#include <ReactCommon/TurboModule.h>
#include <ReactCommon/TurboModuleUtils.h>
#include <TurboModuleProvider.h> // It is RNW specific
#include <dispatchQueue/dispatchQueue.h>
#include <future/future.h>
#include <sstream>
#include <string>
#include "TestEventService.h"
#include "TestReactNativeHostHolder.h"

using namespace winrt;
using namespace facebook;
using namespace Microsoft::ReactNative;

namespace ReactNativeIntegrationTests {
// Use anonymous namespace to avoid any linking conflicts
namespace {

REACT_STRUCT(CppTurboModuleConstants)
struct CppTurboModuleConstants {
  REACT_FIELD(constantString3)
  std::string constantString3;
  REACT_FIELD(constantInt3)
  int constantInt3;
};

REACT_STRUCT(Point)
struct Point {
  REACT_FIELD(X, L"x")
  int X;
  REACT_FIELD(Y, L"y")
  int Y;
};

#define INDEX(counterBase) (__COUNTER__ - counterBase)

struct CppTurboModuleSpec : TurboModuleSpec {
  static constexpr int c1 = __COUNTER__ + 1; // +1 to start INDEX() with zero
  static constexpr auto constants = std::tuple{
      TypedConstant<CppTurboModuleConstants>{INDEX(c1)},
  };
  static constexpr int c2 = __COUNTER__ + 1;
  static constexpr auto methods = std::tuple{
      SyncMethod<int(std::string, React::JSValue) noexcept>{INDEX(c2), L"logAction"},

      Method<void(int, int, Callback<int>) noexcept>{INDEX(c2), L"add"},
      Method<void(int, Callback<int>) noexcept>{INDEX(c2), L"negate"},
      Method<void(Callback<std::string>) noexcept>{INDEX(c2), L"sayHello"},

      Method<void() noexcept>{INDEX(c2), L"sayHello0"},
      Method<void(Point) noexcept>{INDEX(c2), L"printPoint"},
      Method<void(Point, Point) noexcept>{INDEX(c2), L"printLine"},

      Method<void(int, int, Callback<int>) noexcept>{INDEX(c2), L"addCallback"},
      Method<void(int, Callback<int>) noexcept>{INDEX(c2), L"negateCallback"},
      Method<void(int, Callback<int>) noexcept>{INDEX(c2), L"negateAsyncCallback"},
      Method<void(int, Callback<int>) noexcept>{INDEX(c2), L"negateDispatchQueueCallback"},
      Method<void(int, Callback<int>) noexcept>{INDEX(c2), L"negateFutureCallback"},
      Method<void(Callback<std::string const &>) noexcept>{INDEX(c2), L"sayHelloCallback"},

      Method<void(Callback<>) noexcept>{INDEX(c2), L"callbackZeroArgs"},
      Method<void(Callback<int, int>) noexcept>{INDEX(c2), L"callbackTwoArgs"},
      Method<void(Callback<int, int, std::string const &>) noexcept>{INDEX(c2), L"callbackThreeArgs"},

      Method<void(int, int, Callback<int>, Callback<std::string const &>) noexcept>{INDEX(c2), L"divideCallbacks"},
      Method<void(int, Callback<int>, Callback<std::string const &>) noexcept>{INDEX(c2), L"negateCallbacks"},
      Method<void(int, Callback<int>, Callback<std::string const &>) noexcept>{INDEX(c2), L"negateAsyncCallbacks"},
      Method<void(int, Callback<int>, Callback<std::string const &>) noexcept>{
          INDEX(c2),
          L"negateDispatchQueueCallbacks"},
      Method<void(int, Callback<int>, Callback<std::string const &>) noexcept>{INDEX(c2), L"negateFutureCallbacks"},
      Method<void(Callback<std::string const &>, Callback<std::string const &>) noexcept>{
          INDEX(c2),
          L"resolveSayHelloCallbacks"},
      Method<void(Callback<std::string const &>, Callback<std::string const &>) noexcept>{
          INDEX(c2),
          L"rejectSayHelloCallbacks"},

      Method<void(bool, Callback<>, Callback<>) noexcept>{INDEX(c2), L"twoCallbacksZeroArgs"},
      Method<void(bool, Callback<int, int>, Callback<int, int>) noexcept>{INDEX(c2), L"twoCallbacksTwoArgs"},
      Method<void(bool, Callback<int, int, std::string const &>, Callback<int, int, std::string const &>) noexcept>{
          INDEX(c2),
          L"twoCallbacksThreeArgs"},

      Method<void(int, int, ReactPromise<int>) noexcept>{INDEX(c2), L"dividePromise"},
      Method<void(int, ReactPromise<int>) noexcept>{INDEX(c2), L"negatePromise"},
      Method<void(int, ReactPromise<int>) noexcept>{INDEX(c2), L"negateAsyncPromise"},
      Method<void(int, ReactPromise<int>) noexcept>{INDEX(c2), L"negateDispatchQueuePromise"},
      Method<void(int, ReactPromise<int>) noexcept>{INDEX(c2), L"negateFuturePromise"},
      Method<void(int, ReactPromise<void>) noexcept>{INDEX(c2), L"voidPromise"},
      Method<void(ReactPromise<std::string>) noexcept>{INDEX(c2), L"resolveSayHelloPromise"},
      Method<void(ReactPromise<std::string>) noexcept>{INDEX(c2), L"rejectSayHelloPromise"},

      SyncMethod<int(int, int) noexcept>{INDEX(c2), L"addSync"},
      SyncMethod<int(int) noexcept>{INDEX(c2), L"negateSync"},
      SyncMethod<std::string() noexcept>{INDEX(c2), L"sayHelloSync"},

      Method<void(int, Callback<int>) noexcept>{INDEX(c2), L"negateDeferredCallback"},
      Method<void(int, Callback<int>, Callback<std::string const &>) noexcept>{
          INDEX(c2),
          L"negateDeferredTwoCallbacks"},
      Method<void(int, ReactPromise<int>) noexcept>{INDEX(c2), L"negateDeferredPromise"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    static constexpr int c3 = __COUNTER__ + 1; // +1 to start INDEX() with zero
    constexpr auto methodCheckResults = CheckMethods<TModule, CppTurboModuleSpec>();
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "logAction", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "add", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negate", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "sayHello", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "sayHello0", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "printPoint", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "printLine", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "addCallback", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateCallback", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateAsyncCallback", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateDispatchQueueCallback", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateFutureCallback", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "sayHelloCallback", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "callbackZeroArgs", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "callbackTwoArgs", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "callbackThreeArgs", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "divideCallbacks", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateCallbacks", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateAsyncCallbacks", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateDispatchQueueCallbacks", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateFutureCallbacks", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "resolveSayHelloCallbacks", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "rejectSayHelloCallbacks", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "twoCallbacksZeroArgs", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "twoCallbacksTwoArgs", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "twoCallbacksThreeArgs", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "dividePromise", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negatePromise", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateAsyncPromise", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateDispatchQueuePromise", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateFuturePromise", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "voidPromise", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "resolveSayHelloPromise", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "rejectSayHelloPromise", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "addSync", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateSync", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "sayHelloSync", "Generated error message with signatures");

    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateDeferredCallback", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateDeferredTwoCallbacks", "Generated error message with signatures");
    REACT_SHOW_METHOD_SPEC_ERRORS(INDEX(c3), "negateDeferredPromise", "Generated error message with signatures");
  }
};

REACT_MODULE_NOREG(CppTurboModule)
struct CppTurboModule {
  using ModuleSpec = CppTurboModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(ReactContext const &reactContext) noexcept {
    m_reactContext = reactContext;
  }

  REACT_SYNC_METHOD(GetTestName, L"getTestName")
  hstring GetTestName() noexcept {
    return m_reactContext.Properties().Get(CppTurboModule::TestName).value();
  }

  REACT_CONSTANT(ConstantString, L"constantString")
  const std::string ConstantString{"myConstantString"};

  REACT_CONSTANT(ConstantInt, L"constantInt")
  const int ConstantInt{3};

  REACT_CONSTANT_PROVIDER(GetConstants)
  void GetConstants(React::ReactConstantProvider &provider) noexcept {
    provider.Add(L"constantString2", L"Hello");
    provider.Add(L"constantInt2", 10);
  }

  REACT_GET_CONSTANTS(GetConstants2)
  CppTurboModuleConstants GetConstants2() noexcept {
    CppTurboModuleConstants x;
    x.constantString3 = "strong-typed-constants!";
    x.constantInt3 = 20;
    return x;
  }

  REACT_SYNC_METHOD(LogAction, L"logAction")
  int LogAction(std::string actionName, React::JSValue value) noexcept {
    TestEventService::LogEvent(actionName, std::move(value));
    return 0;
  }

  REACT_METHOD(Add, L"add")
  int Add(int x, int y) noexcept {
    return x + y;
  }

  REACT_METHOD(Negate, L"negate")
  int Negate(int x) noexcept {
    return -x;
  }

  REACT_METHOD(SayHello, L"sayHello")
  std::string SayHello() noexcept {
    return "Hello";
  }

  REACT_METHOD(SayHello0, L"sayHello0")
  void SayHello0() noexcept {
    TestEventService::LogEvent("sayHello0", "Hello");
  }

  REACT_METHOD(PrintPoint, L"printPoint")
  void PrintPoint(Point pt) noexcept {
    std::stringstream ss;
    ss << "Point: (" << pt.X << ", " << pt.Y << ")";
    TestEventService::LogEvent("printPoint", ss.str());
  }

  REACT_METHOD(PrintLine, L"printLine")
  void PrintLine(Point start, Point end) noexcept {
    std::stringstream ss;
    ss << "Line: (" << start.X << ", " << start.Y << ")-(" << end.X << ", " << end.Y << ")";
    TestEventService::LogEvent("printLine", ss.str());
  }

  REACT_METHOD(AddCallback, L"addCallback")
  void AddCallback(int x, int y, std::function<void(int)> const &resolve) noexcept {
    resolve(x + y);
  }

  REACT_METHOD(NegateCallback, L"negateCallback")
  void NegateCallback(int x, std::function<void(int)> const &resolve) noexcept {
    resolve(-x);
  }

  REACT_METHOD(NegateAsyncCallback, L"negateAsyncCallback")
  fire_and_forget NegateAsyncCallback(int x, std::function<void(int)> resolve) noexcept {
    co_await winrt::resume_background();
    resolve(-x);
  }

  REACT_METHOD(NegateDispatchQueueCallback, L"negateDispatchQueueCallback")
  void NegateDispatchQueueCallback(int x, std::function<void(int)> const &resolve) noexcept {
    Mso::DispatchQueue::ConcurrentQueue().Post([x, resolve]() noexcept { resolve(-x); });
  }

  REACT_METHOD(NegateFutureCallback, L"negateFutureCallback")
  void NegateFutureCallback(int x, std::function<void(int)> const &resolve) noexcept {
    Mso::PostFuture([x, resolve]() noexcept { resolve(-x); });
  }

  REACT_METHOD(SayHelloCallback, L"sayHelloCallback")
  void SayHelloCallback(std::function<void(const std::string &)> const &resolve) noexcept {
    resolve("Hello_2");
  }

  REACT_METHOD(CallbackZeroArgs, L"callbackZeroArgs")
  void CallbackZeroArgs(std::function<void()> const &resolve) noexcept {
    resolve();
  }

  REACT_METHOD(CallbackTwoArgs, L"callbackTwoArgs")
  void CallbackTwoArgs(std::function<void(int, int)> const &resolve) noexcept {
    resolve(1, 2);
  }

  REACT_METHOD(CallbackThreeArgs, L"callbackThreeArgs")
  void CallbackThreeArgs(std::function<void(int, int, std::string const &)> const &resolve) noexcept {
    resolve(1, 2, "Hello");
  }

  REACT_METHOD(DivideCallbacks, L"divideCallbacks")
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

  REACT_METHOD(NegateCallbacks, L"negateCallbacks")
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

  REACT_METHOD(NegateAsyncCallbacks, L"negateAsyncCallbacks")
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

  REACT_METHOD(NegateDispatchQueueCallbacks, L"negateDispatchQueueCallbacks")
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

  REACT_METHOD(NegateFutureCallbacks, L"negateFutureCallbacks")
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

  REACT_METHOD(ResolveSayHelloCallbacks, L"resolveSayHelloCallbacks")
  void ResolveSayHelloCallbacks(
      std::function<void(std::string const &)> const &resolve,
      std::function<void(std::string const &)> const & /*reject*/) noexcept {
    resolve("Hello_3");
  }

  REACT_METHOD(RejectSayHelloCallbacks, L"rejectSayHelloCallbacks")
  void RejectSayHelloCallbacks(
      std::function<void(std::string const &)> const & /*resolve*/,
      std::function<void(std::string const &)> const &reject) noexcept {
    reject("Goodbye");
  }

  REACT_METHOD(TwoCallbacksZeroArgs, L"twoCallbacksZeroArgs")
  void TwoCallbacksZeroArgs(
      bool useFirst,
      std::function<void()> const &callback1,
      std::function<void()> const &callback2) noexcept {
    if (useFirst) {
      callback1();
    } else {
      callback2();
    }
  }

  REACT_METHOD(TwoCallbacksTwoArgs, L"twoCallbacksTwoArgs")
  void TwoCallbacksTwoArgs(
      bool useFirst,
      std::function<void(int, int)> const &callback1,
      std::function<void(int, int)> const &callback2) noexcept {
    if (useFirst) {
      callback1(1, 2);
    } else {
      callback2(3, 4);
    }
  }

  REACT_METHOD(TwoCallbacksThreeArgs, L"twoCallbacksThreeArgs")
  void TwoCallbacksThreeArgs(
      bool useFirst,
      std::function<void(int, int, std::string const &)> const &callback1,
      std::function<void(int, int, std::string const &)> const &callback2) noexcept {
    if (useFirst) {
      callback1(1, 2, "Hello");
    } else {
      callback2(3, 4, "World");
    }
  }

  REACT_METHOD(DividePromise, L"dividePromise")
  void DividePromise(int x, int y, React::ReactPromise<int> const &result) noexcept {
    if (y != 0) {
      result.Resolve(x / y);
    } else {
      result.Reject("Division by 0");
    }
  }

  REACT_METHOD(NegatePromise, L"negatePromise")
  void NegatePromise(int x, React::ReactPromise<int> const &result) noexcept {
    if (x >= 0) {
      result.Resolve(-x);
    } else {
      React::ReactError error{};
      error.Message = "Already negative";
      result.Reject(std::move(error));
    }
  }

  REACT_METHOD(NegateAsyncPromise, L"negateAsyncPromise")
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

  REACT_METHOD(NegateDispatchQueuePromise, L"negateDispatchQueuePromise")
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

  REACT_METHOD(NegateFuturePromise, L"negateFuturePromise")
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

  REACT_METHOD(VoidPromise, L"voidPromise")
  void VoidPromise(int x, React::ReactPromise<void> const &result) noexcept {
    if (x % 2 == 0) {
      result.Resolve();
    } else {
      result.Reject("Odd unexpected");
    }
  }

  REACT_METHOD(ResolveSayHelloPromise, L"resolveSayHelloPromise")
  void ResolveSayHelloPromise(React::ReactPromise<std::string> const &result) noexcept {
    result.Resolve("Hello_4");
  }

  REACT_METHOD(RejectSayHelloPromise, L"rejectSayHelloPromise")
  void RejectSayHelloPromise(React::ReactPromise<std::string> const &result) noexcept {
    React::ReactError error{};
    error.Message = "Promise rejected";
    result.Reject(std::move(error));
  }

  REACT_SYNC_METHOD(AddSync, L"addSync")
  int AddSync(int x, int y) noexcept {
    return x + y;
  }

  REACT_SYNC_METHOD(NegateSync, L"negateSync")
  int NegateSync(int x) noexcept {
    return -x;
  }

  REACT_SYNC_METHOD(SayHelloSync, L"sayHelloSync")
  std::string SayHelloSync() noexcept {
    return "Hello";
  }

  REACT_METHOD(NegateDeferredCallback, L"negateDeferredCallback")
  void NegateDeferredCallback(int x, std::function<void(int)> const &resolve) noexcept {
    TestNotificationService::Set("NegateDeferredCallback called");
    auto dispatcher = ReactDispatcher(m_reactContext.Properties().Get(TestDispatcher));
    dispatcher.Post([x, resolve]() noexcept {
      TestNotificationService::Set("NegateDeferredCallback call started");
      resolve(-x);
      TestNotificationService::Set("NegateDeferredCallback call ended");
    });
  }

  REACT_METHOD(NegateDeferredTwoCallbacks, L"negateDeferredTwoCallbacks")
  void NegateDeferredTwoCallbacks(
      int x,
      std::function<void(int)> const &resolve,
      std::function<void(std::string const &)> const &reject) noexcept {
    TestNotificationService::Set("NegateDeferredTwoCallbacks called");
    auto dispatcher = ReactDispatcher(m_reactContext.Properties().Get(TestDispatcher));
    dispatcher.Post([x, resolve, reject]() noexcept {
      if (x >= 0) {
        TestNotificationService::Set("NegateDeferredTwoCallbacks resolve started");
        resolve(-x);
      } else {
        TestNotificationService::Set("NegateDeferredTwoCallbacks reject started");
        reject("Already negative");
      }
      TestNotificationService::Set("NegateDeferredTwoCallbacks call ended");
    });
  }

  REACT_METHOD(NegateDeferredPromise, L"negateDeferredPromise")
  void NegateDeferredPromise(int x, React::ReactPromise<int> const &result) noexcept {
    TestNotificationService::Set("NegateDeferredPromise called");
    auto dispatcher = ReactDispatcher(m_reactContext.Properties().Get(TestDispatcher));
    dispatcher.Post([x, result]() noexcept {
      if (x >= 0) {
        TestNotificationService::Set("Promise resolve started");
        result.Resolve(-x);
      } else {
        TestNotificationService::Set("Promise reject started");
        React::ReactError error{};
        error.Message = "Already negative";
        result.Reject(std::move(error));
      }
      TestNotificationService::Set("Promise call ended");
    });
  }

 public:
  static inline ReactPropertyId<hstring> TestName{L"TurboModuleTests", L"TestName"};
  static inline ReactPropertyId<IReactDispatcher> TestDispatcher{L"TurboModuleTests", L"TestDispatcher"};

 private:
  ReactContext m_reactContext;
};

struct CppTurboModulePackageProvider : winrt::implements<CppTurboModulePackageProvider, IReactPackageProvider> {
  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept {
    packageBuilder.AddTurboModule(L"CppTurboModule", MakeTurboModuleProvider<CppTurboModule>());
  }
};

} // namespace

TEST_CLASS (TurboModuleTests) {
  TEST_METHOD(EnsureReactModuleNotAutoRegistered) {
    bool registered = false;
    auto current = ModuleRegistration::Head();
    while (current) {
      if (wcscmp(current->ModuleName(), L"CppTurboModule") == 0) {
        registered = true;
      }
      current = current->Next();
    }
    TestCheck(!registered);
  }

  TEST_METHOD(ExecuteSampleTurboModule) {
    TestEventService::Initialize();

    auto reactNativeHost = TestReactNativeHostHolder(L"TurboModuleTests", [](ReactNativeHost const &host) noexcept {
      host.PackageProviders().Append(winrt::make<CppTurboModulePackageProvider>());
      ReactPropertyBag(host.InstanceSettings().Properties()).Set(CppTurboModule::TestName, L"ExecuteSampleTurboModule");
    });

    TestEventService::ObserveEvents({
        TestEvent{"constantString", "myConstantString"},
        TestEvent{"constantInt", 3},
        TestEvent{"constantString2", "Hello"},
        TestEvent{"constantInt2", 10},
        TestEvent{"constantString3", "strong-typed-constants!"},
        TestEvent{"constantInt3", 20},

        TestEvent{"add", 10},
        TestEvent{"negate", -10},
        TestEvent{"sayHello", "Hello"},

        TestEvent{"sayHello0", "Hello"},
        TestEvent{"printPoint", "Point: (1, 2)"},
        TestEvent{"printLine", "Line: (1, 2)-(3, 4)"},

        TestEvent{"addCallback", 15},
        TestEvent{"negateCallback", -15},
        TestEvent{"negateAsyncCallback", -16},
        TestEvent{"negateDispatchQueueCallback", -17},
        TestEvent{"negateFutureCallback", -18},
        TestEvent{"sayHelloCallback", "Hello_2"},

        TestEvent{"callbackZeroArgs", nullptr},
        TestEvent{"callbackTwoArgs", JSValueObject{{"x", 1}, {"y", 2}}},
        TestEvent{"callbackThreeArgs", JSValueObject{{"x", 1}, {"y", 2}, {"msg", "Hello"}}},

        TestEvent{"divideCallbacks", 2},
        TestEvent{"divideCallbacks.error", "Division by 0"},
        TestEvent{"negateCallbacks", -10},
        TestEvent{"negateCallbacks.error", "Already negative"},
        TestEvent{"negateAsyncCallbacks", -10},
        TestEvent{"negateAsyncCallbacks.error", "Already negative"},
        TestEvent{"negateDispatchQueueCallbacks", -10},
        TestEvent{"negateDispatchQueueCallbacks.error", "Already negative"},
        TestEvent{"negateFutureCallbacks", -10},
        TestEvent{"negateFutureCallbacks.error", "Already negative"},
        TestEvent{"resolveSayHelloCallbacks", "Hello_3"},
        TestEvent{"rejectSayHelloCallbacks.error", "Goodbye"},

        TestEvent{"twoCallbacksZeroArgs1", "success"},
        TestEvent{"twoCallbacksZeroArgs2", "failure"},
        TestEvent{"twoCallbacksTwoArgs1", JSValueObject{{"x", 1}, {"y", 2}}},
        TestEvent{"twoCallbacksTwoArgs2", JSValueObject{{"x", 3}, {"y", 4}}},
        TestEvent{"twoCallbacksThreeArgs1", JSValueObject{{"x", 1}, {"y", 2}, {"msg", "Hello"}}},
        TestEvent{"twoCallbacksThreeArgs2", JSValueObject{{"x", 3}, {"y", 4}, {"msg", "World"}}},

        TestEvent{"dividePromise", 5},
        TestEvent{"dividePromise.error", "Division by 0"},
        TestEvent{"negatePromise", -10},
        TestEvent{"negatePromise.error", "Already negative"},
        TestEvent{"negateAsyncPromise", -10},
        TestEvent{"negateAsyncPromise.error", "Already negative"},
        TestEvent{"negateDispatchQueuePromise", -10},
        TestEvent{"negateDispatchQueuePromise.error", "Already negative"},
        TestEvent{"negateFuturePromise", -10},
        TestEvent{"negateFuturePromise.error", "Already negative"},
        TestEvent{"voidPromise", "success"},
        TestEvent{"voidPromise.error", "Odd unexpected"},
        TestEvent{"resolveSayHelloPromise", "Hello_4"},
        TestEvent{"rejectSayHelloPromise", "Promise rejected"},

        TestEvent{"addSync", 42},
        TestEvent{"negateSync", -12},
        TestEvent{"sayHelloSync", "Hello"},
    });
  }

  TEST_METHOD(JSDispatcherAfterInstanceUnload) {
    TestEventService::Initialize();
    TestNotificationService::Initialize();

    CallInvoker callInvoker{nullptr};

    auto reactNativeHost = TestReactNativeHostHolder(L"TurboModuleTests", [&](ReactNativeHost const &host) noexcept {
      host.PackageProviders().Append(winrt::make<CppTurboModulePackageProvider>());
      ReactPropertyBag(host.InstanceSettings().Properties())
          .Set(CppTurboModule::TestName, L"JSDispatcherAfterInstanceUnload");
      host.InstanceSettings().InstanceDestroyed(
          [&](IInspectable const & /*sender*/, InstanceDestroyedEventArgs const & /*args*/) {
            TestNotificationService::Set("Instance destroyed event");
          });
      host.InstanceSettings().InstanceCreated(
          [&](IInspectable const & /*sender*/, InstanceCreatedEventArgs const &args) {
            callInvoker = args.Context().CallInvoker();
          });
    });

    TestEventService::ObserveEvents({
        TestEvent{"addSync", 42},
    });

    reactNativeHost.Host().UnloadInstance();
    TestNotificationService::Wait("Instance destroyed event");

    struct CallbackData {
      ~CallbackData() {
        TestNotificationService::Set("CallbackData destroyed");
      }
    };
    bool callbackIsCalled{false};

    // callInvoker must not process any callbacks
    callInvoker.InvokeAsync(
        [&callbackIsCalled, data = std::make_shared<CallbackData>()](
            const winrt::Windows::Foundation::IInspectable & /*runtimeHandle*/) { callbackIsCalled = true; });
    TestNotificationService::Wait("CallbackData destroyed");
    TestCheck(!callbackIsCalled);
  }

  TEST_METHOD(DeferCallbackAfterInstanceUnload) {
    TestNotificationService::Initialize();

    auto testDispatcher = ReactDispatcher(ReactDispatcherHelper::CreateSerialDispatcher());
    testDispatcher.Post([] { TestNotificationService::Wait("Resume Dispatcher"); });

    auto reactNativeHost =
        TestReactNativeHostHolder(L"TurboModuleTests", [testDispatcher](ReactNativeHost const &host) noexcept {
          host.PackageProviders().Append(winrt::make<CppTurboModulePackageProvider>());
          auto properties = ReactPropertyBag(host.InstanceSettings().Properties());
          properties.Set(CppTurboModule::TestName, L"DeferCallbackAfterInstanceUnload");
          properties.Set(CppTurboModule::TestDispatcher, testDispatcher.Handle());
          host.InstanceSettings().InstanceDestroyed(
              [&](IInspectable const & /*sender*/, InstanceDestroyedEventArgs const & /*args*/) {
                TestNotificationService::Set("Instance destroyed event");
              });
        });

    TestNotificationService::Wait("NegateDeferredCallback called");

    reactNativeHost.Host().UnloadInstance();
    TestNotificationService::Wait("Instance destroyed event");

    // No crash must happen when the Promise is resolved after RN instance unload.
    TestNotificationService::Set("Resume Dispatcher");
    TestNotificationService::Wait("NegateDeferredCallback call started");
    TestNotificationService::Wait("NegateDeferredCallback call ended");
  }

  TEST_METHOD(DeferResolveCallbackAfterInstanceUnload) {
    TestNotificationService::Initialize();

    auto testDispatcher = ReactDispatcher(ReactDispatcherHelper::CreateSerialDispatcher());
    testDispatcher.Post([] { TestNotificationService::Wait("Resume Dispatcher"); });

    auto reactNativeHost =
        TestReactNativeHostHolder(L"TurboModuleTests", [testDispatcher](ReactNativeHost const &host) noexcept {
          host.PackageProviders().Append(winrt::make<CppTurboModulePackageProvider>());
          auto properties = ReactPropertyBag(host.InstanceSettings().Properties());
          properties.Set(CppTurboModule::TestName, L"DeferResolveCallbackAfterInstanceUnload");
          properties.Set(CppTurboModule::TestDispatcher, testDispatcher.Handle());
          host.InstanceSettings().InstanceDestroyed(
              [&](IInspectable const & /*sender*/, InstanceDestroyedEventArgs const & /*args*/) {
                TestNotificationService::Set("Instance destroyed event");
              });
        });

    TestNotificationService::Wait("NegateDeferredTwoCallbacks called");

    reactNativeHost.Host().UnloadInstance();
    TestNotificationService::Wait("Instance destroyed event");

    // No crash must happen when the Promise is resolved after RN instance unload.
    TestNotificationService::Set("Resume Dispatcher");
    TestNotificationService::Wait("NegateDeferredTwoCallbacks resolve started");
    TestNotificationService::Wait("NegateDeferredTwoCallbacks call ended");
  }

  TEST_METHOD(DeferRejectCallbackAfterInstanceUnload) {
    TestNotificationService::Initialize();

    auto testDispatcher = ReactDispatcher(ReactDispatcherHelper::CreateSerialDispatcher());
    testDispatcher.Post([] { TestNotificationService::Wait("Resume Dispatcher"); });

    auto reactNativeHost =
        TestReactNativeHostHolder(L"TurboModuleTests", [testDispatcher](ReactNativeHost const &host) noexcept {
          host.PackageProviders().Append(winrt::make<CppTurboModulePackageProvider>());
          auto properties = ReactPropertyBag(host.InstanceSettings().Properties());
          properties.Set(CppTurboModule::TestName, L"DeferRejectCallbackAfterInstanceUnload");
          properties.Set(CppTurboModule::TestDispatcher, testDispatcher.Handle());
          host.InstanceSettings().InstanceDestroyed(
              [&](IInspectable const & /*sender*/, InstanceDestroyedEventArgs const & /*args*/) {
                TestNotificationService::Set("Instance destroyed event");
              });
        });

    TestNotificationService::Wait("NegateDeferredTwoCallbacks called");

    reactNativeHost.Host().UnloadInstance();
    TestNotificationService::Wait("Instance destroyed event");

    // No crash must happen when the Promise is resolved after RN instance unload.
    TestNotificationService::Set("Resume Dispatcher");
    TestNotificationService::Wait("NegateDeferredTwoCallbacks reject started");
    TestNotificationService::Wait("NegateDeferredTwoCallbacks call ended");
  }

  TEST_METHOD(DeferPromiseResolveAfterInstanceUnload) {
    TestNotificationService::Initialize();

    auto testDispatcher = ReactDispatcher(ReactDispatcherHelper::CreateSerialDispatcher());
    testDispatcher.Post([] { TestNotificationService::Wait("Resume Dispatcher"); });

    auto reactNativeHost =
        TestReactNativeHostHolder(L"TurboModuleTests", [testDispatcher](ReactNativeHost const &host) noexcept {
          host.PackageProviders().Append(winrt::make<CppTurboModulePackageProvider>());
          auto properties = ReactPropertyBag(host.InstanceSettings().Properties());
          properties.Set(CppTurboModule::TestName, L"DeferPromiseResolveAfterInstanceUnload");
          properties.Set(CppTurboModule::TestDispatcher, testDispatcher.Handle());
          host.InstanceSettings().InstanceDestroyed(
              [&](IInspectable const & /*sender*/, InstanceDestroyedEventArgs const & /*args*/) {
                TestNotificationService::Set("Instance destroyed event");
              });
        });

    TestNotificationService::Wait("NegateDeferredPromise called");

    reactNativeHost.Host().UnloadInstance();
    TestNotificationService::Wait("Instance destroyed event");

    // No crash must happen when the Promise is resolved after RN instance unload.
    TestNotificationService::Set("Resume Dispatcher");
    TestNotificationService::Wait("Promise resolve started");
    TestNotificationService::Wait("Promise call ended");
  }

  TEST_METHOD(DeferPromiseRejectAfterInstanceUnload) {
    TestNotificationService::Initialize();

    auto testDispatcher = ReactDispatcher(ReactDispatcherHelper::CreateSerialDispatcher());
    testDispatcher.Post([] { TestNotificationService::Wait("Resume Dispatcher"); });

    auto reactNativeHost =
        TestReactNativeHostHolder(L"TurboModuleTests", [testDispatcher](ReactNativeHost const &host) noexcept {
          host.PackageProviders().Append(winrt::make<CppTurboModulePackageProvider>());
          auto properties = ReactPropertyBag(host.InstanceSettings().Properties());
          properties.Set(CppTurboModule::TestName, L"DeferPromiseRejectAfterInstanceUnload");
          properties.Set(CppTurboModule::TestDispatcher, testDispatcher.Handle());
          host.InstanceSettings().InstanceDestroyed(
              [&](IInspectable const & /*sender*/, InstanceDestroyedEventArgs const & /*args*/) {
                TestNotificationService::Set("Instance destroyed event");
              });
        });

    TestNotificationService::Wait("NegateDeferredPromise called");

    reactNativeHost.Host().UnloadInstance();
    TestNotificationService::Wait("Instance destroyed event");

    // No crash must happen when the Promise is resolved after RN instance unload.
    TestNotificationService::Set("Resume Dispatcher");
    TestNotificationService::Wait("Promise reject started");
    TestNotificationService::Wait("Promise call ended");
  }
};

} // namespace ReactNativeIntegrationTests
