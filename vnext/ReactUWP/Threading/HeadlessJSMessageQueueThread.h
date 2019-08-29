// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/BatchingMessageQueueThread.h>
#include <cxxreact/MessageQueueThread.h>

namespace react {
namespace uwp {

/// This is a Message Queue Thread that wraps two seperate Message Queue
/// Threads. The idea is to allow starting a react instance before a Window or
/// UI Dispatcher are available (running background tasks) Then once
/// AttachRootView is called a new Message Queue Thread can be provided with
/// setUIMessageQueue.

class HeadlessJSMessageQueueThread
    : public facebook::react::BatchingMessageQueueThread {
 public:
  HeadlessJSMessageQueueThread(bool isBatching);
  virtual ~HeadlessJSMessageQueueThread();

  virtual void runOnQueue(std::function<void()> &&func);
  virtual void runOnQueueSync(std::function<void()> &&func);
  virtual void quitSynchronous();
  virtual void onBatchComplete();

  void setUIMessageQueue(
      std::unique_ptr<MessageQueueThread> uiMessageQueueThread);

 private:
  std::unique_ptr<MessageQueueThread> m_workerMessageQueue;
  std::unique_ptr<MessageQueueThread> m_uiThreadQueue;
  bool m_isBatching;
};

} // namespace uwp
} // namespace react
