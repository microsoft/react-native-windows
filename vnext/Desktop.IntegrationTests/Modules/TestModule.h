// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <windows.h>

#include <NativeModules.h>

// Standard Library
#include <atomic>
#include <functional>
#include <string>

namespace Microsoft::React::Test {

enum class TestStatus { Pending = 0, Passed, Failed };

REACT_MODULE(TestModule)
struct TestModule {
  // Static test signaling - call Reset() before each test.
  static void Reset() noexcept {
    ResetEvent(s_completed.get());
    s_status = TestStatus::Pending;
  }

  static TestStatus AwaitCompletion(DWORD timeoutMs = INFINITE) noexcept {
    WaitForSingleObject(s_completed.get(), timeoutMs);
    return s_status;
  }

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
    m_reactContext = reactContext;
  }

  REACT_METHOD(MarkTestCompleted, L"markTestCompleted")
  void MarkTestCompleted() noexcept {
    MarkTestPassed(true);
  }

  REACT_METHOD(MarkTestPassed, L"markTestPassed")
  void MarkTestPassed(bool success) noexcept {
    s_status = success ? TestStatus::Passed : TestStatus::Failed;
    SetEvent(s_completed.get());
  }

  REACT_METHOD(VerifySnapshot, L"verifySnapshot")
  void VerifySnapshot(std::function<void(bool)> const &callback) noexcept {
    // Snapshot testing is not supported on Windows; always report success.
    callback(true);
  }

  REACT_METHOD(SendAppEvent, L"sendAppEvent")
  void SendAppEvent(std::string name, ::React::JSValue body) noexcept {
    m_reactContext.EmitJSEvent(L"RCTDeviceEventEmitter", winrt::to_hstring(name), body);
  }

  REACT_METHOD(ShouldResolve, L"shouldResolve")
  void ShouldResolve(::React::ReactPromise<int> &&result) noexcept {
    result.Resolve(1);
  }

  REACT_METHOD(ShouldReject, L"shouldReject")
  void ShouldReject(::React::ReactPromise<::React::JSValue> &&result) noexcept {
    result.Reject(::React::ReactError{});
  }

  TestStatus Status() const noexcept {
    return s_status;
  }

 private:
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;

  static inline std::atomic<TestStatus> s_status{TestStatus::Pending};
  static inline winrt::handle s_completed{CreateEvent(nullptr, TRUE /*manualReset*/, FALSE /*initialState*/, nullptr)};
};

} // namespace Microsoft::React::Test
