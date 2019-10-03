// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <Threading/WorkerMessageQueueThread.h>
#include <memory>
#include "ProxyMessageQueueThread.h"

using namespace react::uwp;

ProxyMessageQueueThread::ProxyMessageQueueThread()
    : m_uiThreadQueue(nullptr),
      m_workerMessageQueue(std::make_unique<WorkerMessageQueueThread>()) {}

ProxyMessageQueueThread::~ProxyMessageQueueThread() {
  m_uiThreadQueue = nullptr;
  m_workerMessageQueue = nullptr;
}

void ProxyMessageQueueThread::runOnQueue(std::function<void()> &&func) {
  if (m_uiThreadQueue != nullptr) {
    m_uiThreadQueue->runOnQueue(std::move(func));
  } else {
    m_workerMessageQueue->runOnQueue(std::move(func));
  }
}

void ProxyMessageQueueThread::runOnQueueSync(
    std::function<void()> &&func) {
  if (m_uiThreadQueue != nullptr) {
    m_uiThreadQueue->runOnQueueSync(std::move(func));
  } else {
    m_workerMessageQueue->runOnQueueSync(std::move(func));
  }
}

void ProxyMessageQueueThread::quitSynchronous() {
  if (m_uiThreadQueue != nullptr) {
    m_uiThreadQueue->quitSynchronous();
  }

  m_workerMessageQueue->quitSynchronous();
}

void ProxyMessageQueueThread::onBatchComplete() {
  if (m_uiThreadQueue != nullptr && m_isBatching) {
    static_cast<BatchingMessageQueueThread *>(m_uiThreadQueue.get())
        ->onBatchComplete();
  }
}

void ProxyMessageQueueThread::setUIMessageQueue(
    bool isBatching,
    std::unique_ptr<MessageQueueThread> uiMessageQueueThread) {
  m_isBatching = isBatching;
  m_uiThreadQueue = std::move(uiMessageQueueThread);
}
