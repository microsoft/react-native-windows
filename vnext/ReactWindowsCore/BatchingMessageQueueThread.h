// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>

namespace facebook {
namespace react {

class BatchingMessageQueueThread : public MessageQueueThread {
 public:
   void runOnQueue(std::function<void()>&&) override {}
   void runOnQueueSync(std::function<void()>&&) override {}
   void quitSynchronous() override {}

   virtual void runOnQueueUnbatched(std::function<void()>&&) {}
   virtual void onBatchComplete() {}
};

} // namespace react
} // namespace facebook
