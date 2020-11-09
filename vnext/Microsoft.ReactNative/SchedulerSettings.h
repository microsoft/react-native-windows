// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactCommon/RuntimeExecutor.h>
#include "ReactPropertyBag.h"

namespace facebook::react {
class MessageQueueThread;
}

namespace Microsoft::ReactNative::SchedulerSettings {

void SetJsMessageQueueThread(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    const std::shared_ptr<facebook::react::MessageQueueThread> jsMessageQueue) noexcept;
const std::shared_ptr<facebook::react::MessageQueueThread> GetJsMessageQueueThread(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

void SetRuntimeExecutor(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    facebook::react::RuntimeExecutor runtimeExecutor) noexcept;
facebook::react::RuntimeExecutor &GetRuntimeExecutor(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept;

} // namespace Microsoft::ReactNative::SchedulerSettings
