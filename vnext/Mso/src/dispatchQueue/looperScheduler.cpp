// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Microsoft.ReactNative/IReactDispatcher.h"
#include "dispatchQueue/dispatchQueue.h"
#include "eventWaitHandle/eventWaitHandle.h"
#include "queueService.h"

namespace Mso {

struct LooperScheduler : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IDispatchQueueScheduler> {
  LooperScheduler(winrt::Microsoft::ReactNative::IReactNotificationService notificationService) noexcept;
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
  winrt::Microsoft::ReactNative::IReactNotificationService m_notificationService;
  Mso::WeakPtr<IDispatchQueueService> m_queue;
  std::atomic_bool m_isShutdown{false};
  std::thread m_looperThread; // it must be last in the initialization list
};

//=============================================================================
// LooperScheduler implementation
//=============================================================================

LooperScheduler::LooperScheduler(winrt::Microsoft::ReactNative::IReactNotificationService notificationService) noexcept
    : m_notificationService(notificationService),
      m_looperThread([weakSelf = Mso::WeakPtr{this}]() noexcept { RunLoop(weakSelf); }) {}

LooperScheduler::~LooperScheduler() noexcept {
  AwaitTermination();
}

/*static*/ void LooperScheduler::RunLoop(const Mso::WeakPtr<LooperScheduler> &weakSelf) noexcept {
  for (;;) {
    if (auto self = weakSelf.GetStrongPtr()) {
      if (auto queue = self->m_queue.GetStrongPtr()) {
        DispatchTask task;
        while (queue->TryDequeTask(task)) {
          if (self->m_notificationService) {
            self->m_notificationService.SendNotification(
                winrt::Microsoft::ReactNative::ReactDispatcherHelper::JSDispatcherTaskStartingEventName(), nullptr, nullptr);
          }
          queue->InvokeTask(std::move(task), std::nullopt);
        }
      }

      if (self->m_isShutdown) {
        break;
      }

      if (self->m_notificationService) {
        self->m_notificationService.SendNotification(
            winrt::Microsoft::ReactNative::ReactDispatcherHelper::JSDispatcherIdleWaitStartingEventName(), nullptr, nullptr);
      }
      self->m_wakeUpEvent.Wait();
      self->m_wakeUpEvent.Reset();
      if (self->m_notificationService) {
        self->m_notificationService.SendNotification(
            winrt::Microsoft::ReactNative::ReactDispatcherHelper::JSDispatcherIdleWaitCompletedEventName(), nullptr, nullptr);
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
    winrt::Microsoft::ReactNative::IReactNotificationService notificationService) noexcept {
  return Mso::Make<LooperScheduler, IDispatchQueueScheduler>(notificationService);
}

} // namespace Mso
