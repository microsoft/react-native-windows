// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeModules.h>
#include <atomic>
#include <functional>
#include <string>

namespace Microsoft::React::Test {

enum class TestStatus { Pending = 0, Passed, Failed };

REACT_MODULE(TestModule)
struct TestModule {
  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
    m_reactContext = reactContext;
  }

  REACT_METHOD(markTestCompleted)
  void MarkTestCompleted() noexcept {
    MarkTestPassed(true);
  }

  REACT_METHOD(markTestPassed)
  void MarkTestPassed(bool success) noexcept {
    m_status = success ? TestStatus::Passed : TestStatus::Failed;
  }

  REACT_METHOD(verifySnapshot)
  void VerifySnapshot(std::function<void(bool)> const &callback) noexcept {
    // Snapshot testing is not supported on Windows; always report success.
    callback(true);
  }

  REACT_METHOD(sendAppEvent)
  void SendAppEvent(std::string name, ::React::JSValue body) noexcept {
    // No-op on Windows.
  }

  REACT_METHOD(shouldResolve)
  void ShouldResolve(::React::ReactPromise<int> &&result) noexcept {
    result.Resolve(1);
  }

  REACT_METHOD(shouldReject)
  void ShouldReject(::React::ReactPromise<::React::JSValue> &&result) noexcept {
    result.Reject(::React::ReactError{});
  }

  TestStatus Status() const noexcept {
    return m_status;
  }

 private:
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  std::atomic<TestStatus> m_status{TestStatus::Pending};
};

} // namespace Microsoft::React::Test
