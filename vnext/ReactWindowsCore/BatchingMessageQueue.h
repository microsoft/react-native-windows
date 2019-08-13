// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>

namespace facebook {
namespace react {

class BatchingMessageQueue : public MessageQueueThread {
 public:
   BatchingMessageQueue();
  virtual ~CxxMessageQueue() override;
  virtual void runOnQueue(std::function<void()> &&) override;
  void runOnQueueDelayed(std::function<void()> &&, uint64_t delayMs);
  // runOnQueueSync and quitSynchronous are dangerous.  They should only be
  // used for initialization and cleanup.
  virtual void runOnQueueSync(std::function<void()> &&) override;
  // Once quitSynchronous() returns, no further work should run on the queue.
  virtual void quitSynchronous() override;

  bool isOnQueue();

  // This returns a function that will actually run the runloop.
  // This runloop will return some time after quitSynchronous (or after this is
  // destroyed).
  //
  // When running the runloop, it is important to ensure that no frames in the
  // current stack have a strong reference to the queue.
  //
  // Only one thread should run the runloop.
  static std::function<void()> getRunLoop(std::shared_ptr<CxxMessageQueue> mq);

  static std::weak_ptr<CxxMessageQueue> current();

 private:
  class QueueRunner;
  std::shared_ptr<QueueRunner> qr_;
};

} // namespace react
} // namespace facebook
