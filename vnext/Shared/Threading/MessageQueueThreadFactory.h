// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <BatchingMessageQueueThread.h>
#include <cxxreact/MessageQueueThread.h>

namespace Microsoft::ReactNative {

std::shared_ptr<facebook::react::MessageQueueThread> MakeJSQueueThread() noexcept;

std::shared_ptr<facebook::react::MessageQueueThread> MakeUIQueueThread() noexcept;

std::shared_ptr<facebook::react::BatchingMessageQueueThread> MakeBatchingQueueThread(
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept;

} // namespace Microsoft::ReactNative
