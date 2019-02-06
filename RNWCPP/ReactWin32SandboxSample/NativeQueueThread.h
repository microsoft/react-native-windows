// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <list>
#include <functional>
#include <exception>

#include <windows.h>
#include <folly/ProducerConsumerQueue.h>

class NativeMessageQueueThread : public facebook::react::MessageQueueThread
{
public:
  NativeMessageQueueThread(std::function<void(std::exception&)>&& exceptionHandler);
  virtual ~NativeMessageQueueThread() = default;

  // MessageQueueThread interface
  void runOnQueue(std::function<void()>&& func) override;

  // runOnQueueSync and quitSynchronous are dangerous.  They should only be
  // used for initialization and cleanup.
  void runOnQueueSync(std::function<void()>&& func) override;

  // Once quitSynchronous() returns, no further work should run on the queue.
  void quitSynchronous() override;

private:
  static void run(NativeMessageQueueThread *queueThread);

  std::atomic<bool> m_stopped = false;
  std::unique_ptr<folly::ProducerConsumerQueue<std::function<void()>>> m_queue;
  HANDLE m_postEvent;
  unsigned m_numberPrePopped = 0; // Number of items that have been run synchronously - causing the m_queue to get out of sync with the FastModel::Thread
  std::thread m_thread;
  std::function<void(std::exception&)> m_exceptionHandler;
};
