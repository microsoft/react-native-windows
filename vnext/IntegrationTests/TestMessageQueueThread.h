// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Windows.h>
#include <cxxreact/MessageQueueThread.h>
#include <atomic>
#include <queue>

namespace Microsoft::React::Test {

class TestMessageQueueThread : public facebook::react::MessageQueueThread {
  class Lock {
   public:
    Lock(HANDLE mutex) noexcept;

    ~Lock() noexcept;

   private:
    HANDLE m_mutex = NULL;
  };

 public:
  using VoidFunctor = std::function<void()>;

  TestMessageQueueThread(VoidFunctor &&initializeThread = nullptr, VoidFunctor &&uninitializeThread = nullptr) noexcept;

#pragma region MessageQueueThread members

  ~TestMessageQueueThread() override;

  void runOnQueue(VoidFunctor &&func) noexcept override;

  // runOnQueueSync and quitSynchronous are dangerous.  They should only be
  // used for initialization and cleanup.
  void runOnQueueSync(VoidFunctor &&func) noexcept override;

  // Once quitSynchronous() returns, no further work should run on the queue.
  void quitSynchronous() noexcept override;

#pragma endregion MessageQueueThread members

 private:
  enum class State { Running, QuitSynchronousHasBeenCalled, WorkerThreadHasExited };

  enum class ThreadSignalIndex {
    FunctorAvailable = 0,
    QuitRequested = 1,
    Count // ensure this is the last member
  };

  static DWORD WINAPI Dispatch(LPVOID lpParameter) noexcept;

  bool IsWorkerThread();

  void quitInternal() noexcept;

  std::atomic<State> m_state{State::Running};
  DWORD m_creatorThreadId;

  VoidFunctor m_initializeThread;
  VoidFunctor m_uninitializeThread;
  HANDLE m_threadSignals[static_cast<long>(ThreadSignalIndex::Count)];
  HANDLE m_workerThread = NULL;
  HANDLE m_queueMutex = NULL;
  bool m_continueDispatchLoop = true;
  std::queue<VoidFunctor> m_queue;
};

} // namespace Microsoft::React::Test
