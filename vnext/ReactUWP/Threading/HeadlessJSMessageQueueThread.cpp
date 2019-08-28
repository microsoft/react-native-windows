// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "HeadlessJSMessageQueueThread.h"
#include <Threading/WorkerMessageQueueThread.h>

using namespace react::uwp;

HeadlessJSMessageQueueThread::HeadlessJSMessageQueueThread(): m_uiThreadQueue(nullptr), m_workerMessageQueue(std::make_unique<WorkerMessageQueueThread>()) { }

HeadlessJSMessageQueueThread::~HeadlessJSMessageQueueThread() {
  m_uiThreadQueue = nullptr;
  m_workerMessageQueue = nullptr;
}

void HeadlessJSMessageQueueThread::runOnQueue(std::function<void()>&& func)
{
  if (m_uiThreadQueue != nullptr)
  {
    m_uiThreadQueue->runOnQueue(std::move(func));
  }
  else
  {
    m_workerMessageQueue->runOnQueue(std::move(func));
  }
}

void HeadlessJSMessageQueueThread::runOnQueueSync(std::function<void()>&& func)
{
  if (m_uiThreadQueue != nullptr)
  {
    m_uiThreadQueue->runOnQueueSync(std::move(func));
  }
  else
  {
    m_workerMessageQueue->runOnQueueSync(std::move(func));
  }
}

void HeadlessJSMessageQueueThread::quitSynchronous()
{
  if (m_uiThreadQueue != nullptr)
  {
    m_uiThreadQueue->quitSynchronous();
  }

  m_workerMessageQueue->quitSynchronous();
}


void HeadlessJSMessageQueueThread::setUIMessageQueue(std::unique_ptr<UIMessageQueueThread> uiMessageQueueThread)
{
  m_uiThreadQueue = std::move(uiMessageQueueThread);
}



HeadlessJSBatchingMessageQueueThread::HeadlessJSBatchingMessageQueueThread(): m_uiThreadQueue(nullptr), m_workerMessageQueue(std::make_unique<WorkerMessageQueueThread>()) { }

HeadlessJSBatchingMessageQueueThread::~HeadlessJSBatchingMessageQueueThread()
{
  m_uiThreadQueue = nullptr;
  m_workerMessageQueue = nullptr;
}

void HeadlessJSBatchingMessageQueueThread::runOnQueue(std::function<void()>&& func)
{
  if (m_uiThreadQueue != nullptr)
  {
    m_uiThreadQueue->runOnQueue(std::move(func));
  }
  else
  {
    m_workerMessageQueue->runOnQueue(std::move(func));
  }
}

void HeadlessJSBatchingMessageQueueThread::runOnQueueSync(std::function<void()>&& func)
{
  if (m_uiThreadQueue != nullptr)
  {
    m_uiThreadQueue->runOnQueueSync(std::move(func));
  }
  else
  {
    m_workerMessageQueue->runOnQueueSync(std::move(func));
  }
}

void HeadlessJSBatchingMessageQueueThread::quitSynchronous()
{
  if (m_uiThreadQueue != nullptr)
  {
    m_uiThreadQueue->quitSynchronous();
  }

  m_workerMessageQueue->quitSynchronous();
}

void HeadlessJSBatchingMessageQueueThread::onBatchComplete()
{
  if (m_uiThreadQueue != nullptr)
  {
    m_uiThreadQueue->onBatchComplete();
  }
}

void HeadlessJSBatchingMessageQueueThread::setUIMessageQueue(std::unique_ptr<BatchingUIMessageQueueThread> uiMessageQueueThread)
{
  m_uiThreadQueue = std::move(uiMessageQueueThread);
}
