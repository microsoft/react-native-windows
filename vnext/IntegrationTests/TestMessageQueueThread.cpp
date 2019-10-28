// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "TestMessageQueueThread.h"

#include <assert.h>

namespace Microsoft::React::Test {

#pragma region TestMessageQueueThread members

TestMessageQueueThread::Lock::Lock(HANDLE mutex) noexcept : m_mutex{mutex} {
  assert(mutex != NULL);
  DWORD waitResult = WaitForSingleObject(m_mutex, INFINITE);
  assert(waitResult == WAIT_OBJECT_0);
}

TestMessageQueueThread::Lock::~Lock() noexcept {
  ReleaseMutex(m_mutex);
  m_mutex = NULL;
}

TestMessageQueueThread::TestMessageQueueThread(
    VoidFunctor &&initializeThread,
    VoidFunctor &&uninitializeThread) noexcept
    : m_initializeThread{move(initializeThread)}, m_uninitializeThread{move(uninitializeThread)} {
  m_creatorThreadId = GetCurrentThreadId();

  m_queueMutex = CreateMutex(
      /* lpMutexAttributes */ NULL,
      /* bInitialOwner */ FALSE,
      /* lpName */ nullptr);
  assert(m_queueMutex != NULL);

  for (int i = 0; i < static_cast<int>(ThreadSignalIndex::Count); ++i) {
    m_threadSignals[i] = CreateEvent(
        /* lpEventAttributes */ NULL,
        /* bManualReset */ FALSE,
        /* bInitialState */ FALSE,
        /* lpName */ nullptr);
    assert(m_threadSignals[i] != NULL);
  }

  m_workerThread = CreateThread(
      /* lpThreadAttributes */ NULL,
      /* dwStackSize */ 0,
      /* ThreadProc lpStartAddress */ TestMessageQueueThread::Dispatch,
      /* lpParameter */ this,
      /* dwCreationFlags */ 0,
      /* lpThreadId */ nullptr);

  assert(m_workerThread != NULL);
}

TestMessageQueueThread::~TestMessageQueueThread() {
  quitInternal();

  for (int i = 0; i < static_cast<int>(ThreadSignalIndex::Count); ++i) {
    assert(m_threadSignals[i] != NULL);
    CloseHandle(m_threadSignals[i]);
    m_threadSignals[i] = NULL;
  }

  assert(m_queueMutex != NULL);
  CloseHandle(m_queueMutex);
  m_queueMutex = NULL;
}

void TestMessageQueueThread::runOnQueue(VoidFunctor &&func) noexcept {
  assert(m_state == State::Running);

  Lock queueLock(m_queueMutex);
  m_queue.push(move(func));
  SetEvent(m_threadSignals[static_cast<int>(ThreadSignalIndex::FunctorAvailable)]);
}

// runOnQueueSync and quitSynchronous are dangerous.  They should only be
// used for initialization and cleanup.
void TestMessageQueueThread::runOnQueueSync(VoidFunctor &&func) noexcept {
  assert(m_state == State::Running);

  if (IsWorkerThread()) {
    func();
  } else {
    HANDLE done = CreateEvent(NULL, FALSE, FALSE, nullptr);
    assert(done != NULL);

    {
      Lock queueLock(m_queueMutex);
      m_queue.push(move(func));
      m_queue.push([done]() { SetEvent(done); });
      SetEvent(m_threadSignals[static_cast<int>(ThreadSignalIndex::FunctorAvailable)]);
    }

    DWORD waitResult = WaitForSingleObject(done, INFINITE);
    assert(waitResult == WAIT_OBJECT_0);
    CloseHandle(done);
  }
}

// Once quitSynchronous() returns, no further work should run on the queue.
void TestMessageQueueThread::quitSynchronous() noexcept {
  assert(m_state == State::Running);
  m_state = State::QuitSynchronousHasBeenCalled;
}

void TestMessageQueueThread::quitInternal() noexcept {
  assert(GetCurrentThreadId() == m_creatorThreadId);

  if (m_state != State::WorkerThreadHasExited) {
    SetEvent(m_threadSignals[static_cast<int>(ThreadSignalIndex::QuitRequested)]);
    DWORD waitResult = WaitForSingleObject(m_workerThread, INFINITE);
    assert(waitResult == WAIT_OBJECT_0);
    m_state = State::WorkerThreadHasExited;
    m_workerThread = NULL;
    assert(m_workerThread == NULL);
    assert(m_state == State::WorkerThreadHasExited);
  }
}

/*static*/ DWORD WINAPI TestMessageQueueThread::Dispatch(LPVOID lpParameter) noexcept {
  TestMessageQueueThread *instance = static_cast<TestMessageQueueThread *>(lpParameter);
  assert(instance != nullptr);

  if (instance->m_initializeThread) {
    instance->m_initializeThread();
  }

  while (instance->m_continueDispatchLoop) {
    DWORD waitResult = WaitForMultipleObjects(
        /* nCount */ static_cast<DWORD>(ThreadSignalIndex::Count),
        /* lpHandles */ instance->m_threadSignals,
        /* bWaitAll */ FALSE,
        /* dwMilliseconds */ INFINITE);

    switch (waitResult) {
      case static_cast<DWORD>(ThreadSignalIndex::FunctorAvailable): {
        VoidFunctor func;
        {
          Lock queueLock(instance->m_queueMutex);
          assert(instance->m_queue.size() > 0);
          func = instance->m_queue.front();
          instance->m_queue.pop();
          if (instance->m_queue.size() > 0) {
            SetEvent(instance->m_threadSignals[static_cast<int>(ThreadSignalIndex::FunctorAvailable)]);
          }
        }
        func();
      } break;

      case static_cast<DWORD>(ThreadSignalIndex::QuitRequested):
        instance->m_continueDispatchLoop = false;
        break;

      default:
        assert(false);
        break;
    }
  }

  if (instance->m_uninitializeThread) {
    instance->m_uninitializeThread();
  }

  return 0;
}

bool TestMessageQueueThread::IsWorkerThread() {
  return m_workerThread != NULL && GetCurrentThreadId() == GetThreadId(m_workerThread);
}

#pragma endregion namespace TestMessageQueueThread members

} // namespace Microsoft::React::Test
