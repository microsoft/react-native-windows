// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "Threading/BatchingQueueThread.h"
#include "crash/verifyElseCrash.h"
#include "debugAssertApi/debugAssertApi.h"

namespace react::uwp {

BatchingQueueThread::BatchingQueueThread(
    std::shared_ptr<facebook::react::MessageQueueThread> const &queueThread) noexcept
    : m_queueThread{queueThread} {}

BatchingQueueThread::~BatchingQueueThread() noexcept {}

void BatchingQueueThread::runOnQueue(std::function<void()> &&func) noexcept {
  threadCheck();
  ensureQueue();
  m_taskQueue->emplace_back(std::move(func));

//#define TRACK_UI_CALLS
#ifdef TRACK_UI_CALLS
  char buffer[1024];
  static uint32_t cCalls = 0;
  _snprintf_s(buffer, _countof(buffer), _TRUNCATE, "BatchingUIMessageQueueThread Calls: %u\r\n", ++cCalls);
  OutputDebugStringA(buffer);
#endif
}

void BatchingQueueThread::threadCheck() noexcept {
#if DEBUG
  if (m_expectedThreadId == std::thread::id{}) {
    m_expectedThreadId = std::this_thread::get_id();
  } else {
    Assert(m_expectedThreadId == std::this_thread::get_id());
  }
#endif
}

void BatchingQueueThread::ensureQueue() noexcept {
  if (!m_taskQueue) {
    m_taskQueue = std::make_shared<WorkItemQueue>();
    m_taskQueue->reserve(2048);
  }
}

void BatchingQueueThread::onBatchComplete() noexcept {
  threadCheck();
  if (m_taskQueue) {
    m_queueThread->runOnQueue([taskQueue{std::move(m_taskQueue)}]() {
      for (auto &task : *taskQueue) {
        task();
        task = nullptr;
      }
    });
  }
}

void BatchingQueueThread::runOnQueueSync(std::function<void()> &&func) noexcept {
  VerifyElseCrashSz(false, "Not supported");
}

void BatchingQueueThread::quitSynchronous() noexcept {
  VerifyElseCrashSz(false, "Not supported");
}

} // namespace react::uwp
