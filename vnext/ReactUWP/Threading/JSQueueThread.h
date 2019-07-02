// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>

namespace facebook {
namespace react {
class CxxMessageQueue;
}
} // namespace facebook

namespace react {
namespace uwp {

std::shared_ptr<facebook::react::CxxMessageQueue> CreateAndStartJSQueueThread();
}
} // namespace react
