// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "TestModule.h"

using namespace winrt::Microsoft::ReactNative;

namespace IntegrationTest {

/*static*/ ReactNotificationId<void> TestModule::TestCompletedEvent() noexcept {
  return {L"TestModule", L"TestCompleted"};
}

/*static*/ ReactNotificationId<bool> TestModule::TestPassedEvent() noexcept {
  return {L"TestModule", L"TestPassed"};
}

void TestModule::Init(const ReactContext &ctx) noexcept {
  m_context = ctx;
}

void TestModule::markTestCompleted() noexcept {
  m_context.Notifications().SendNotification(TestCompletedEvent());
}

void TestModule::markTestPassed(bool passed) noexcept {
  m_context.Notifications().SendNotification(TestPassedEvent(), passed);
}

void TestModule::verifySnapshot(std::function<void(bool)> callback) noexcept {
  // NYI
  callback(false);
}

void TestModule::shouldResolve(ReactPromise<void> promise) noexcept {
  promise.Resolve();
}

void TestModule::shouldReject(ReactPromise<void> promise) noexcept {
  promise.Resolve();
}

void TestModule::sendAppEvent(
    const std::string eventName,
    const winrt::Microsoft::ReactNative::JSValueObject &event) noexcept {
  m_context.EmitJSEvent(L"RCTNativeAppEventEmitter", winrt::to_hstring(eventName), event);
}

} // namespace IntegrationTest
