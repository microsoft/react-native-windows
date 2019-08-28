// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/MessageQueueThread.h>
#include <ReactWindowsCore/BatchingMessageQueueThread.h>

namespace react {
  namespace uwp {

    // TALK ABOUT HOW THIS IS A REALLY COOL HACK


    class HeadlessJSMessageQueueThread
      : public facebook::react::BatchingMessageQueueThread {
    public:
      HeadlessJSMessageQueueThread(bool isBatching);
      virtual ~HeadlessJSMessageQueueThread();

      virtual void runOnQueue(std::function<void()>&& func);
      virtual void runOnQueueSync(std::function<void()>&& func);
      virtual void quitSynchronous();
      virtual void onBatchComplete();

      void setUIMessageQueue(std::unique_ptr<MessageQueueThread> uiMessageQueueThread);

    private:
      std::unique_ptr<MessageQueueThread> m_workerMessageQueue;
      std::unique_ptr<MessageQueueThread> m_uiThreadQueue;
      bool m_isBatching;
    };

  } // namespace uwp
} // namespace react
