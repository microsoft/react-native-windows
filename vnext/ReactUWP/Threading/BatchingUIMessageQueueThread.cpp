// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "Threading/BatchingUIMessageQueueThread.h"

#include <winrt/Windows.UI.Core.h>

namespace react {
namespace uwp {

BatchingUIMessageQueueThread::BatchingUIMessageQueueThread(
    winrt::Windows::UI::Core::CoreDispatcher dispatcher)
    : m_uiDispatcher(dispatcher) {}

BatchingUIMessageQueueThread::~BatchingUIMessageQueueThread() {}

void BatchingUIMessageQueueThread::runOnQueue(std::function<void()> &&func) {
  threadCheck();
  ensureQueue();
  m_queue->emplace_back(std::move(func));

//#define TRACK_UI_CALLS
#ifdef TRACK_UI_CALLS
  char buffer[1024];
  static uint32_t cCalls = 0;
  _snprintf_s(
      buffer,
      _countof(buffer),
      _TRUNCATE,
      "BatchingUIMessageQueueThread Calls: %u\r\n",
      ++cCalls);
  OutputDebugStringA(buffer);
#endif
}

void BatchingUIMessageQueueThread::threadCheck() {
#if DEBUG
  if (m_expectedThreadId == 0) {
    m_expectedThreadId = GetCurrentThreadId();
  } else {
    assert(m_expectedThreadId == GetCurrentThreadId());
  }
#endif
}

void BatchingUIMessageQueueThread::ensureQueue() {
  if (m_queue == nullptr) {
    m_queue = std::make_unique<WorkItemQueue>();
    m_queue->reserve(2048);
  }
}

void BatchingUIMessageQueueThread::onBatchComplete() {
  threadCheck();
  std::shared_ptr<WorkItemQueue> queue = m_queue;
  m_queue = nullptr;
  if (queue) {
    m_uiDispatcher.RunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
        [queue{std::move(queue)}]() {
          for (auto &func : *queue) {
            func();
          }
        });
  }
}

void BatchingUIMessageQueueThread::runOnQueueSync(
    std::function<void()> &&func) {
  // Not supported
  // TODO: crash
}

void BatchingUIMessageQueueThread::quitSynchronous() {
  // Not supported
  // TODO: crash
}

} // namespace uwp
} // namespace react
