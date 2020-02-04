// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/BatchingMessageQueueThread.h>
#include <cxxreact/MessageQueueThread.h>

namespace react::uwp {

std::shared_ptr<facebook::react::MessageQueueThread> MakeJSQueueThread() noexcept;

std::shared_ptr<facebook::react::MessageQueueThread> MakeUIQueueThread() noexcept;

std::shared_ptr<facebook::react::MessageQueueThread> MakeSerialQueueThread() noexcept;

std::shared_ptr<facebook::react::BatchingMessageQueueThread> MakeBatchingQueueThread(
    winrt::Windows::UI::Core::CoreDispatcher const &dispatcher) noexcept;

} // namespace react::uwp
