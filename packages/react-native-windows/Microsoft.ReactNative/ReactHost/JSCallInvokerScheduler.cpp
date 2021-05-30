// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "JSCallInvokerScheduler.h"
#include <ReactCommon/CallInvoker.h>
#include <Threading/MessageDispatchQueue.h>
#include <cxxreact/MessageQueueThread.h>
#include "eventWaitHandle/eventWaitHandle.h"

namespace Mso {

struct JSCallInvokerScheduler
    : Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, IDispatchQueueScheduler, IJSCallInvokerQueueScheduler> {
  JSCallInvokerScheduler(
      std::shared_ptr<facebook::react::CallInvoker> &&callInvoker,
      Mso::Functor<void(const Mso::ErrorCode &)> &&errorHandler,
      Mso::Promise<void> &&whenQuit) noexcept;
  ~JSCallInvokerScheduler() noexcept override;

 public:
  // IDispatchQueueScheduler
  void IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept override;
  bool HasThreadAccess() noexcept override;
  bool IsSerial() noexcept override;
  void Post() noexcept override;
  void Shutdown() noexcept override;
  void AwaitTermination() noexcept override;

  // IJSCallInvokerQueueScheduler
  std::shared_ptr<facebook::react::MessageQueueThread> GetMessageQueue() noexcept override;

 private:
  std::shared_ptr<Mso::React::MessageDispatchQueue> m_jsMessageThread;

  Mso::WeakPtr<IDispatchQueueService> m_queue;
  std::shared_ptr<facebook::react::CallInvoker> m_callInvoker;
};

std::shared_ptr<facebook::react::MessageQueueThread> JSCallInvokerScheduler::GetMessageQueue() noexcept {
  return m_jsMessageThread;
}

//=============================================================================
// JSCallInvokerScheduler implementation
//=============================================================================

JSCallInvokerScheduler::JSCallInvokerScheduler(
    std::shared_ptr<facebook::react::CallInvoker> &&callInvoker,
    Mso::Functor<void(const Mso::ErrorCode &)> &&errorHandler,
    Mso::Promise<void> &&whenQuit) noexcept
    : m_callInvoker(callInvoker) {
  m_jsMessageThread = std::make_shared<Mso::React::MessageDispatchQueue>(
      Mso::DispatchQueue::MakeLooperQueue(), std::move(errorHandler), std::move(whenQuit));
}

JSCallInvokerScheduler::~JSCallInvokerScheduler() noexcept {
  AwaitTermination();
}

void JSCallInvokerScheduler::IntializeScheduler(Mso::WeakPtr<IDispatchQueueService> &&queue) noexcept {
  m_queue = std::move(queue);
}

bool JSCallInvokerScheduler::HasThreadAccess() noexcept {
  return m_jsMessageThread->DispatchQueue().HasThreadAccess();
}

bool JSCallInvokerScheduler::IsSerial() noexcept {
  return true;
}

void JSCallInvokerScheduler::Post() noexcept {
  m_jsMessageThread->DispatchQueue().Post([wkThis = Mso::WeakPtr<JSCallInvokerScheduler>(this)]() {
    if (auto stringThis = wkThis.GetStrongPtr()) {
      if (auto queue = stringThis->m_queue.GetStrongPtr()) {
        Mso::DispatchTask task;
        if (queue->TryDequeTask(task)) {
          stringThis->m_callInvoker->invokeAsync(std::move(task));
        }
      }
    }
  });
}

void JSCallInvokerScheduler::Shutdown() noexcept {
  m_jsMessageThread->DispatchQueue().Shutdown(PendingTaskAction::Complete);
}

void JSCallInvokerScheduler::AwaitTermination() noexcept {
  m_jsMessageThread->DispatchQueue().AwaitTermination();
}

Mso::CntPtr<IDispatchQueueScheduler> MakeJSCallInvokerScheduler(
    std::shared_ptr<facebook::react::CallInvoker> &&callInvoker,
    Mso::Functor<void(const Mso::ErrorCode &)> &&errorHandler,
    Mso::Promise<void> &&whenQuit) noexcept {
  return Mso::Make<JSCallInvokerScheduler, IDispatchQueueScheduler>(
      std::move(callInvoker), std::move(errorHandler), std::move(whenQuit));
}

} // namespace Mso
