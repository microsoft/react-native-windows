// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>

#include "NativeModules.h"

namespace SampleLibraryCPP {

// Sample REACT_MODULE

REACT_MODULE(SampleModuleCPP);
struct SampleModuleCPP {
  const std::string Name = "SampleModuleCPP";

  void DebugWriteLine(const std::string &methodName, const std::string &arg) {
    std::string output = Name;
    output.append(".");
    output.append(methodName);
    output.append("(");
    output.append(arg);
    output.append(")\n");
    OutputDebugStringA(output.c_str());
  }

  void DebugWriteLine(const std::string &methodName, double arg) {
    DebugWriteLine(methodName, std::to_string(arg));
  }

  void DebugWriteLine(std::string methodName) {
    DebugWriteLine(methodName, "");
  }

#pragma region Constants

  REACT_CONSTANT(NumberConstant);
  const double NumberConstant = M_PI;

  REACT_CONSTANT(StringConstant);
  const std::string StringConstant = "Hello World";

  REACT_CONSTANT_PROVIDER(ConstantsViaConstantsProvider)
  void ConstantsViaConstantsProvider(const winrt::Microsoft::ReactNative::Bridge::IJSValueWriter &writer) noexcept {
    ::Microsoft::ReactNative::WriteProperty(writer, "NumberConstantViaProvider", M_PI);
    ::Microsoft::ReactNative::WriteProperty(writer, "StringConstantViaProvider", "Hello World");
  }

#pragma endregion

#pragma region Methods

  REACT_METHOD(VoidMethod);
  void VoidMethod() noexcept {
    DebugWriteLine("VoidMethod");
  }

  REACT_METHOD(VoidMethodWithArgs);
  void VoidMethodWithArgs(double arg) noexcept {
    DebugWriteLine("VoidMethodWithArgs", arg);
  }

  REACT_METHOD(ReturnMethod);
  double ReturnMethod() noexcept {
    DebugWriteLine("ReturnMethod");
    return M_PI;
  }

  REACT_METHOD(ReturnMethodWithArgs);
  double ReturnMethodWithArgs(double arg) noexcept {
    DebugWriteLine("ReturnMethodWithArgs", arg);
    return M_PI;
  }

#pragma endregion

#pragma region Methods with ReactCallbacks

  REACT_METHOD(ExplicitCallbackMethod);
  void ExplicitCallbackMethod(std::function<void(double)> &&callback) noexcept {
    DebugWriteLine("ExplicitCallbackMethod");
    callback(M_PI);
  }

  REACT_METHOD(ExplicitCallbackMethodWithArgs);
  void ExplicitCallbackMethodWithArgs(double arg, std::function<void(double)> &&callback) noexcept {
    DebugWriteLine("ExplicitCallbackMethodWithArgs", arg);
    callback(M_PI);
  }

  REACT_METHOD(ExplicitPromiseMethod);
  void ExplicitPromiseMethod(
      std::function<void(double)> &&resolve,
      std::function<void(std::string)> &&reject) noexcept {
    DebugWriteLine("ExplicitPromiseMethod");
    try {
      resolve(M_PI);
    } catch (const std::exception &ex) {
      reject(ex.what());
    }
  }

  REACT_METHOD(ExplicitPromiseMethodWithArgs);
  void ExplicitPromiseMethodWithArgs(
      double arg,
      std::function<void(double)> &&resolve,
      std::function<void(std::string)> &&reject) noexcept {
    DebugWriteLine("ExplicitPromiseMethodWithArgs", arg);
    try {
      resolve(M_PI);
    } catch (const std::exception &ex) {
      reject(ex.what());
    }
  }

#pragma endregion

#pragma region Synchronous Methods

  REACT_SYNC_METHOD(SyncReturnMethod);
  double SyncReturnMethod() noexcept {
    DebugWriteLine("SyncReturnMethod");
    return M_PI;
  }

  REACT_SYNC_METHOD(SyncReturnMethodWithArgs);
  double SyncReturnMethodWithArgs(double arg) noexcept {
    DebugWriteLine("SyncReturnMethodWithArgs", arg);
    return M_PI;
  }

#pragma endregion

#pragma region Events

  REACT_EVENT(TimedEvent, "TimedEventCPP");
  std::function<void(int)> TimedEvent;

#pragma endregion

 public:
  SampleModuleCPP() {
    m_timer = winrt::Windows::System::Threading::ThreadPoolTimer::CreatePeriodicTimer(
        [this](const winrt::Windows::System::Threading::ThreadPoolTimer) noexcept {
          if (TimedEvent) {
            TimedEvent(++m_timerCount);
          }
        },
        std::chrono::milliseconds(TimedEventIntervalMS));
  }

  ~SampleModuleCPP() {
    if (m_timer) {
      m_timer.Cancel();
    }
  }

 private:
  winrt::Windows::System::Threading::ThreadPoolTimer m_timer = nullptr;
  int m_timerCount = 0;
  const int TimedEventIntervalMS = 5000;
};

} // namespace SampleLibraryCPP
