// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "BatchingUIMessageQueueThread.h"
#include <winrt/Windows.UI.Core.h>

namespace react {
namespace uwp {

BatchingUIMessageQueueThread::BatchingUIMessageQueueThread(
  winrt::Windows::UI::Core::CoreDispatcher dispatcher)
  : m_uiDispatcher(dispatcher) {}

BatchingUIMessageQueueThread::~BatchingUIMessageQueueThread() {}

void BatchingUIMessageQueueThread::runOnQueue(std::function<void()> &&func) {
  ensureQueue();
  m_queue->push_back(func);
}

void BatchingUIMessageQueueThread::ensureQueue(){
  if (m_queue == nullptr) {
    m_queue = std::make_unique<WorkItemQueue>();
  }
}

void BatchingUIMessageQueueThread::onBatchComplete() {
  std::shared_ptr<WorkItemQueue> queue = m_queue;
  m_queue = nullptr;
  if (queue) {
    m_uiDispatcher.RunAsync(
      winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
      [queue]() {
        for (auto func : *queue) {
          func();
        }

        // TODO:  Notify the UIManager onBatchComplete
//        g_theUIManager->onBatchComplete();

      });
  }
}

void BatchingUIMessageQueueThread::runOnQueueSync(std::function<void()> &&func) {
  // Not supported
}

void BatchingUIMessageQueueThread::quitSynchronous() {
  // Not supported
}

} // namespace uwp
} // namespace react

