// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Threading/BatchingQueueThread.h"
#include <eventWaitHandle/eventWaitHandle.h>
#include <cassert>

namespace react::uwp {

BatchingQueueThread::BatchingQueueThread(
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept
    : m_queueThread{queueThread} {}

BatchingQueueThread::~BatchingQueueThread() noexcept {}

void BatchingQueueThread::runOnQueue(std::function<void()> &&func) noexcept {
  std::scoped_lock lck(m_mutex);

  EnsureQueue();
  m_taskQueue->emplace_back(std::move(func));

//#define TRACK_UI_CALLS
#ifdef TRACK_UI_CALLS
  char buffer[1024];
  static uint32_t cCalls = 0;
  _snprintf_s(buffer, _countof(buffer), _TRUNCATE, "BatchingUIMessageQueueThread Calls: %u\r\n", ++cCalls);
  OutputDebugStringA(buffer);
#endif
}

void BatchingQueueThread::ThreadCheck() noexcept {
#if DEBUG
  if (m_expectedThreadId == std::thread::id{}) {
    m_expectedThreadId = std::this_thread::get_id();
  } else {
    assert(m_expectedThreadId == std::this_thread::get_id());
  }
#endif
}

void BatchingQueueThread::EnsureQueue() noexcept {
  if (!m_taskQueue) {
    m_taskQueue = std::make_shared<WorkItemQueue>();
    m_taskQueue->reserve(2048);
  }
}

void BatchingQueueThread::PostBatch() noexcept {
  if (m_taskQueue) {
    m_queueThread->runOnQueue([taskQueue{std::move(m_taskQueue)}]() noexcept {
      for (auto &task : *taskQueue) {
        task();
        task = nullptr;
      }
    });
  }
}

void BatchingQueueThread::onBatchComplete() noexcept {
  std::scoped_lock lck(m_mutex);
  PostBatch();
}

void BatchingQueueThread::runOnQueueSync(std::function<void()> && /*func*/) noexcept {
  assert(false && "Not supported");
  std::terminate();
}

void BatchingQueueThread::quitSynchronous() noexcept {
  std::scoped_lock lck(m_mutex);
  PostBatch();
  m_queueThread->quitSynchronous();
}

} // namespace react::uwp
