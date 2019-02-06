// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "NativeQueueThread.h"

void NativeMessageQueueThread::run(NativeMessageQueueThread* queueThread) {
  while (!queueThread->m_stopped) {
    WaitForSingleObject(queueThread->m_postEvent, INFINITE);
    std::function<void()> func;
    while (queueThread->m_queue->read(func)) {
      try {
        func();
      } catch (std::exception& e) {
        queueThread->m_exceptionHandler(e);
      }
    }
  }
}

NativeMessageQueueThread::NativeMessageQueueThread(std::function<void(std::exception&)>&& exceptionHandler)
    : m_exceptionHandler(std::move(exceptionHandler)) {
  m_queue = std::make_unique<folly::ProducerConsumerQueue<std::function<void()>>>(1024);
  m_postEvent = CreateEvent(NULL, false, false, NULL);
  m_thread = std::thread(run, this);
}

void NativeMessageQueueThread::runOnQueue(std::function<void()>&& func) {
  m_queue->write(func);
  SetEvent(m_postEvent);
}


// runOnQueueSync and quitSynchronous are dangerous.  They should only be
// used for initialization and cleanup.
void NativeMessageQueueThread::runOnQueueSync(std::function<void()>&& func) {
  if (m_thread.get_id() == std::this_thread::get_id()) {
    std::function<void()> func;
    while (m_queue->read(func)) {
      try {
        func();
        m_numberPrePopped++;
      } catch (std::exception& e) {
        m_exceptionHandler(e);
      }
    }
    return;
  }

  HANDLE callbackFinished = CreateEvent(NULL, false, false, NULL);

  runOnQueue([func, &callbackFinished]() noexcept {
    func();
    SetEvent(callbackFinished);
  });

  WaitForSingleObject(callbackFinished, INFINITE);
}

// Once quitSynchronous() returns, no further work should run on the queue.
void NativeMessageQueueThread::quitSynchronous() {
  m_stopped = true;
  runOnQueueSync([]()
    {});
}
