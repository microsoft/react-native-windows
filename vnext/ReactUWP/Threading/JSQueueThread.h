// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <memory>

namespace facebook {
namespace react {
class MessageQueueThread;
}
} // namespace facebook

namespace react {
namespace uwp {

std::shared_ptr<facebook::react::MessageQueueThread> CreateAndStartJSQueueThread();
}
} // namespace react
