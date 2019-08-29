// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "HeadlessJSMessageQueueThread.h"
#include <Threading/WorkerMessageQueueThread.h>
#include <memory>

using namespace react::uwp;

HeadlessJSMessageQueueThread::HeadlessJSMessageQueueThread(bool isBatching)
    : m_uiThreadQueue(nullptr),
      m_workerMessageQueue(std::make_unique<WorkerMessageQueueThread>()),
      m_isBatching(isBatching) {}

HeadlessJSMessageQueueThread::~HeadlessJSMessageQueueThread() {
  m_uiThreadQueue = nullptr;
  m_workerMessageQueue = nullptr;
}

void HeadlessJSMessageQueueThread::runOnQueue(std::function<void()> &&func) {
  if (m_uiThreadQueue != nullptr) {
    m_uiThreadQueue->runOnQueue(std::move(func));
  } else {
    m_workerMessageQueue->runOnQueue(std::move(func));
  }
}

void HeadlessJSMessageQueueThread::runOnQueueSync(
    std::function<void()> &&func) {
  if (m_uiThreadQueue != nullptr) {
    m_uiThreadQueue->runOnQueueSync(std::move(func));
  } else {
    m_workerMessageQueue->runOnQueueSync(std::move(func));
  }
}

void HeadlessJSMessageQueueThread::quitSynchronous() {
  if (m_uiThreadQueue != nullptr) {
    m_uiThreadQueue->quitSynchronous();
  }

  m_workerMessageQueue->quitSynchronous();
}

void HeadlessJSMessageQueueThread::onBatchComplete() {
  if (m_uiThreadQueue != nullptr && m_isBatching) {
    static_cast<BatchingMessageQueueThread *>(m_uiThreadQueue.get())
        ->onBatchComplete();
  }
}

void HeadlessJSMessageQueueThread::setUIMessageQueue(
    std::unique_ptr<MessageQueueThread> uiMessageQueueThread) {
  m_uiThreadQueue = std::move(uiMessageQueueThread);
}
