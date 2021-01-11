// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeModules.h>

namespace IntegrationTest {

using TestCompletedEventId = winrt::Microsoft::ReactNative::ReactNotificationId<void>;
using TestPassedEventId = winrt::Microsoft::ReactNative::ReactNotificationId<bool>;

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
