// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

#include "DebugHelpers.h"
#include "NativeModules.h"

#define DEBUG_OUTPUT(...) DebugWriteLine("SampleModuleCppImpl", ##__VA_ARGS__);

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

REACT_MODULE(SampleModuleCppImpl, L"SampleModuleCpp");
struct SampleModuleCppImpl {
#pragma region Initialization

  REACT_INIT(Initialize)
  void Initialize(ReactContext const &reactContext) noexcept {
    PropertyName<int> myProp1{L"Prop1"};
    PropertyName<winrt::hstring> myProp2{L"Prop2"};

    DEBUG_OUTPUT("globalProps.Prop1:", *reactContext.GlobalProperties().Get(myProp1));
    DEBUG_OUTPUT("instanceProps.Prop2:", winrt::to_string(*reactContext.InstanceProperties().Get(myProp2)));

    m_timer = winrt::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
        [this](const winrt::Windows::System::Threading::ThreadPoolTimer) noexcept {
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

#pragma endregion

#pragma region JS Functions

  REACT_FUNCTION(CalcDistance, L"calcDistance");
  std::function<void(Point const &, Point const &)> CalcDistance;

  REACT_METHOD(CallDistanceFunction, L"callDistanceFunction");
  void CallDistanceFunction(Point &&point1, Point &&point2) noexcept {
    CalcDistance(point1, point2);
  }

#pragma endregion

 private:
  winrt::Windows::System::Threading::ThreadPoolTimer m_timer{nullptr};
  int m_timerCount{0};
  static constexpr std::chrono::milliseconds TimedEventInterval{5000};
};

} // namespace SampleLibraryCpp
