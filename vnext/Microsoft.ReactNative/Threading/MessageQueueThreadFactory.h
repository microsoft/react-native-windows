// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Shared/BatchingMessageQueueThread.h>
#include <cxxreact/MessageQueueThread.h>

namespace react::uwp {

std::shared_ptr<facebook::react::MessageQueueThread> MakeJSQueueThread() noexcept;

std::shared_ptr<facebook::react::MessageQueueThread> MakeUIQueueThread() noexcept;

std::shared_ptr<facebook::react::MessageQueueThread> MakeSerialQueueThread() noexcept;

std::shared_ptr<facebook::react::BatchingMessageQueueThread> MakeBatchingQueueThread(
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept;

} // namespace react::uwp
