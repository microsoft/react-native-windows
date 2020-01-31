// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "MessageDispatchQueue.h"
#include "Threading/BatchingQueueThread.h"

namespace react::uwp {

std::shared_ptr<facebook::react::MessageQueueThread> MakeJSQueueThread() noexcept {
  return std::make_shared<Mso::React::MessageDispatchQueue>(Mso::DispatchQueue::MakeLooperQueue(), nullptr, nullptr);
}

std::shared_ptr<facebook::react::MessageQueueThread> MakeUIQueueThread() noexcept {
  return std::make_shared<Mso::React::MessageDispatchQueue>(Mso::DispatchQueue::MainUIQueue(), nullptr, nullptr);
}

std::shared_ptr<facebook::react::MessageQueueThread> MakeSerialQueueThread() noexcept {
  return std::make_shared<Mso::React::MessageDispatchQueue>(Mso::DispatchQueue{}, nullptr, nullptr);
}

std::shared_ptr<facebook::react::BatchingMessageQueueThread> MakeBatchingQueueThread(
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept {
  return std::make_shared<BatchingQueueThread>(queueThread);
}

} // namespace react::uwp
