// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>

namespace facebook {
namespace react {

class BatchingMessageQueueThread : public MessageQueueThread {
 public:
  virtual void onBatchComplete() = 0;
};

} // namespace react
} // namespace facebook
