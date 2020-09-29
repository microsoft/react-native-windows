// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "MessageQueueThreadFactory.h"
#include "BatchingQueueThread.h"
#include "MessageDispatchQueue.h"

namespace react::uwp {

std::shared_ptr<facebook::react::MessageQueueThread> MakeJSQueueThread() noexcept {
  return std::make_shared<Mso::React::MessageDispatchQueue>(Mso::DispatchQueue::MakeLooperQueue(), nullptr, nullptr);
}

std::shared_ptr<facebook::react::MessageQueueThread> MakeUIQueueThread() noexcept {
  Mso::DispatchQueue queue = Mso::DispatchQueue::GetCurrentUIThreadQueue();
  std::shared_ptr<facebook::react::MessageQueueThread> messageThread =
      queue ? std::make_shared<Mso::React::MessageDispatchQueue>(queue, nullptr, nullptr) : nullptr;
  return messageThread;
}

std::shared_ptr<facebook::react::BatchingMessageQueueThread> MakeBatchingQueueThread(
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept {
  return std::make_shared<BatchingQueueThread>(queueThread);
}

} // namespace react::uwp
