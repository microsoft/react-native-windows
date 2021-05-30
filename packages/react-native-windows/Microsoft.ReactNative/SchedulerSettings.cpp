// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <cxxreact/MessageQueueThread.h>
#include "ReactPropertyBag.h"
#include "SchedulerSettings.h"

namespace Microsoft::ReactNative::SchedulerSettings {

winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<facebook::react::MessageQueueThread>>>
JSMessageQueueThreadProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<facebook::react::MessageQueueThread>>>
      propId{L"ReactNative.Scheduler", L"JSMessageQueueThread"};
  return propId;
}
void SetJsMessageQueueThread(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    const std::shared_ptr<facebook::react::MessageQueueThread> value) noexcept {
  properties.Set(JSMessageQueueThreadProperty(), value);
}

const std::shared_ptr<facebook::react::MessageQueueThread> GetJsMessageQueueThread(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept {
  return properties.Get(JSMessageQueueThreadProperty()).Value();
}

winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<facebook::react::RuntimeExecutor>>
RuntimeExecutorProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<facebook::react::RuntimeExecutor>>
      propId{L"ReactNative.Scheduler", L"RuntimeExecutor"};
  return propId;
}

void SetRuntimeExecutor(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    facebook::react::RuntimeExecutor value) noexcept {
  properties.Set(RuntimeExecutorProperty(), value);
}

facebook::react::RuntimeExecutor &GetRuntimeExecutor(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept {
  return properties.Get(RuntimeExecutorProperty()).Value();
}

} // namespace Microsoft::ReactNative::SchedulerSettings
