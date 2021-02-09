// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

#include "DebugHelpers.h"
#include "NativeModules.h"

#define DEBUG_OUTPUT(...) DebugWriteLine("SampleLibraryCpp", ##__VA_ARGS__);

namespace SampleLibraryCpp {

// Point struct can be automatically serialized/deserialized thanks to the custom attributes.
// Alternatively, we can provide WriteValue and ReadValue functions.
// See Microsoft.ReactNaitve.Cxx.UnitTests for examples.
REACT_STRUCT(Point)
struct Point {
  REACT_FIELD(X, L"x")
  int X;

  REACT_FIELD(Y, L"y")
  int Y;
};

// Sample REACT_MODULE
// The important notes about the module class:
// - The module class must have the REACT_MODULE attribute.
// - All exported methods with custom attributes must be public and noexcept.

REACT_MODULE(SampleModuleCppImpl, L"SampleModuleCpp");
struct SampleModuleCppImpl {
#pragma region Initialization

  REACT_INIT(Initialize)
  void Initialize(ReactContext const &reactContext) noexcept {
    m_reactContext = reactContext;

    const ReactPropertyId<int> myProp1{L"Prop1"};
    const ReactPropertyId<winrt::hstring> myProp2{L"Prop2"};
    DEBUG_OUTPUT("C++ Properties.Prop1:", *reactContext.Properties().Get(myProp1));
    DEBUG_OUTPUT("C++ Properties.Prop2:", winrt::to_string(*reactContext.Properties().Get(myProp2)));

    const ReactNotificationId<int> cppTimerNotification{L"SampleModuleCppImpl", L"TimerNotification"};
    const ReactNotificationId<int> csTimerNotification{L"SampleModuleCS", L"TimerNotification"};

    // Note that all notification subscriptions are removed automatically when React instance is unloaded.
    reactContext.Notifications().Subscribe(
        csTimerNotification,
        [](winrt::Windows::Foundation::IInspectable const &, ReactNotificationArgs<int> const &args) noexcept {
          DEBUG_OUTPUT("C++ module, C# timer:", *args.Data());
        });

    m_timer = winrt::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
        [this, cppTimerNotification, notifications = reactContext.Notifications()](
            const winrt::Windows::System::Threading::ThreadPoolTimer) noexcept {
          notifications.SendNotification(cppTimerNotification, m_timerCount);
          TimedEvent(++m_timerCount);
          if (m_timer && m_timerCount == 5) {
            m_timer.Cancel();
          }
        },
        TimedEventInterval);
  }

  ~SampleModuleCppImpl() {
    if (m_timer) {
      m_timer.Cancel();
    }
  }

#pragma endregion

#pragma region Constants

  REACT_CONSTANT(NumberConstant);
  const double NumberConstant = M_PI;

  REACT_CONSTANT(StringConstant);
  const std::string StringConstant = "Hello World";

  REACT_CONSTANT_PROVIDER(ConstantsViaConstantsProvider)
  void ConstantsViaConstantsProvider(ReactConstantProvider &constants) noexcept {
    constants.Add(L"NumberConstantViaProvider", M_PI);
    constants.Add(L"StringConstantViaProvider", "Hello World");
  }

#pragma endregion

#pragma region Methods

  REACT_METHOD(VoidMethod);
  void VoidMethod() noexcept {
    DEBUG_OUTPUT("VoidMethod");
  }

  REACT_METHOD(VoidMethodWithArgs);
  void VoidMethodWithArgs(double arg) noexcept {
    DEBUG_OUTPUT("VoidMethodWithArgs", arg);
  }

  REACT_METHOD(ReturnMethod);
  double ReturnMethod() noexcept {
    DEBUG_OUTPUT("ReturnMethod");
    return M_PI;
  }

  REACT_METHOD(ReturnMethodWithArgs);
  double ReturnMethodWithArgs(double arg) noexcept {
    DEBUG_OUTPUT("ReturnMethodWithArgs", arg);
    return M_PI;
  }

#pragma endregion

#pragma region Methods with ReactCallbacks

  REACT_METHOD(ExplicitCallbackMethod);
  void ExplicitCallbackMethod(std::function<void(double)> &&callback) noexcept {
    DEBUG_OUTPUT("ExplicitCallbackMethod");
    callback(M_PI);
  }

  REACT_METHOD(ExplicitCallbackMethodWithArgs);
  void ExplicitCallbackMethodWithArgs(double arg, std::function<void(double)> &&callback) noexcept {
    DEBUG_OUTPUT("ExplicitCallbackMethodWithArgs", arg);
    callback(M_PI);
  }

  // Use callback order as with ReactPromise
  REACT_METHOD(TwoCallbacksMethod);
  void TwoCallbacksMethod(
      bool shouldSucceed,
      std::function<void(std::string)> &&onSuccess,
      std::function<void(std::string)> &&onFailure) noexcept {
    DEBUG_OUTPUT("TwoCallbacksMethod", shouldSucceed);
    if (shouldSucceed) {
      onSuccess("TwoCallbacksMethod succeeded");
    } else {
      onFailure("TwoCallbacksMethod failed");
    }
  }

  // Use callback order as with ReactPromise
  REACT_METHOD(TwoCallbacksAsyncMethod);
  winrt::fire_and_forget TwoCallbacksAsyncMethod(
      bool shouldSucceed,
      std::function<void(std::string)> onSuccess,
      std::function<void(std::string)> onFailure) noexcept {
    DEBUG_OUTPUT("TwoCallbacksAsyncMethod", shouldSucceed);
    co_await winrt::resume_background();
    if (shouldSucceed) {
      onSuccess("TwoCallbacksMethod succeeded");
    } else {
      onFailure("TwoCallbacksMethod failed");
    }
  }

  // Use callback order as in "classic" ReactNative.
  REACT_METHOD(ReverseTwoCallbacksMethod);
  void ReverseTwoCallbacksMethod(
      bool shouldSucceed,
      std::function<void(std::string)> &&onFailure,
      std::function<void(std::string)> &&onSuccess) noexcept {
    DEBUG_OUTPUT("ReverseTwoCallbacksMethod", shouldSucceed);
    if (shouldSucceed) {
      onSuccess("ReverseTwoCallbacksMethod succeeded");
    } else {
      onFailure("ReverseTwoCallbacksMethod failed");
    }
  }

  // Use callback order as in "classic" ReactNative.
  REACT_METHOD(ReverseTwoCallbacksAsyncMethod);
  winrt::fire_and_forget ReverseTwoCallbacksAsyncMethod(
      bool shouldSucceed,
      std::function<void(std::string)> onFailure,
      std::function<void(std::string)> onSuccess) noexcept {
    DEBUG_OUTPUT("ReverseTwoCallbacksAsyncMethod", shouldSucceed);
    co_await winrt::resume_background();
    if (shouldSucceed) {
      onSuccess("ReverseTwoCallbacksAsyncMethod succeeded");
    } else {
      onFailure("ReverseTwoCallbacksAsyncMethod failed");
    }
  }

  REACT_METHOD(ExplicitPromiseMethod);
  void ExplicitPromiseMethod(winrt::Microsoft::ReactNative::ReactPromise<double> const &result) noexcept {
    DEBUG_OUTPUT("ExplicitPromiseMethod");
    try {
      result.Resolve(M_PI);
    } catch (const std::exception &ex) {
      result.Reject(ex.what());
    }
  }

  REACT_METHOD(ExplicitPromiseMethodWithArgs);
  void ExplicitPromiseMethodWithArgs(
      double arg,
      winrt::Microsoft::ReactNative::ReactPromise<double> const &result) noexcept {
    DEBUG_OUTPUT("ExplicitPromiseMethodWithArgs", arg);
    try {
      result.Resolve(M_PI);
    } catch (const std::exception &ex) {
      result.Reject(ex.what());
    }
  }

  REACT_METHOD(NegateAsyncPromise)
  winrt::fire_and_forget NegateAsyncPromise(int x, ReactPromise<int> result) noexcept {
    co_await winrt::resume_background();
    if (x >= 0) {
      result.Resolve(-x);
    } else {
      result.Reject("Already negative");
    }
  }

#pragma endregion

#pragma region Synchronous Methods

  REACT_SYNC_METHOD(SyncReturnMethod);
  double SyncReturnMethod() noexcept {
    DEBUG_OUTPUT("SyncReturnMethod");
    return M_PI;
  }

  REACT_SYNC_METHOD(SyncReturnMethodWithArgs);
  double SyncReturnMethodWithArgs(double arg) noexcept {
    DEBUG_OUTPUT("SyncReturnMethodWithArgs", arg);
    return M_PI;
  }

#pragma endregion

#pragma region Events

  REACT_EVENT(TimedEvent, L"TimedEventCpp");
  std::function<void(int)> TimedEvent;

  REACT_METHOD(EmitJSEvent1)
  void EmitJSEvent1(int value) noexcept {
    // Test the ReactContext::EmitJSEvent
    m_reactContext.EmitJSEvent(L"RCTDeviceEventEmitter", L"EmitJSEvent1Cpp", value);
  }

  REACT_METHOD(EmitJSEvent2)
  void EmitJSEvent2(int value1, int value2) noexcept {
    // Test the ReactContext::EmitJSEvent
    m_reactContext.EmitJSEvent(L"RCTDeviceEventEmitter", L"EmitJSEvent2Cpp", value1, value2);
  }

  REACT_METHOD(EmitJSEvent3)
  void EmitJSEvent3(int value1, int value2) noexcept {
    // Test the ReactContext::EmitJSEvent
    m_reactContext.EmitJSEvent(
        L"RCTDeviceEventEmitter", L"EmitJSEvent3Cpp", [&](IJSValueWriter const argWriter) noexcept {
          WriteValue(argWriter, value1);
          WriteValue(argWriter, value2);
        });
  }

#pragma endregion

#pragma region JS Functions

  REACT_FUNCTION(CalcDistance, L"calcDistance");
  std::function<void(Point const &, Point const &)> CalcDistance;

  REACT_METHOD(CallDistanceFunction, L"callDistanceFunction");
  void CallDistanceFunction(Point &&point1, Point &&point2) noexcept {
    CalcDistance(point1, point2);
  }

#pragma endregion

  REACT_METHOD(ReloadInstance)
  void ReloadInstance() noexcept {
    ReactNativeHost::FromContext(m_reactContext.Handle()).ReloadInstance();
  }

 private:
  winrt::Windows::System::Threading::ThreadPoolTimer m_timer{nullptr};
  int m_timerCount{0};
  static constexpr std::chrono::milliseconds TimedEventInterval{5000};
  ReactContext m_reactContext;
};

// SampleSharedCppModule shows how to inherited native modules from std::enable_shared_from_this
// to use weak_from_this() in event handlers. In this example we use notifications instead
// of events just to show case the std::weak_ptr use.
REACT_MODULE(SampleSharedCppModule);
struct SampleSharedCppModule : std::enable_shared_from_this<SampleSharedCppModule> {
  using IInspectable = winrt::Windows::Foundation::IInspectable;

  // The Initialize method is called when React instance loaded JavaScript and the module is ready to use.
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept {
    const ReactNotificationId<int> cppTimerNotification{L"SampleModuleCppImpl", L"TimerNotification"};
    const ReactNotificationId<int> csTimerNotification{L"SampleModuleCS", L"TimerNotification"};

    // Note that all notification subscriptions are removed automatically when React instance is unloaded.
    reactContext.Notifications().Subscribe(
        csTimerNotification,
        [weakSelf = weak_from_this()](IInspectable const &, ReactNotificationArgs<int> const &args) noexcept {
          if (auto strongSelf = weakSelf.lock()) {
            strongSelf->ReportCSTimer(*args.Data());
          }
        });

    reactContext.Notifications().Subscribe(
        csTimerNotification,
        [weakSelf = weak_from_this()](IInspectable const &, ReactNotificationArgs<int> const &args) noexcept {
          if (auto strongSelf = weakSelf.lock()) {
            strongSelf->ReportCppTimer(*args.Data());
          }
        });
  }

  void ReportCSTimer(int timerCount) noexcept {
    DEBUG_OUTPUT("SampleSharedCppModule, C# timer:", timerCount);
  }

  void ReportCppTimer(int timerCount) noexcept {
    DEBUG_OUTPUT("SampleSharedCppModule, C++ timer:", timerCount);
  }

  REACT_METHOD(SayHello)
  std::string SayHello() noexcept {
    // This method is currently unused
    return "Hello";
  }
};

} // namespace SampleLibraryCpp
