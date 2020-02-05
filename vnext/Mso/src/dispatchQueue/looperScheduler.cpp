// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "dispatchQueue/dispatchQueue.h"
#include "eventWaitHandle/eventWaitHandle.h"
#include "queueService.h"

namespace Mso {

struct LooperScheduler : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IDispatchQueueScheduler> {
  LooperScheduler() noexcept;
  ~LooperScheduler() noexcept override;

  static void RunLoop(const Mso::WeakPtr<LooperScheduler> &weakSelf) noexcept;

 public: // IDispatchQueueScheduler
  void IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept override;
  bool HasThreadAccess() noexcept override;
  bool IsSerial() noexcept override;
  void Post() noexcept override;
  void Shutdown() noexcept override;
  void AwaitTermination() noexcept override;

 private:
  ManualResetEvent m_wakeUpEvent;
  Mso::WeakPtr<IDispatchQueueService> m_queue;
  std::atomic_bool m_isShutdown{false};
  std::thread m_looperThread; // it must be last in the initialization list
};

//=============================================================================
// LooperScheduler implementation
//=============================================================================

LooperScheduler::LooperScheduler() noexcept
    : m_looperThread([weakSelf = Mso::WeakPtr{this}]() noexcept { RunLoop(weakSelf); }) {}

LooperScheduler::~LooperScheduler() noexcept {
  AwaitTermination();
}

/*static*/ void LooperScheduler::RunLoop(const Mso::WeakPtr<LooperScheduler> &weakSelf) noexcept {
  for (;;) {
    if (auto self = weakSelf.GetStrongPtr()) {
      if (auto queue = self->m_queue.GetStrongPtr()) {
        DispatchTask task;
        while (queue->TryDequeTask(task)) {
          queue->InvokeTask(std::move(task), std::nullopt);
        }
      }

      if (self->m_isShutdown) {
        break;
      }

      self->m_wakeUpEvent.Wait();
      self->m_wakeUpEvent.Reset();
      continue;
    }

    break;
  }
}

void LooperScheduler::IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept {
  m_queue = std::move(queue);
}

bool LooperScheduler::HasThreadAccess() noexcept {
  return m_looperThread.get_id() == std::this_thread::get_id();
}

bool LooperScheduler::IsSerial() noexcept {
  return true;
}

void LooperScheduler::Post() noexcept {
  m_wakeUpEvent.Set();
}

void LooperScheduler::Shutdown() noexcept {
  m_isShutdown = true;
  m_wakeUpEvent.Set();
}

void LooperScheduler::AwaitTermination() noexcept {
  Shutdown();
  if (m_looperThread.joinable()) {
    m_looperThread.join();
  }
}

//=============================================================================
// DispatchQueueStatic::MakeThreadPoolScheduler implementation
//=============================================================================

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeLooperScheduler() noexcept {
  return Mso::Make<LooperScheduler, IDispatchQueueScheduler>();
}

} // namespace Mso
