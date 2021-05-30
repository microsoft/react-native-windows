// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>

namespace facebook {
namespace react {

class Instance;

class BatchingMessageQueueThread : public MessageQueueThread {
 public:
  virtual void onBatchComplete() = 0;
  virtual void decoratedNativeCallInvokerReady(std::weak_ptr<facebook::react::Instance> wkInstance) noexcept = 0;
};

} // namespace react
} // namespace facebook
