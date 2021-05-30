// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "dispatchQueue/dispatchQueue.h"
#include "queueService.h"

using namespace std::chrono_literals;

namespace Mso {

struct ThreadPoolWorkDeleter {
  void operator()(TP_WORK *tpWork) noexcept;
};

struct ThreadPoolSchedulerWin : Mso::UnknownObject<IDispatchQueueScheduler> {
  ThreadPoolSchedulerWin(uint32_t maxThreads) noexcept;
  ~ThreadPoolSchedulerWin() noexcept override;

  static void __stdcall WorkCallback(
      _Inout_ PTP_CALLBACK_INSTANCE instance,
      _Inout_opt_ PVOID context,
      _Inout_ PTP_WORK work);

 public: // IDispatchQueueScheduler
  void IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept override;
  bool HasThreadAccess() noexcept override;
  bool IsSerial() noexcept override;
  void Post() noexcept override;
  void Shutdown() noexcept override;
  void AwaitTermination() noexcept override;

 private:
  struct ThreadAccessGuard {
    ThreadAccessGuard(ThreadPoolSchedulerWin *scheduler) noexcept;
    ~ThreadAccessGuard() noexcept;

    static bool HasThreadAccess(ThreadPoolSchedulerWin *scheduler) noexcept;

   private:
    ThreadPoolSchedulerWin *m_prevScheduler{nullptr};
    static thread_local ThreadPoolSchedulerWin *tls_scheduler;
  };

 private:
  std::unique_ptr<TP_WORK, ThreadPoolWorkDeleter> m_threadPoolWork;
  Mso::WeakPtr<IDispatchQueueService> m_queue;
  const uint32_t m_maxThreads{1};
  std::atomic<uint32_t> m_usedThreads{0};

  constexpr static uint32_t MaxConcurrentThreads{64};
};

//=============================================================================
// ThreadpoolWorkDeleter implementation
//=============================================================================

void ThreadPoolWorkDeleter::operator()(TP_WORK *tpWork) noexcept {
  if (tpWork != nullptr) {
    ::CloseThreadpoolWork(tpWork);
  }
}

//=============================================================================
// ThreadPoolSchedulerWin implementation
//=============================================================================

ThreadPoolSchedulerWin::ThreadPoolSchedulerWin(uint32_t maxThreads) noexcept
    : m_threadPoolWork{::CreateThreadpoolWork(WorkCallback, this, nullptr)},
      m_maxThreads{maxThreads == 0 ? MaxConcurrentThreads : maxThreads} {}

ThreadPoolSchedulerWin::~ThreadPoolSchedulerWin() noexcept {
  AwaitTermination();
}

/*static*/ void __stdcall ThreadPoolSchedulerWin::WorkCallback(
    _Inout_ PTP_CALLBACK_INSTANCE /*instance*/,
    _Inout_opt_ PVOID context,
    _Inout_ PTP_WORK /*work*/) {
  // The ThreadPoolSchedulerWin is alive here because m_threadPoolWork must be completed before it is destroyed.
  ThreadPoolSchedulerWin *self = static_cast<ThreadPoolSchedulerWin *>(context);

  if (auto queue = self->m_queue.GetStrongPtr()) {
    auto endTime = std::chrono::steady_clock::now() + 100ms;
    DispatchTask task;
    while (queue->TryDequeTask(task)) {
      ThreadAccessGuard guard{self};
      queue->InvokeTask(std::move(task), endTime);

      if (std::chrono::steady_clock::now() > endTime) {
        break;
      }
    }

    --self->m_usedThreads; // We finished using this thread.

    if (queue->HasTasks()) {
      self->Post();
    }
  }
}

void ThreadPoolSchedulerWin::IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept {
  m_queue = std::move(queue);
}

bool ThreadPoolSchedulerWin::HasThreadAccess() noexcept {
  return ThreadAccessGuard::HasThreadAccess(this);
}

bool ThreadPoolSchedulerWin::IsSerial() noexcept {
  return m_maxThreads == 1;
}

void ThreadPoolSchedulerWin::Post() noexcept {
  //! Call SubmitThreadpoolWork if number of used threads is below m_maxThreads
  uint32_t usedThreads = m_usedThreads.load(std::memory_order_relaxed);
  do {
    if (usedThreads == m_maxThreads) {
      return;
    }
  } while (!m_usedThreads.compare_exchange_weak(
      usedThreads, usedThreads + 1, std::memory_order_release, std::memory_order_relaxed));

  ::SubmitThreadpoolWork(m_threadPoolWork.get());
}

void ThreadPoolSchedulerWin::Shutdown() noexcept {
  // It is not used by this scheduler
}

void ThreadPoolSchedulerWin::AwaitTermination() noexcept {
  ::WaitForThreadpoolWorkCallbacks(m_threadPoolWork.get(), false);
}

//=============================================================================
// ThreadPoolSchedulerWin::ThreadAccessGuard implementation
//=============================================================================

/*static*/ thread_local ThreadPoolSchedulerWin *ThreadPoolSchedulerWin::ThreadAccessGuard::tls_scheduler{nullptr};

ThreadPoolSchedulerWin::ThreadAccessGuard::ThreadAccessGuard(ThreadPoolSchedulerWin *scheduler) noexcept
    : m_prevScheduler{tls_scheduler} {
  tls_scheduler = scheduler;
}

ThreadPoolSchedulerWin::ThreadAccessGuard::~ThreadAccessGuard() noexcept {
  tls_scheduler = m_prevScheduler;
}

/*static*/ bool ThreadPoolSchedulerWin::ThreadAccessGuard::HasThreadAccess(ThreadPoolSchedulerWin *scheduler) noexcept {
  return tls_scheduler == scheduler;
}

//=============================================================================
// DispatchQueueStatic::MakeThreadPoolScheduler implementation
//=============================================================================

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeThreadPoolScheduler(
    uint32_t maxThreads) noexcept {
  return Mso::Make<ThreadPoolSchedulerWin, IDispatchQueueScheduler>(maxThreads);
}

} // namespace Mso
