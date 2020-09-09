// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeModules.h>

namespace IntegrationTest {

using TestCompletedEventArgs = winrt::Microsoft::ReactNative::ReactNonAbiValue<std::chrono::steady_clock::time_point>;
using TestCompletedEventId = winrt::Microsoft::ReactNative::ReactNotificationId<TestCompletedEventArgs>;
using TestPassedEventArgs =
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::pair<std::chrono::steady_clock::time_point, bool>>;
using TestPassedEventId = winrt::Microsoft::ReactNative::ReactNotificationId<TestPassedEventArgs>;

REACT_MODULE(TestModule)
struct TestModule {
  static TestCompletedEventId TestCompletedEvent() noexcept;
  static TestPassedEventId TestPassedEvent() noexcept;

  REACT_INIT(Init)
  void Init(const winrt::Microsoft::ReactNative::ReactContext &ctx) noexcept;

  REACT_METHOD(markTestCompleted)
  void markTestCompleted() noexcept;

  REACT_METHOD(markTestPassed)
  void markTestPassed(bool passed) noexcept;

  REACT_METHOD(verifySnapshot)
  void verifySnapshot(std::function<void(bool)> callback) noexcept;

  REACT_METHOD(shouldResolve)
  void shouldResolve(winrt::Microsoft::ReactNative::ReactPromise<void> promise) noexcept;

  REACT_METHOD(shouldReject)
  void shouldReject(winrt::Microsoft::ReactNative::ReactPromise<void> promise) noexcept;

  REACT_METHOD(sendAppEvent)
  void sendAppEvent(const std::string eventName, const winrt::Microsoft::ReactNative::JSValueObject &event) noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace IntegrationTest
