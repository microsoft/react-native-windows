// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>

namespace Microsoft::ReactNative {

std::shared_ptr<facebook::react::MessageQueueThread> MakeJSQueueThread() noexcept;

std::shared_ptr<facebook::react::MessageQueueThread> MakeUIQueueThread() noexcept;

} // namespace Microsoft::ReactNative
