// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <memory>

namespace react::uwp {

std::shared_ptr<facebook::react::MessageQueueThread> CreateAndStartJSQueueThread() noexcept;

} // namespace react::uwp
