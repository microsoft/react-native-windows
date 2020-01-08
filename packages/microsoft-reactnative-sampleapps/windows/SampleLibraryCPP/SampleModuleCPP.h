// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

#include "DebugHelpers.h"
#include "NativeModules.h"

#define DEBUG_OUTPUT(...) DebugWriteLine("SampleModuleCppImpl", ##__VA_ARGS__);

namespace SampleLibraryCpp {

// Sample REACT_MODULE

REACT_MODULE(SampleModuleCppImpl, L"SampleModuleCpp");
struct SampleModuleCppImpl {
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

  REACT_METHOD(ExplicitPromiseMethod);
  void ExplicitPromiseMethod(winrt::Microsoft::ReactNative::ReactPromise<double> &&result) noexcept {
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
      winrt::Microsoft::ReactNative::ReactPromise<double> &&result) noexcept {
    DEBUG_OUTPUT("ExplicitPromiseMethodWithArgs", arg);
    try {
      result.Resolve(M_PI);
    } catch (const std::exception &ex) {
      result.Reject(ex.what());
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

 public:
  SampleModuleCppImpl() {
    m_timer = winrt::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
        [this](const winrt::Windows::System::Threading::ThreadPoolTimer) noexcept {
          if (TimedEvent) {
            TimedEvent(++m_timerCount);
          }
        },
        std::chrono::milliseconds(TimedEventIntervalMS));
  }

  ~SampleModuleCppImpl() {
    if (m_timer) {
      m_timer.Cancel();
    }
  }

 private:
  winrt::Windows::System::Threading::ThreadPoolTimer m_timer = nullptr;
  int m_timerCount = 0;
  const int TimedEventIntervalMS = 5000;
};

} // namespace SampleLibraryCpp
