// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Threading/UIMessageQueueThread.h>
#include <cxxreact/MessageQueueThread.h>
#include <ReactWindowsCore/BatchingMessageQueueThread.h>
#include <ReactUWP/Threading/BatchingUIMessageQueueThread.h>

namespace react {
  namespace uwp {

    // TALK ABOUT HOW THIS IS A REALLY COOL HACK


    class HeadlessJSMessageQueueThread : public facebook::react::MessageQueueThread {
    public:
      HeadlessJSMessageQueueThread();
      virtual ~HeadlessJSMessageQueueThread();

      virtual void runOnQueue(std::function<void()>&& func);
      virtual void runOnQueueSync(std::function<void()>&& func);
      virtual void quitSynchronous();

      void setUIMessageQueue(std::unique_ptr<UIMessageQueueThread> uiMessageQueueThread);

    private:
      std::unique_ptr<MessageQueueThread> m_workerMessageQueue;
      std::unique_ptr<UIMessageQueueThread> m_uiThreadQueue;
    };

    class HeadlessJSBatchingMessageQueueThread : public facebook::react::BatchingMessageQueueThread {
    public:
      HeadlessJSBatchingMessageQueueThread();
      virtual ~HeadlessJSBatchingMessageQueueThread();

      virtual void runOnQueue(std::function<void()>&& func);
      virtual void runOnQueueSync(std::function<void()>&& func);
      virtual void quitSynchronous();
      virtual void onBatchComplete();

      void setUIMessageQueue(std::unique_ptr<BatchingUIMessageQueueThread> uiMessageQueueThread);

    private:
      std::unique_ptr<MessageQueueThread> m_workerMessageQueue;
      std::unique_ptr<BatchingUIMessageQueueThread> m_uiThreadQueue;
    };

  } // namespace uwp
} // namespace react
