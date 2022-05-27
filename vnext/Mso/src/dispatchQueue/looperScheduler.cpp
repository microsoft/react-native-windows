// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "dispatchQueue/dispatchQueue.h"
#include "eventWaitHandle/eventWaitHandle.h"
#include "queueService.h"

namespace Mso {

struct LooperScheduler : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IDispatchQueueScheduler> {
  LooperScheduler(DispatchQueueSettings const &settings = {}) noexcept;
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
  DispatchQueueSettings m_settings;
  Mso::WeakPtr<IDispatchQueueService> m_queue;
  std::atomic_bool m_isShutdown{false};
  std::thread m_looperThread; // it must be last in the initialization list
};

//=============================================================================
// LooperScheduler implementation
//=============================================================================

LooperScheduler::LooperScheduler(DispatchQueueSettings const &settings) noexcept
    : m_settings(settings), m_looperThread([weakSelf = Mso::WeakPtr{this}]() noexcept { RunLoop(weakSelf); }) {}

LooperScheduler::~LooperScheduler() noexcept {
  AwaitTermination();
}

/*static*/ void LooperScheduler::RunLoop(const Mso::WeakPtr<LooperScheduler> &weakSelf) noexcept {
  for (;;) {
    if (auto self = weakSelf.GetStrongPtr()) {
      if (auto queue = DispatchQueue{self->m_queue.GetStrongPtr()}) {
        for (;;) {
          DispatchTask task;
          if (!(*GetRawState(queue))->TryDequeTask(task)) {
            break;
          }

          if (auto &func = self->m_settings.TaskStarting) {
            func(queue);
          }

          (*GetRawState(queue))->InvokeTask(std::move(task), std::nullopt);

          if (auto &func = self->m_settings.TaskCompleted) {
            func(queue);
          }
        }
      }

      if (self->m_isShutdown) {
        break;
      }

      if (auto &func = self->m_settings.IdleWaitStarting) {
        if (auto queue = DispatchQueue{self->m_queue.GetStrongPtr()}) {
          func(queue);
        }
      }

      self->m_wakeUpEvent.Wait();
      self->m_wakeUpEvent.Reset();

      if (auto &func = self->m_settings.IdleWaitCompleted) {
        if (auto queue = DispatchQueue{self->m_queue.GetStrongPtr()}) {
          func(queue);
        }
      }

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
    if (m_looperThread.get_id() != std::this_thread::get_id()) {
      m_looperThread.join();
    } else {
      // We are on the same thread. We cannot join because it would crash because of a deadlock.
      // We also cannot allow std::thread destructor to run because it would crash on non-joined thread.
      // So, we just detach and let the underlying system thread finish on its own.
      m_looperThread.detach();
    }
  }
}

//=============================================================================
// DispatchQueueStatic::MakeThreadPoolScheduler implementation
//=============================================================================

/*static*/ Mso::CntPtr<IDispatchQueueScheduler> DispatchQueueStatic::MakeLooperScheduler(
    DispatchQueueSettings const &settings) noexcept {
  return Mso::Make<LooperScheduler, IDispatchQueueScheduler>(settings);
}

} // namespace Mso
